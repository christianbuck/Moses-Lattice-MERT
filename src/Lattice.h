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

/**
 The Lattice class stores the decoder search graph (lattice).
 The graph contains a set of vertices and a set of edges, both indexed by integers.
 Due to specifics of Moses search graph, vertices can have arbitrary key, 
 while edges are always enumerated from 0.
 Each edge stores a target language phrase and a vector of transition scores.
 The class is tailored for convex hull search algorithm only.
 **/
class Lattice
{
public:
  typedef size_t VertexKey;		//! vertex key type
  typedef size_t EdgeKey;		//! edge key type

  /**
   Represents a vertice in the lattice.
   */
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

  /**
   Represents an edge in the lattice.
   */
  struct Edge
  {
    FeatureVector scores;
    Phrase phrase;
    VertexKey from;
    VertexKey to;
  };

  //! add an edge to the lattice
  void AddEdge(const Edge &edge);

  //! connect all sinks to one sink
  void CreateSink();

  /**
   Gets a vertex by its key.
   */
  Vertex& GetVertex(const VertexKey key) 
  {	
    //assert(vertices.find(key) != vertices.end());
    return m_vertices[key];
  }

  /**
   Gets an edge by its key.
   */
  const Edge& GetEdge(const EdgeKey key) const
  {
    assert (key < m_edges.size());
    return m_edges[key];
  }

  /**
   Gets the total number of vertices in the graph.
   */
  size_t GetVertexCount() const
  {
    return m_vertices.size();
  }

  /**
   Gets the total number of edges in the graph.
   */
  size_t GetEdgeCount() const
  {
    return m_edges.size();
  }

private:
  std::map<VertexKey, Vertex> m_vertices;	//! map of all lattice vertices
  std::vector<Edge> m_edges;	//! array of all lattice edges
};

/**
 The class TopoIterator allows to iterate though the vertices of a lattice
 in topological order.
 **/
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
  	/*
      The algorithm from Wikipedia:

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

