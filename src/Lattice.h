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
#include "Types.h"
#include <ostream>
#include <algorithm>

class Lattice
{
public:
  typedef size_t VertexKey;
  typedef size_t EdgeKey;

  struct Vertex
  {
    std::vector<EdgeKey> in;
    std::vector<EdgeKey> out;
    size_t in_visited;

    Vertex() :
        in_visited(0)
    {
    }
  };

  struct Edge
  {
    FeatureVector scores;
    Phrase phrase;
    VertexKey from;
    VertexKey to;
  };

  void AddEdge(const Edge &edge)
  {
    EdgeKey key = m_edges.size();
    m_vertices[edge.from].out.push_back(key);
    m_vertices[edge.to].in.push_back(key);
    m_edges.push_back(edge);
  }

  void CreateSink()
  {
    size_t sink_vkey = 999999999;

    for (std::map<VertexKey, Vertex>::iterator it = m_vertices.begin();
        it != m_vertices.end(); ++it)
    {
      if (it->second.out.size() == 0 && it->first != sink_vkey)
      {
        Edge edge;
        edge.from = it->first;
        edge.to = sink_vkey;
        AddEdge(edge);
      }
    }
  }

  Vertex& GetVertex(const VertexKey key)
  {
    //assert(vertices.find(key) != vertices.end());
    return m_vertices[key];
  }

  const Edge& GetEdge(const EdgeKey key) const
  {
    assert (key < m_edges.size());
    return m_edges[key];
  }

  size_t GetVertexCount() const
  {
    return m_vertices.size();
  }

  size_t GetEdgeCount() const
  {
    return m_edges.size();
  }

private:
  std::map<VertexKey, Vertex> m_vertices;
  std::vector<Edge> m_edges;
};

class TopoIterator
{
public:
  TopoIterator(Lattice &lattice, std::vector<Lattice::VertexKey> &start) :
      m_lattice(lattice), m_pendingVertices(start)
  {
  }

  Lattice::VertexKey Get() const
  {
    return m_pendingVertices.back();
  }

  void FindNext()
  {
    const Lattice::Vertex & v = m_lattice.GetVertex(m_pendingVertices.back());
    m_pendingVertices.pop_back();
    for (size_t i = 0; i < v.out.size(); ++i)
    {
      const Lattice::Edge &edge = m_lattice.GetEdge(v.out[i]);
      Lattice::Vertex& vEnd = m_lattice.GetVertex(edge.to);
      if (++vEnd.in_visited % vEnd.in.size() == 0)
      {
        m_pendingVertices.push_back(edge.to);
      }
    }
  }

  bool IsEnd() const
  {
    return m_pendingVertices.empty();
  }

private:
  Lattice& m_lattice;
  std::vector<Lattice::VertexKey> m_pendingVertices;
};

/*
 From Wikipedia:

 L <- Empty list that will contain the sorted elements
 S <- Set of all nodes with no incoming edges
 while S is non-empty do
 remove a node n from S
 insert n into L
 for each node m with an edge e from n to m do
 remove edge e from the graph
 if m has no other incoming edges then
 insert m into S
 */

// describes a Line segment (m * x + b) in a hull
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

void LatticeEnvelope(Lattice &lattice, const FeatureVector& d,
    const FeatureVector& lambdas, std::vector<Line> &a);

