#include <vector>
#include <algorithm>
#include <limits>
#include <math>
#include "Types.h"

using std::vector;
using std::numeric_limits;


// Implementation of Algorithm 1
// Calculates the upper envelope for a set of lines 
// The algorithm modifies the line array in place

void sweepLine(vector<Line> &a)
{
    sort (a.begin(), a.end(), Line::CompareBySlope);

    size_t j = 0;
    size_t K = a.size();
    for (size_t i = 0; i < K; i++) {
        Line &l = a[i];
        l.x = -numeric_limits<double>::max();
        if (0 < j) {
            if (a[j - 1].m == l.m) {
                if (l.y0 <= a[j - 1].y0) continue;
                --j;
            }
            while (0 < j) {
                l.x = (l.y0 - a[j - 1].y0) / (a[j - 1].m - l.m);
                if (a[j - 1].x < l.x) break;
                --j;
            }
            if (0 == j) l.x = -numeric_limits<double>::max();
        } 
        a[j++] = l;
    }
    a.resize(j);
}

vector<Line> latticeEnvelope(const Vertex& source, const int nEdges, const vector<double>& d, const vector<double>& lambdas) 
{
    vector<Line> a;
    vector< vector< Line> > L(nEdges);
    for (v = s to t) {
        a.clear();
        for (size_t i=0; i<v.in.size();++i) {
            const vector<Line>& currentLines = L[v.in[i].idx];
            a.insert(a.end(),currentLines.begin(),currentLines.end() );
        }
        sweepLine(a);
        for (size_t i=0; i<v.in.size();++i) {
            L[v.in[i].idx].clear();
        }

        for (size_t i=0; i<v.out.size();++i) {
            vector<Line>& currentLines =  L[v.out[i].idx];
            currentLines = a;
            for (size_t j=0; j<a.size();++j) {
                currentLines[j].m += dotProduct(v.out[i].features, d);
                currentLines[j].y += dotProduct(v.out[i].features, lambdas);
                currentLines[j].hypothesis.insert(currentLines[j].hypothesis.end(),v.out[i].phrase.begin(), v.out[i].phrase.end()); 
            }
        }
    }
    return a;
}

void countNGrams(const Phrase& reference, NgramCounts& counts) 
{
    size_t referenceSize = reference.size();
    for (size_t n=1; n<=4;n++) {
        for (size_t offset=0; offset<referenceSize-n+1;offset++) {
            Phrase ngram;
            for (size_t pos=offset; pos<offset+n;pos++) {
                ngram.push_back(reference[pos]);
            }
            counts[ngram]++;
        }
    }
}

double dotProduct(const vector<double>& a,const vector<double>& b) 
{
    size_t d = a.size();
    assert(b.size() == d);
    double p = 0.0;
    for (size_t i=0;i<d;++i) {
        p += a[i]*b[i];
    }
}

void computeBleuStats(const vector<line>& a, const Phrase& reference, vector<BleuStats>& stats) 
{
    size_t K = a.size();
    NgramCounts referenceCounts;
    countNGrams(reference,referenceCounts);
    
    for (size_t i = 0; i < K; i++) {
        Phrase hyp = a[i].p;
        NgramCounts hypCounts;
        countNGrams(hyp, hypCounts);

        for (NgramCounts::const_iterator hit = hypCounts.begin(); hit != hypCounts.end(); hit++) {
            Phrase& p = hit->first;
            NgramCounts::const_iterator rit = referenceCounts.find(p)
            if (rit != referenceCounts.end()) {
                // size_t c = rit->second < hit->second] ? rit->second : hit->second;
                stats[i].counts[hit->first.size()] += std::min(rit->second, hit->second);
            }
        }
        stats[i].length = hyp.length();
        stats[i].leftBoundary = a[i].x;
    }
}

void accumulateBleu(const vector<BleuStats>& stats, vector<boundary>& cumulatedCounts) 
{
    int nStats = stats.size();
    for (size_t i=0;i<nStats;++i) {
        vector<int> diffs(5);
        int oldCount = 0;
        for (size_t n =0; n<4+1;n++) {  // cumulatedCounts[x]->second[4] == lengths
            int curr = n<4 ? stats[i].counts[n] : stats[i].length;
            diffs[n] = curr - oldCount;
            oldCount = curr;
        }
        cumulatedCount.push_back( boundary(stats[i].leftBoundary,diffs) );
    }
}


double Bleu(int[] p) 
{
    double score = 0.0;
    for (size_t n=0; n<4; n++) {
        score += log( p[n]/p[n+4] );
    }
    return exp(score);
}

void optimizeBleu(const vector<boundary>& cumulatedCounts, Interval& bestInterval) 
{
    double p[8];
    int nCounts = cumulatedCounts.size();
    
    bestInterval.score = -numeric_limits<double>::max();
    
    double oldBoundary = 0.0;
    for (size_t i=0; i<nCounts; i++) {
        vector<int>& currCounts = cumulatedCounts[i]->second;
        double newBoundary  cumulatedCounts[i]->first;
        if (oldBoundary != newBoundary || i==0) {
            if (i>0) { // check if we shall update bestInterval
                b = Bleu(p);
                if (b > bestInterval.score) {
                    bestInterval.score = b;
                    bestInterval.left = oldBoundary;
                    bestInterval.right = newBoundary;
                }
            }
            for (size n=0;n<8;n++) {
                p[n]=0.0;
            }
            oldBoundary = newBoundary;
        }
        for (size n=0;n<4;n++) {
            p[n] += currCounts[n];      // clipped ngram count
            p[n+4] += std::max(0,currCounts[4]-n); // ngram count
        }
    }
    assert (bestInterval.score > -numeric_limits<double>::max());
}        


void pruneCounts(vector< vector<boundary> >& cumulatedCounts)
{
    vector<boundary>& counts = cumulatedCounts[i];
    std::sort(counts.start(),counts.end());
    double oldBoundary = 0;
    for (vector<boundary>::iterator i=counts.begin();i<counts.end();i++) {
        if (i==counts.begin()) {
            oldBoundary = (*i)->first;
            continue;
        }
        double currBoundary = (*i)->first;
        if (currBoundary==oldBoundary) {
            (*i)->second += (*i)->second; 
            counts.erase(i)
            i--;
        }
    }
}    




