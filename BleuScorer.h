#pragma once

#include <vector>

#include "Types.h"

void computeBleuStats(const vector<Line>& a, const Phrase& reference, vector<BleuStats>& stats);

