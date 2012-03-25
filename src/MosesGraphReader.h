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
