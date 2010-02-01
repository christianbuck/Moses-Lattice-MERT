#pragma once

#include <vector>
#include <map>

#include "Types.h"
#include "Lattice.h"

using std::map;

typedef map<Phrase,int> NgramCounts;

struct BleuStats
{
    double leftBoundary;
    size_t counts[bleuOrder*2];
    size_t length;

    BleuStats(const size_t length, const double leftBoundary) {
        this->leftBoundary = leftBoundary;
        this->length = length;
        for (size_t n = 0; n < bleuOrder; ++n) { counts[n]=0; }
    }
};

void computeBleuStats(Lattice &lattice, const std::vector<Line>& a, const Phrase& reference, std::vector<BleuStats>& stats);
void accumulateBleu(const std::vector<BleuStats>& stats, std::vector<boundary>& cumulatedCounts);
void optimizeBleu(std::vector<boundary>& cumulatedCounts, Interval& bestInterval);
