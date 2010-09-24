#include <limits>
#include <iostream>
#include <algorithm>
#include <cassert>

#include <list>

#include "Lattice.h"
#include "Types.h"

using std::numeric_limits;
using std::vector;
using std::list;
using std::map;
using std::ostream;
using std::cout;
using std::endl;

template<class F>
double dotProduct(const vector<F>& a,const vector<F>& b)
{
    size_t d = a.size();
    assert(b.size() == d);
    F p = 0;
    for (size_t i=0;i<d;++i) {
        p += a[i]*b[i];
    }
    return p;
}

ostream & operator << (ostream &os, const Phrase& p) {
    for (size_t i = 0; i < p.size(); i++) {
        if (i > 0) os << " ";
        os << p[i];
    }
    return os;
}


// Implementation of Algorithm 1
// Calculates the upper envelope for a set of lines
// The algorithm modifies the line array in place

void sweepLine(list<Line> &a)
{
//    a.sort(Line::CompareBySlope);

    list<Line>::iterator it = a.begin();
    while (it != a.end()) {
        if (it != a.begin()) {
            list<Line>::iterator it_prev = it;
            it_prev--;
            if (it_prev->slope == it->slope) {
                if (it->offset <= it_prev->offset) {
                    list<Line>::iterator it_erase = it;
                    it++;
                    a.erase(it_erase);
                    continue;
                }
                else if (it_prev != a.begin()) {
                    list<Line>::iterator it_erase = it_prev;
                    it_prev--;
                    a.erase(it_erase);
                }
                else {
                    list<Line>::iterator it_erase = it_prev;
                    a.erase(it_erase);
                    continue;
                }
            }
            while (true) {
                it->leftBound = (it->offset - it_prev->offset) / (it_prev->slope - it->slope);
                if (it->leftBound > it_prev->leftBound) break;

                list<Line>::iterator it_erase = it_prev;
                if (it_prev == a.begin()) {
                    it->leftBound = -numeric_limits<double>::infinity();
                    a.erase(it_erase);
                    break;
                }
                it_prev--;
                a.erase(it_erase);
            }
        }
        else it->leftBound = -numeric_limits<double>::infinity();
        it++;
    }
}

void sweepLine(vector<Line> &a)
{
    // assumed: a is sorted by slope
    // a.sort(Line::CompareBySlope);

    size_t j = 0;
    size_t K = a.size();
    for (size_t i = 0; i < K; i++) {
        Line &l = a[i];
        if (j > 0) {
            if (a[j - 1].slope == l.slope) {
                if (l.offset <= a[j - 1].offset) continue;
                j--;
            }
            while (j > 0) {
                l.leftBound = (l.offset - a[j - 1].offset) / 
                    (a[j - 1].slope - l.slope);
                if (l.leftBound > a[j - 1].leftBound) break;
                j--;
            }
            a[j] = l;
        }
        if (j == 0) a[j].leftBound = -numeric_limits<double>::infinity();
        j++;
    }
    a.resize(j);
}

// Implementation of Algorithm 2 from the paper
// The function computes upper envelope for the best translations in the lattice
// Given feature weights lambda and direction vector dir,
// it finds the boundary points along the line described by dir.
// Each boundary point is described by a Line structure.

#ifdef NEW_ENVELOPE

void latticeEnvelope(Lattice &lattice, const FeatureVector &dir, const FeatureVector &lambda, vector<Line> &avec)
{
    // temporary object for storing hull lines that are associated with edges
    vector<Line> *L = new vector<Line>[lattice.getEdgeCount()];
    vector<Line> a;       // hull for the current vertex

    vector<size_t> start;
    start.push_back(0);     // 0 is the source node key in lattice

    TopoIterator v_it(lattice, start);

    // traverse the lattice in topological order
    while (!v_it.isEnd()) {
        size_t vkey = v_it.get();
        Lattice::Vertex & v = lattice.getVertex(vkey);

//        cout << "Visiting " << vkey
//            << " In: " << v.in.size()
//            << " Out: " << v.out.size() << endl;

        a.clear();
        // special case for source node: insert horizontal line
        if (vkey == 0)
            a.push_back(Line());

        // merge hulls associated with incoming edges into single sorted list of lines
        for (size_t i=0; i<v.in.size();++i) {
            Lattice::EdgeKey edgekey = v.in[i];

            if (L[edgekey].size() == 0) {
                Lattice::Edge & edge = lattice.getEdge(edgekey);
                cout << "WARNING: In edge from " << edge.from << " contains zero lines!" << endl;
            }
            a.insert(a.end(), L[edgekey].begin(), L[edgekey].end());
            L[edgekey].clear();
        }
        make_heap(a.begin(), a.end(), Line::CompareBySlope);
        sort_heap(a.begin(), a.end(), Line::CompareBySlope);
//        a.sort(Line::CompareBySlope);

        // compute the upper envelope by removing unneccesary lines from a and updating leftBounds
        sweepLine(a);

        // update hulls associated with outgoing edges
        for (size_t i=0; i<v.out.size();++i) {
            Lattice::EdgeKey edgekey = v.out[i];
            Lattice::Edge &edge = lattice.getEdge(edgekey);

            L[edgekey] = a;
            // update unless the edge leads to the sink node and has no feature score vector
            if (edge.scores.size() > 0) {
                vector<Line> &lines = L[edgekey];
                double dot_dir = dotProduct(edge.scores, dir);
                double dot_lambda = dotProduct(edge.scores, lambda);
                for (vector<Line>::iterator lit = lines.begin(); lit != lines.end(); ++lit) {
                    lit->slope += dot_dir;
                    lit->offset += dot_lambda;
                    lit->path.push_back(edgekey);
//                cout << "    edge phrase " << edge.phrase << endl;
                }
            }
        }
        v_it.findNext();
    }

    // return result
    avec.insert(avec.end(), a.begin(), a.end());

    delete[] L;

//    size_t K = avec.size();
//    for (size_t i = 0; i < K; i++) {
//        Line &l = avec[i];
//        cout << "  line " << l.x << endl;
//        cout << "  line " << l.x << " " << l.hypothesis << endl;
//    }
}


#else

void latticeEnvelope(Lattice &lattice, const FeatureVector &dir, const FeatureVector &lambda, vector<Line> &avec)
{
    // temporary object for storing hull lines that are associated with edges
    list<Line> *L = new list<Line>[lattice.getEdgeCount()];
    list<Line> a;       // hull for the current vertex

    vector<size_t> start;
    start.push_back(0);     // 0 is the source node key in lattice

    TopoIterator v_it(lattice, start);

    // traverse the lattice in topological order
    while (!v_it.isEnd()) {
        size_t vkey = v_it.get();
        Lattice::Vertex & v = lattice.getVertex(vkey);

//        cout << "Visiting " << vkey
//            << " In: " << v.in.size()
//            << " Out: " << v.out.size() << endl;

        a.clear();
        // special case for source node: insert horizontal line
        if (vkey == 0)
            a.push_back(Line());

        // merge hulls associated with incoming edges into single sorted list of lines
        for (size_t i=0; i<v.in.size();++i) {
            Lattice::EdgeKey edgekey = v.in[i];

            if (L[edgekey].size() == 0) {
                Lattice::Edge & edge = lattice.getEdge(edgekey);
                cout << "WARNING: In edge from " << edge.from << " contains zero lines!" << endl;
            }
            // this also empties L[edgekey], which is not needed anymore
            a.merge(L[edgekey], Line::CompareBySlope);
        }

        // compute the upper envelope by removing unneccesary lines from a and updating leftBounds
        sweepLine(a);

        // update hulls associated with outgoing edges
        for (size_t i=0; i<v.out.size();++i) {
            Lattice::EdgeKey edgekey = v.out[i];
            Lattice::Edge &edge = lattice.getEdge(edgekey);

            L[edgekey] = a;
            // update unless the edge leads to the sink node and has no feature score vector
            if (edge.scores.size() > 0) {
                list<Line> &lines = L[edgekey];
                double dot_dir = dotProduct(edge.scores, dir);
                double dot_lambda = dotProduct(edge.scores, lambda);
                for (list<Line>::iterator lit = lines.begin(); lit != lines.end(); ++lit) {
                    lit->slope += dot_dir;
                    lit->offset += dot_lambda;
                    lit->path.push_back(edgekey);
//                cout << "    edge phrase " << edge.phrase << endl;
                }
            }
        }
        v_it.findNext();
    }

    // return result
    avec.insert(avec.end(), a.begin(), a.end());

    delete[] L;

//    size_t K = avec.size();
//    for (size_t i = 0; i < K; i++) {
//        Line &l = avec[i];
//        cout << "  line " << l.x << endl;
//        cout << "  line " << l.x << " " << l.hypothesis << endl;
//    }
}

#endif
