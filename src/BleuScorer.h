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

