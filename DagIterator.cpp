#include "DagIterator.h"
#include <assert.h>

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


DagIterator::DagIterator(Vertex start) {
    pendingVertices.push_back(start);
}

Vertex DagIterator::getVertex() 
{
    assert (pendingVertices.size() > 0);
    return pendingVertices.back();
};

bool DagIterator::next() 
{
    Vertex v = pendingVertices.back();
    pendingVertices.pop_back();
    for (size_t i=0; i<v.out.size();++i) {
        Vertex* vEnd = v.out[i].end;
        if (++vEnd->nIncomingVisited % vEnd->in.size() == 0) {
            pendingVertices.push_back(*vEnd);
        }
    }
    return !pendingVertices.empty();
};


        

