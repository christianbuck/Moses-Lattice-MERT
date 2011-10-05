#pragma once

#include <iostream>
#include <boost/regex.hpp>

#include "Lattice.h"
#include "Types.h"
class MosesGraphReader
{
public:
  MosesGraphReader(std::istream &is);

  bool GetNextLattice(Lattice &lattice);

private:
  std::istream &is;

  static boost::regex rx_line0, rx_line1, rx_line2, rx_line3, rx_line4,
      rx_line5;

  struct Entry
  {
    size_t sentence;
    size_t hyp;
    size_t back;
    FeatureVector features;
    Phrase phrase;
  };

  void parseLine(const string &line, Entry &e);
  void getSentenceNumber(string &suffix, Entry &e);
  void getHypothesis(string &suffix, Entry &e);
  void getBackRef(string &suffix, Entry &e);
  void getFeatures(string &suffix, Entry &e);
  void getPhrase(string &suffix, Entry &e);
};
