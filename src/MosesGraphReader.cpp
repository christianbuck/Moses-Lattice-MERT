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

#include <iostream>
#include <assert.h>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "MosesGraphReader.h"
#include "Types.h"
using namespace boost;
using namespace std;

// 0 hyp=0 stack=0
// 0 hyp=37 stack=1 back=0 [ w: -2 u: 0 d: -4 0 0 -1.03429 0 0 0 lm: -10.9675 tm: -0.442792 -0.794259 -1.87952 -2.90528 0.999896 ] out=die inflation

//regex MosesGraphReader::m_rx_line0("^(\\d+) hyp=(\\d+) ");
//regex MosesGraphReader::m_rx_line1("^back=(\\d+) \\[ (.*) \\] out=(.*) $");
//regex MosesGraphReader::m_rx_line2("\\w+:");
//regex MosesGraphReader::m_rx_line3("-?\\d+(\\.\\d+)?");

MosesGraphReader::MosesGraphReader(istream &is) :
    m_is(is)
{
  string line;
  getline(is, line);
}

void MosesGraphReader::ParseLine(const string &line, Entry &e)
{
//    cerr << "Parsing [" << line << "]" << endl;
  string suffix = line;

  GetSentenceNumber(suffix, e);
  GetHypothesis(suffix, e);
  if (e.m_hyp == 0)
    return;
  GetBackRef(suffix, e);
  GetFeatures(suffix, e);
  GetPhrase(suffix, e);
}

void MosesGraphReader::GetSentenceNumber(string &suffix, Entry &e)
{
  size_t pos = suffix.find_first_of(' ');
  e.m_sentence = lexical_cast < size_t > (suffix.substr(0, pos));
  suffix = suffix.substr(pos + 1);
}

void MosesGraphReader::GetHypothesis(string &suffix, Entry &e)
{
  size_t pos1 = suffix.find_first_of('=');
  size_t pos2 = suffix.find_first_of(' ');
  pos1++;
  e.m_hyp = lexical_cast < size_t > (suffix.substr(pos1, pos2 - pos1));
  suffix = suffix.substr(pos2 + 1);
}

void MosesGraphReader::GetBackRef(string &suffix, Entry &e)
{
  size_t pos1 = suffix.find_first_of('=');
  size_t pos2 = suffix.find_first_of(' ');
  pos1++;
  e.m_back = lexical_cast < size_t > (suffix.substr(pos1, pos2 - pos1));
  suffix = suffix.substr(pos2 + 1);
}

void MosesGraphReader::GetFeatures(string &suffix, Entry &e)
{
  // suffix[0] == '['
  size_t pos1 = 0;
  while (pos1 < suffix.size())
  {
    size_t pos2 = suffix.find_first_of(' ', pos1);
    if (pos2 == string::npos)
      break;

    // get the token
    const string &token = suffix.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;

    // check for the brackets
    if (token == "[")
      continue;
    if (token == "]")
      break;

    // check if it is not a descriptor
    if (token[token.size() - 1] != ':')
    {
      FeatureValue feature = lexical_cast < FeatureValue > (token);
      e.m_features.push_back(feature);
//            cout << "  [" << feature << "]" << endl;
    }
  }
  suffix = suffix.substr(pos1);
}

void MosesGraphReader::GetPhrase(string &suffix, Entry &e)
{
  size_t pos1 = suffix.find_first_of('=');
  pos1++;
  while (pos1 < suffix.size())
  {
    size_t pos2 = suffix.find_first_of(' ', pos1);

    // get the token
    const string &token =
        (pos2 != string::npos) ?
            suffix.substr(pos1, pos2 - pos1) : suffix.substr(pos1);
    pos1 = pos2 + 1;

//        cout << "  [" << token << "]" << endl;
    e.m_phrase.push_back(token);
    if (pos2 == string::npos)
      break;
  }
  suffix = suffix.substr(pos1);
}

bool MosesGraphReader::GetNextLattice(Lattice &lattice)
{
  if (m_is.eof())
    return false;
  while (!m_is.eof())
  {
    string line;
    getline(m_is, line);
    if (m_is.eof())
      break;

    Entry e;
    ParseLine(line, e);

    if (e.m_hyp == 0)
      break;

    // add the edge to the lattice
//        cout << "Edge " << backId << " - " << hypId << " phrase [" << phrase << "]" << endl;
//        cout << "Lattice vertices " << lattice.getVertexCount() << " edges " << lattice.getEdgeCount() << endl;

    Lattice::Edge edge;
    edge.scores = e.m_features;
    edge.phrase = e.m_phrase;
    edge.from = e.m_back;
    edge.to = e.m_hyp;
    lattice.AddEdge(edge);

    for (size_t i = 0; i < edge.phrase.size(); i++)
      assert( edge.phrase[i].length() > 0);
  }
//    cout << "Lattice vertices " << lattice.getVertexCount() << " edges " << lattice.getEdgeCount() << endl;
  lattice.CreateSink();
  return true;
}
