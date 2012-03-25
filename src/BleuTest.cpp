#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>

using namespace std;

#include "Types.h"
#include "BleuScorer.h"

BleuStats ComputeBleuStats(const Phrase &hyp, const Phrase& ref)
{
  const size_t refSize = ref.size();
  NGramTree refTree;
  for (size_t pos = 0; pos < refSize; ++pos)
  {
    BuildNGramTree(ref, refTree, pos, refSize, 0);
  }
  const size_t hypSize = hyp.size();
  BleuStats lineStats(hypSize, 0);

  NGramTree hypTree;
  for (size_t pos = 0; pos < hypSize; ++pos)
  {
    BuildNGramTree(hyp, hypTree, pos, hypSize, 0);
  }

  CountNGrams(hypTree, refTree, 0, lineStats.m_counts);
//        cout << "Ref: " << ref << endl;
//        cout << "Hyp: " << hyp << endl;
//        cout << "Stats: ";
//        cout << lineStats.counts[0] << ", ";
//        cout << lineStats.counts[1] << ", ";
//        cout << lineStats.counts[2] << ", ";
//        cout << lineStats.counts[3] << endl;
  return lineStats;
}

void ReadPhrase(istream &is, Phrase &phrase)
{
  string line;
  getline(is, line);
  istringstream iss(line);
  copy(istream_iterator<string>(iss), istream_iterator<string>(),
      back_inserter<vector<string> >(phrase));
//    for (size_t i = 0; i < phrase.size(); i++)
//        assert( phrase[i].length() > 0 );
}

int main(int argc, char** argv)
{
  if (argc < 3)
    return 1;
  char *pathHyp = argv[1];
  char *pathRef = argv[2];
  ifstream isHyp(pathHyp), isRef(pathRef);
  vector<boundary> counts;
  size_t totalRefSize = 0;
  while (isHyp && isRef)
  {
    Phrase hyp, ref;
    ReadPhrase(isHyp, hyp);
    ReadPhrase(isRef, ref);
//        cout << "Hyp: " << hyp << endl;
//        cout << "Ref: " << ref << endl;
    if (!isHyp || !isRef)
      break;
    vector<BleuStats> stats;
    stats.push_back(ComputeBleuStats(hyp, ref));
    AccumulateBleu(stats, counts);
    totalRefSize += ref.size();
  }

  int totalCounts[8] =
  { 0 };
  for (size_t i = 0; i < counts.size(); i++)
  {
    for (size_t j = 0; j < 8; j++)
      totalCounts[j] += counts[i].second[j];
  }

  for (size_t i = 0; i < 4; i++)
  {
    cout << "Correct" << i << " = " << totalCounts[i] << "/"
        << totalCounts[i + 4] << endl;
  }
  Interval interval;
  OptimizeBleu(counts, interval, totalRefSize);
  cout << interval.score << endl;
}

