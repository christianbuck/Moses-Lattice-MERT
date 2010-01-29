#pragma once

#include <vector>

#include "Types.h"
#include "Lattice.h"

void computeBleuStats(Lattice &lattice, const std::vector<Line>& a, const Phrase& reference, std::vector<BleuStats>& stats);
void accumulateBleu(const std::vector<BleuStats>& stats, std::vector<boundary>& cumulatedCounts);
void optimizeBleu(std::vector<boundary>& cumulatedCounts, Interval& bestInterval);
