#pragma once

#include <map>
#include <vector>

#include "Types.h"

class Lattice {
public:
    typedef size_t VertexKey;
    typedef size_t EdgeKey;
    //typedef std::pair<VertexKey, VertexKey> EdgeKey;

    struct Vertex {
        std::vector<EdgeKey> in;
        std::vector<EdgeKey> out;
        size_t in_visited;

        Vertex() : in_visited(0) {}
    };

    struct Edge {
        FeatureVector h;
        Phrase phrase;
        VertexKey from;
        VertexKey to;
    };

    void addEdge(const Edge &edge) {
        EdgeKey key = edges.size();
        vertices[edge.from].out.push_back(key);
        vertices[edge.to].in.push_back(key);
        edges.push_back(edge);
    }

    void createSink() {
        size_t skey = 9999999;

        for (std::map<VertexKey, Vertex>::iterator it = vertices.begin(); it != vertices.end(); it++) {
            if (it->second.out.size() == 0) {
                Edge edge;
                edge.from = it->first;
                edge.to = skey;
                addEdge(edge);
            }
        }
    }

    Vertex & getVertex(VertexKey key) { return vertices[key]; }
    Edge & getEdge(EdgeKey key) { return edges[key]; }
    size_t getVertexCount() const { return vertices.size(); }
    size_t getEdgeCount() const { return edges.size(); }

private:
    std::map<VertexKey, Vertex> vertices;
    //std::map<EdgeKey, Edge> edges;
    std::vector<Edge> edges;
};

class TopoIterator {
public:
    TopoIterator(Lattice &lattice, std::vector<Lattice::VertexKey> &start)
        : lattice(lattice), pendingVertices(start) {}

    Lattice::VertexKey get() {
        return pendingVertices.back();
    }

    void findNext() {
        Lattice::Vertex & v = lattice.getVertex(pendingVertices.back());
        pendingVertices.pop_back();
        for (size_t i = 0; i < v.out.size(); ++i) {
            Lattice::Edge &edge = lattice.getEdge(v.out[i]);
            Lattice::Vertex& vEnd = lattice.getVertex(edge.to);
            if (++vEnd.in_visited % vEnd.in.size() == 0) {
                pendingVertices.push_back(edge.to);
            }
        }
    }

    bool isEnd() {
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

struct Line
{
    double  x;      // left boundary
    double  y;      // line offset
    double  m;      // line slope
    vector<Lattice::EdgeKey> path; // path through the graph

    Line() : x(-numeric_limits<double>::infinity()), y(0), m(0) {}

    void getHypothesis(Lattice &lattice, Phrase &hypothesis) const {
        for (size_t i = 0; i < path.size(); i++) {
            const Phrase &phrase = lattice.getEdge(path[i]).phrase;
            hypothesis.insert(hypothesis.end(), phrase.begin(), phrase.end());
        }
    }

    static bool CompareBySlope(const Line &a, const Line &b)
    {
        return a.m < b.m;
    }
};

void latticeEnvelope(Lattice &lattice, const FeatureVector& d, const FeatureVector& lambdas, std::vector<Line> &a);
