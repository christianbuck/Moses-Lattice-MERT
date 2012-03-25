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
//#include <boost/regex.hpp>

#include "Lattice.h"
#include "Types.h"
class MosesGraphReader
{
public:
  MosesGraphReader(std::istream &is);

  bool GetNextLattice(Lattice &lattice);

private:
  std::istream &m_is;

//  static boost::regex m_rx_line0, m_rx_line1, m_rx_line2, m_rx_line3, m_rx_line4,
//      m_rx_line5;

  struct Entry
  {
    size_t m_sentence;
    size_t m_hyp;
    size_t m_back;
    FeatureVector m_features;
    Phrase m_phrase;
  };

  void ParseLine(const string &line, Entry &e);
  void GetSentenceNumber(string &suffix, Entry &e);
  void GetHypothesis(string &suffix, Entry &e);
  void GetBackRef(string &suffix, Entry &e);
  void GetFeatures(string &suffix, Entry &e);
  void GetPhrase(string &suffix, Entry &e);
};
