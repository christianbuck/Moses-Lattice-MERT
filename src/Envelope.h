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

#include <map>
#include <vector>
#include <assert.h>

#include "Lattice.h"

/**
 The class Line describes a line segment (m * x + b) in a hull
 */
class Line
{
public:
  void AddEdge(const Lattice& lattice, const Lattice::EdgeKey edgekey) {
    assert (edgekey < lattice.GetEdgeCount());
    m_path.push_back(edgekey);
  }

  const vector<Lattice::EdgeKey>& GetPath() const {
    return m_path;
  }

  Line() :
      m_slope(0), m_offset(0), m_leftBound(-numeric_limits<double>::infinity())
  {
  }

  Line(const Line& l) :
      m_slope(l.m_slope), m_offset(l.m_offset), m_leftBound(l.m_leftBound)
  {
    // path.insert(path.begin(), l.path.begin(), l.path.end());
	m_path.reserve(l.m_path.size()+1);
    m_path.assign(l.m_path.begin(), l.m_path.end());
  }

  Line(const Line& l, const double slope_update, const double offset_update, const Lattice::EdgeKey edgekey) :
      m_slope(l.m_slope+slope_update), m_offset(l.m_offset+offset_update), m_leftBound(l.m_leftBound)
  {
    m_path.reserve(l.m_path.size()+1);
    m_path.assign(l.m_path.begin(), l.m_path.end());
    m_path.push_back(edgekey);
  }

  void GetHypothesis(const Lattice &lattice, Phrase &hypothesis) const
  {
    for (size_t i = 0; i < m_path.size(); i++)
    {
      const Phrase &phrase = lattice.GetEdge(m_path[i]).phrase;
      hypothesis.insert(hypothesis.end(), phrase.begin(), phrase.end());
    }
  }

  static bool CompareBySlope(const Line &a, const Line &b)
  {
    return a.m_slope < b.m_slope;
  }

  static bool ComparePtrBySlope(const Line* const a, const Line* const b)
  {
    return a->m_slope < b->m_slope;
  }

  double GetLeftBound(void) const
  {
	return m_leftBound;
  }

  // should move these to private
  double m_slope;      // line slope (m)
  double m_offset;     // line offset (b)
  double m_leftBound;  // left boundary
	
private:
  vector<Lattice::EdgeKey> m_path; // path through the graph	
};

//! computes the convex hull envelope
void LatticeEnvelope(Lattice &lattice, const FeatureVector& d,
    const FeatureVector& lambdas, std::vector<Line> &a);

