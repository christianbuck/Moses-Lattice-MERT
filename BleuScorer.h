#pragma once

#include <vector>

#include "Types.h"

void computeBleuStats(const std::vector<Line>& a, const Phrase& reference, std::vector<BleuStats>& stats);
void accumulateBleu(const std::vector<BleuStats>& stats, std::vector<boundary>& cumulatedCounts);
void optimizeBleu(const std::vector<boundary>& cumulatedCounts, Interval& bestInterval);
