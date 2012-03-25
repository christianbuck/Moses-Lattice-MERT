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

#include <limits>
#include <iostream>
#include <cassert>
#include <list>
#include <queue>
#include <algorithm>
#include "Lattice.h"
#include "Types.h"
#include "Util.h"

using std::numeric_limits;
using std::vector;
using std::list;
using std::ostream;
using std::cout;
using std::endl;

struct CompareLinePtr
{
  bool operator()(const Line* a, const Line* b)
  {
    // reverse order
    return !Line::CompareBySlope(*a, *b);
  }
};

void MergeAndSweep(vector<vector<Line*>*>& input, vector<Line*>& a)
{
  vector<Line*> lines;
  for (vector<vector<Line*>*>::const_iterator in_it = input.begin();
      in_it < input.end(); ++in_it)
  {
    lines.insert(lines.end(), (*in_it)->begin(), (*in_it)->end());
  }
  sort(lines.begin(), lines.end(), Line::ComparePtrBySlope);
  size_t a_size = a.size();
  for (vector<Line*>::const_iterator it = lines.begin(); it < lines.end(); ++it)
  {
    Line* const l = (*it);
    bool discard_line = false;
    if (a_size>0)
    {
      const Line* const prev = a.back();
      assert(prev->m_slope <= l->m_slope);
      if (prev->m_slope == l->m_slope)
      {
        if (l->m_offset <= prev->m_offset)
        {
          discard_line = true;
        }
        else
        {
          a.pop_back();
          --a_size;
        }
      }
      while (!discard_line && a_size>0)
      {
        const Line* const prev = a.back();
        l->m_leftBound = (l->m_offset - prev->m_offset) / (prev->m_slope - l->m_slope);
        if (l->m_leftBound > prev->m_leftBound)
          break;
        a.pop_back();
        --a_size;
      }
    }
    if (a_size==0)
    {
      l->m_leftBound = -numeric_limits<double>::infinity();
    }
    if (!discard_line)
    {
      a.push_back(l);
      ++a_size;
    }
  }
}

void LatticeEnvelope(Lattice& lattice, const FeatureVector& dir,
    const FeatureVector& lambda, vector<Line>& avec)
{
  // temporary object for storing hull lines that are associated with edges
  vector<vector<Line*> > L(lattice.GetEdgeCount());
  vector<Line*> lineCache;
  vector<Line*> a;       // hull for the current vertex

  vector<size_t> start;
  start.push_back(0);     // 0 is the source node key in lattice
  TopoIterator v_it(lattice, start);

  // traverse the lattice in topological order
  while (!v_it.IsEnd())
  {
    a.clear();
    const size_t vkey = v_it.Get();
    Lattice::Vertex& v = lattice.GetVertex(vkey);
    // special case for source node: insert horizontal line
    if (vkey == 0)
    {
      assert(lineCache.empty());
      Line* const l = new Line();
      lineCache.push_back(l);
      a.push_back(l);
    }
    else
    {
      // merge hulls associated with incoming edges into single sorted list of lines
      vector<vector<Line*>*> alines;
      alines.reserve(v.in.size());
      for (size_t i = 0; i < v.in.size(); ++i)
      {
        const Lattice::EdgeKey edgekey = v.in[i];
        alines.push_back(&L[edgekey]);
      }
      MergeAndSweep(alines, a);
    }
    assert(!a.empty());

    // update hulls associated with outgoing edges
    const size_t n_outedges = v.out.size();
    for (size_t i = 0; i < n_outedges; ++i)
    {
      const Lattice::EdgeKey edgekey = v.out[i];
      const Lattice::Edge& edge = lattice.GetEdge(edgekey);
      const bool is_sinknode = (edge.scores.size()==0);
      const double dot_dir = is_sinknode ? 0 :dotProduct(edge.scores, dir);
      const double dot_lambda = is_sinknode ? 0 :dotProduct(edge.scores, lambda);
      vector<Line*>& lines = L[edgekey];
      lines.reserve(a.size());
      for (vector<Line*>::const_iterator ait = a.begin(); ait != a.end(); ++ait)
      {
        Line* const l = (i==n_outedges-1) ? *ait : (is_sinknode) ? new Line(**ait) : new Line(**ait, dot_dir, dot_lambda, edgekey);
        if (i==n_outedges-1) {
          // reuse line from a but update if necessesary
          if (!is_sinknode) {
            Line& update_line = *l;
            update_line.m_slope += dot_dir;
            update_line.m_offset += dot_lambda;
            update_line.AddEdge(lattice, edgekey);
          }
        } else {
          // remember to delete newly created line
          lineCache.push_back(l);
        }
        lines.push_back(l);
      }
    }
    v_it.FindNext();
  }
  for (vector<Line*>::const_iterator lit = a.begin(); lit != a.end(); ++lit)
  {
    avec.push_back(**lit);
  }
  for (vector<Line*>::const_iterator cit = lineCache.begin();
      cit != lineCache.end(); ++cit)
  {
    delete *cit;
  }
}
