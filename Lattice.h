#pragma once

#include <map>
#include <vector>
#include <assert.h>
#include "Types.h"

class Lattice
{
public:
  typedef size_t VertexKey;
  typedef size_t EdgeKey;
  //typedef std::pair<VertexKey, VertexKey> EdgeKey;

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

  void addEdge(const Edge &edge)
  {
    EdgeKey key = edges.size();
    vertices[edge.from].out.push_back(key);
    vertices[edge.to].in.push_back(key);
    edges.push_back(edge);
  }

  void createSink()
  {
    size_t sink_vkey = 999999999;

    for (std::map<VertexKey, Vertex>::iterator it = vertices.begin();
        it != vertices.end(); it++)
    {
      if (it->second.out.size() == 0 && it->first != sink_vkey)
      {
        Edge edge;
        edge.from = it->first;
        edge.to = sink_vkey;
        addEdge(edge);
      }
    }
  }

  Vertex & getVertex(VertexKey key)
  {
    assert(vertices.find(key) != vertices.end());
    return vertices[key];
  }
  Edge & getEdge(EdgeKey key)
  {
//    std::cout << key << " of " << edges.size() << std::endl;
    assert (key < edges.size());
    return edges[key];
  }
  size_t getVertexCount() const
  {
    return vertices.size();
  }
  size_t getEdgeCount() const
  {
    return edges.size();
  }

private:
  std::map<VertexKey, Vertex> vertices;
  //std::map<EdgeKey, Edge> edges;
  std::vector<Edge> edges;
};

class TopoIterator
{
public:
  TopoIterator(Lattice &lattice, std::vector<Lattice::VertexKey> &start) :
      lattice(lattice), pendingVertices(start)
  {
  }

  Lattice::VertexKey get()
  {
    return pendingVertices.back();
  }

  void findNext()
  {
    Lattice::Vertex & v = lattice.getVertex(pendingVertices.back());
    pendingVertices.pop_back();
    for (size_t i = 0; i < v.out.size(); ++i)
    {
      Lattice::Edge &edge = lattice.getEdge(v.out[i]);
      Lattice::Vertex& vEnd = lattice.getVertex(edge.to);
      if (++vEnd.in_visited % vEnd.in.size() == 0)
      {
        pendingVertices.push_back(edge.to);
      }
    }
  }

  bool isEnd()
  {
    return pendingVertices.empty();
  }

private:
  Lattice &lattice;
  std::vector<Lattice::VertexKey> pendingVertices;
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
struct Line
{
  double slope;      // line slope (m)
  double offset;     // line offset (b)
  double leftBound;  // left boundary

  void addEdge(const Lattice& lattice, Lattice::EdgeKey edgekey) {
    assert (edgekey < lattice.getEdgeCount());
//    assert (edgekey < 1000000);
//    for (size_t i = 0; i < path.size(); ++i) {
//      assert(path[i] < lattice.getEdgeCount());
//    }
    path.push_back(edgekey);

  }

  const vector<Lattice::EdgeKey>& getPath() const {
    return path;
  }

  Line() :
      slope(0), offset(0), leftBound(-numeric_limits<double>::infinity())
  {
  }

//  Line(const Line& l):
//    slope(l.slope), offset(l.offset), leftBound(l.leftBound), path(l.path)
//  {
//    for (size_t i = 0; i < path.size(); ++i) {
//      assert(path[i] < 1000000);
//    }
//  }

  void getHypothesis(Lattice &lattice, Phrase &hypothesis) const
  {
    for (size_t i = 0; i < path.size(); i++)
    {
      const Phrase &phrase = lattice.getEdge(path[i]).phrase;
      hypothesis.insert(hypothesis.end(), phrase.begin(), phrase.end());
    }
  }

  static bool CompareBySlope(const Line &a, const Line &b)
  {
    return a.slope < b.slope;
  }

private:
  vector<Lattice::EdgeKey> path; // path through the graph


};

void latticeEnvelope(Lattice &lattice, const FeatureVector& d,
    const FeatureVector& lambdas, std::vector<Line> &a);
