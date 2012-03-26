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

  //! get a vertex by its key
  Vertex& GetVertex(const VertexKey key);

  //! get an edge by its key
  const Edge& GetEdge(const EdgeKey key) const;

  //! get the total number of vertices
  size_t GetVertexCount() const;
  
  //! get the total number of edges
  size_t GetEdgeCount() const;
  
private:
  std::map<VertexKey, Vertex> m_vertices;	//! map of all lattice vertices
  std::vector<Edge> m_edges;	//! array of all lattice edges
};

/**
 The class TopoIterator allows to iterate over the vertices of a lattice
 in topological order.
 **/
class TopoIterator
{
public:
  TopoIterator(Lattice &lattice, std::vector<Lattice::VertexKey> &start) :
      m_lattice(lattice), m_pendingVertices(start)
  {
  }

  //! next vertice
  void FindNext();

  //! get the current vertice
  Lattice::VertexKey Get() const
  {
    return m_pendingVertices.back();
  }

  //! check if it is the last one
  bool IsEnd() const
  {
    return m_pendingVertices.empty();
  }

private:
  Lattice& m_lattice;
  std::vector<Lattice::VertexKey> m_pendingVertices;
};
