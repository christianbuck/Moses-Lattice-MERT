#pragma once

#include <map>
#include <vector>

class Lattice {
private:
    typedef size_t VertexKey;
    typedef std::pair<VertexKey, VertexKey> EdgeKey;

public:
    Lattice() : edgeCount(0) {}

    struct Vertex {
        std::vector<VertexKey> in;
        std::vector<VertexKey> out;
    };
    struct Edge {
        std::vector<double> features;
    };

    Edge & addEdge(VertexKey from, VertexKey to) {
        vertices[from].out.push_back(to);
        vertices[to].in.push_back(from);
        Edge & edge = edges[EdgeKey(from, to)];
        edgeCount++;
        return edge;
    }

    size_t getVertexCount() const { return vertices.size(); }
    size_t getEdgeCount() const { return edges.size(); }

private:
    std::map<VertexKey, Vertex> vertices;
    std::map<EdgeKey, Edge> edges;
    size_t edgeCount;
};
