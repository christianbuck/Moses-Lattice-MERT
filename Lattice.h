#pragma once

#include <map>


class Lattice {
public:
    void addEdge(size_t from, size_t to) {
        vertices[from].out.push_back(to);
        vertices[to].in.push_back(from);
    }

private:
    struct Vertex {
        vector<size_t> in;
        vector<size_t> out;
    };

    std::map<size_t, Vertex> vertices;
};
