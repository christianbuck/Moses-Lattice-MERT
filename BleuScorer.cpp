#include <vector>
#include <algorithm>
#include <limits>
#include <math.h>
#include <algorithm>
#include <cassert>
#include "Types.h"
#include "BleuScorer.h"

using std::vector;
using std::numeric_limits;

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

void computeBleuStats(const vector<Line>& a, const Phrase& reference, vector<BleuStats>& stats)
{
    size_t K = a.size();
    NgramCounts referenceCounts;
    countNGrams(reference,referenceCounts);

    for (size_t i = 0; i < K; i++) {
        Phrase hyp = a[i].hypothesis;
        NgramCounts hypCounts;
        countNGrams(hyp, hypCounts);

        for (NgramCounts::const_iterator hit = hypCounts.begin(); hit != hypCounts.end(); hit++) {
            NgramCounts::const_iterator rit = referenceCounts.find(hit->first);
            if (rit != referenceCounts.end()) {
                // size_t c = rit->second < hit->second] ? rit->second : hit->second;
                stats[i].counts[hit->first.size()] += std::min(rit->second, hit->second);
            }
        }
        stats[i].length = hyp.size();
        stats[i].leftBoundary = a[i].x;
    }
}

void accumulateBleu(const vector<BleuStats>& stats, vector<boundary>& cumulatedCounts)
{
/* takes BleuStats data for a single sentences and appends difference vectors to cumulatedCounts */
    int nStats = stats.size();
    for (size_t i=0;i<nStats;++i) {
        vector<int> diffs(5);
        int oldCount[5] = {0.0};
        for (size_t n =0; n<4+1;n++) {  // cumulatedCounts[x]->second[4] == lengths
            int curr = n<4 ? stats[i].counts[n] : stats[i].length;
            diffs[n] = curr - oldCount[n];
            oldCount[n] = curr;
        }
        cumulatedCounts.push_back( boundary(stats[i].leftBoundary,diffs) );
    }
}


double Bleu(int p[])
{
    double score = 0.0;
    for (size_t n=0; n<4; n++) {
        score += log(p[n]) - log(p[n+4]);
    }
    return exp(score);
}

void optimizeBleu(const vector<boundary>& cumulatedCounts, Interval& bestInterval) 
{
    int p[8] = {0};
    int nCounts = cumulatedCounts.size();
    
    bestInterval.score = -numeric_limits<double>::max();
    double oldBoundary = -numeric_limits<double>::max();
    
    for (size_t i=0; i<nCounts; i++) {
        const vector<int>& currCounts = cumulatedCounts[i].second;
        double newBoundary = cumulatedCounts[i].first;
        if (oldBoundary != newBoundary) {
            // check if we shall update bestInterval
            double b = Bleu(p);
            if (b > bestInterval.score) {
                bestInterval.score = b;
                bestInterval.left = oldBoundary;
                bestInterval.right = newBoundary;
            }
            oldBoundary = newBoundary;
        }
        for (size_t n=0; n<4; n++) {
            p[n] += currCounts[n];       // clipped ngram count
            int len = currCounts[4];
            p[n+4] += len>n ? len-n : 0; // ngram count
        }
    }
    assert (bestInterval.score > -numeric_limits<double>::max());
}


/* probably not valid anymore ... */
/*
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

*/


