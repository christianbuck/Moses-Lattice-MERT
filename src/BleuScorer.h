/*
 * LatticeMERT
 * Copyright (C)  2010-2012 
 *   Christian Buck
 *   Kārlis Goba <karlis.goba@gmail.com> 
 * 
 * LatticeMERT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LatticeMERT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <map>

#include "Types.h"
#include "Lattice.h"

using std::map;

typedef map<Phrase, int> NgramCounts;

struct BleuStats
{
  double leftBoundary;
  vector<size_t> counts;
  size_t length;

  BleuStats(const size_t length, const double leftBoundary)
  {
    this->leftBoundary = leftBoundary;
    this->length = length;
    this->counts.resize(bleuOrder);
    // for (size_t n = 0; n < bleuOrder*2; ++n) { counts[n]=0; }
  }
};

struct NGramTree;
struct NGramTree
{
  size_t count;
  size_t used;
  map<Word, NGramTree> branches;
  NGramTree()
  {
    count = 0;
    used = 0;
  }
  NGramTree(const size_t count)
  {
    this->count = count;
    this->used = 0;
  }
};

void computeBleuStats(const Lattice &lattice, const std::vector<Line>& a,
    const Phrase& reference, std::vector<BleuStats>& stats);
void accumulateBleu(const std::vector<BleuStats>& stats,
    std::vector<boundary>& cumulatedCounts);
void optimizeBleu(std::vector<boundary>& cumulatedCounts,
    Interval& bestInterval, size_t refLength);

void buildNGramTree(const Phrase& ref, NGramTree& tree, size_t pos,
    const size_t len, size_t depth);
void countNGrams(NGramTree &hypTree, NGramTree& refTree, const size_t depth,
    vector<size_t> &counts);

