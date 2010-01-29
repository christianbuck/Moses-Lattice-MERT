#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "Types.h"
#include "BleuScorer.h"

using std::vector;
using std::numeric_limits;
using std::cout;
using std::endl;

void countNGrams(const Phrase& reference, NgramCounts& counts)
{
    size_t referenceSize = reference.size();
    size_t maxN = std::min(referenceSize, bleuOrder);
    for (size_t n=1; n<=maxN;n++) {
        for (size_t offset=0; offset+n<referenceSize+1;offset++) {
            Phrase ngram;
            for (size_t pos=offset; pos<offset+n;pos++) {
                ngram.push_back(reference[pos]);
            }
            counts[ngram]++;
        }
    }
}

void computeBleuStats(Lattice &lattice, const vector<Line>& a, const Phrase& reference, vector<BleuStats>& stats)
{
//    cout << "ref: " << reference << endl;
    size_t K = a.size();
    NgramCounts referenceCounts;
    countNGrams(reference,referenceCounts);

    for (size_t i = 0; i < K; i++) {
        Phrase hyp;
        a[i].getHypothesis(lattice, hyp);
        BleuStats lineStats(hyp.size(), a[i].x);
        NgramCounts hypCounts;
        countNGrams(hyp, hypCounts);
        for (NgramCounts::const_iterator hit = hypCounts.begin(); hit != hypCounts.end(); hit++) {
            NgramCounts::const_iterator rit = referenceCounts.find(hit->first);
            if (rit != referenceCounts.end()) {
                // size_t c = rit->second < hit->second] ? rit->second : hit->second;
                lineStats.counts[hit->first.size()-1] += std::min(rit->second, hit->second); // clipped counts
            }
        }
        // lineStats.length = hyp.size();
        // lineStats.leftBoundary = a[i].x;
        // cout << "lstats: l:" << lineStats.length << " lbound: " << lineStats.leftBoundary;
        // cout << " c1 " << lineStats.counts[0] << " c2 " << lineStats.counts[1] << " c3 " << lineStats.counts[2] << " c4 " << lineStats.counts[3] << endl;
        stats.push_back(lineStats);
    }
}

void accumulateBleu(const vector<BleuStats>& stats, vector<boundary>& cumulatedCounts)
{
/* takes BleuStats data for a single sentences and appends difference vectors to cumulatedCounts */
    size_t nStats = stats.size();
    int oldCount[bleuOrder*2] = {0};
    for (size_t i=0;i<nStats;++i) {
        vector<int> diffs(bleuOrder*2);
        int length = stats[i].length;
        for (size_t n =0; n<bleuOrder;n++) {
            int curr = stats[i].counts[n];
            diffs[n] = curr - oldCount[n];
            oldCount[n] = curr;
            int possibleNGrams = std::max(length-(int)n, 0);
            diffs[n+bleuOrder] = possibleNGrams - oldCount[n+bleuOrder];
            oldCount[n+bleuOrder] = possibleNGrams;
        }
        cumulatedCounts.push_back( boundary(stats[i].leftBoundary,diffs) );
    }
}


double Bleu(int p[])
{
    double score = 0.0;
    for (size_t n=0; n<bleuOrder; n++) {
        if (p[n] == 0) {
            return 0.0;
        }
        // score += log((double)p[n] / (double)p[n+bleuOrder]);
        score += log((double)p[n]) - log((double)p[n+bleuOrder]);
    }
    return exp(score/bleuOrder);
}

void optimizeBleu(vector<boundary>& cumulatedCounts, Interval& bestInterval)
{
    std::sort(cumulatedCounts.begin(), cumulatedCounts.end());
    int p[bleuOrder*2] = {0};
    size_t nCounts = cumulatedCounts.size();
    cout << "considering " << nCounts << " line intersections" << endl;

    bestInterval.score = -numeric_limits<double>::infinity();
    double oldBoundary = -numeric_limits<double>::infinity();

    for (size_t i=0; i<nCounts; i++) {
        const vector<int>& currCounts = cumulatedCounts[i].second;
        double newBoundary = cumulatedCounts[i].first;
        if (oldBoundary != newBoundary) {
            // check if we shall update bestInterval
            double bleuScore = Bleu(p);
            // cout << "Interval [" << oldBoundary << " - " << newBoundary << "] score: " << bleuScore;
            // cout << "c: " << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << " | " << p[4] << " " << p[5] << " " << p[6] << " " << p[7] << endl;
            if (bleuScore > bestInterval.score) {
                bestInterval.score = bleuScore;
                bestInterval.left = oldBoundary;
                bestInterval.right = newBoundary;
            }
            oldBoundary = newBoundary;
        }
        for (size_t n=0; n<bleuOrder*2; n++) {
            p[n] += currCounts[n];
        }
    }
    assert (bestInterval.score > -numeric_limits<double>::infinity());
    cout << "Final BestInterval [" << bestInterval.left << " - " << bestInterval.right << "] score: " << bestInterval.score << endl;

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


