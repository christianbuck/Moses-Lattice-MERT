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

/**
 Adds a new edge to the graph. The members edge.from and edge.to 
 must point to the corresponding vertices. Vertices are created automatically
 if they did not exist before.
 */
void Lattice::AddEdge(const Edge &edge)
{
  EdgeKey key = m_edges.size();
  m_vertices[edge.from].out.push_back(key);
  m_vertices[edge.to].in.push_back(key);
  m_edges.push_back(edge);
}

/**
 Creates a new sink (node with no outgoing edges) and connects all existing
 sinks to it. This is necessary as Moses output does not have a single sink.
 */
void Lattice::CreateSink()
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

/**
 Gets a vertex by its key.
 */
Lattice::Vertex& Lattice::GetVertex(const Lattice::VertexKey key) 
{	
  //assert(vertices.find(key) != vertices.end());
  return m_vertices[key];
}

/**
 Gets an edge by its key.
 */
const Lattice::Edge& Lattice::GetEdge(const Lattice::EdgeKey key) const
{
  assert (key < m_edges.size());
  return m_edges[key];
}

/**
 Gets the total number of vertices in the graph.
 */
size_t Lattice::GetVertexCount() const
{
  return m_vertices.size();
}

/**
 Gets the total number of edges in the graph.
 */
size_t Lattice::GetEdgeCount() const
{
  return m_edges.size();
}


/**
 Go to the next vertice in topological order.
 */
void TopoIterator::FindNext()
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