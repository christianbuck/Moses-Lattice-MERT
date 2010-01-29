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
    a.sort(Line::CompareBySlope);

    list<Line>::iterator it = a.begin();
    while (it != a.end()) {
        if (it != a.begin()) {
            list<Line>::iterator it_prev = it;
            it_prev--;
            if (it_prev->m == it->m) {
                if (it->y <= it_prev->y) {
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
                it->x = (it->y - it_prev->y) / (it_prev->m - it->m);
                if (it->x > it_prev->x) break;

                list<Line>::iterator it_erase = it_prev;
                if (it_prev == a.begin()) {
                    it->x = -numeric_limits<double>::infinity();
                    a.erase(it_erase);
                    break;
                }
                it_prev--;
                a.erase(it_erase);
            }
        }
        else it->x = -numeric_limits<double>::infinity();
        it++;
    }
}

void latticeEnvelope(Lattice &lattice, const FeatureVector &dir, const FeatureVector &lambda, vector<Line> &avec)
{
    list<Line> *L = new list<Line>[lattice.getEdgeCount()];
//    map<Lattice::EdgeKey, list<Line> > L;

    vector<size_t> start;
    start.push_back(0);

    TopoIterator v_it(lattice, start);
    list<Line> a;

    while (!v_it.isEnd()) {
        size_t vkey = v_it.get();
        Lattice::Vertex & v = lattice.getVertex(vkey);

//        cout << "Visiting " << vkey
//            << " In: " << v.in.size()
//            << " Out: " << v.out.size() << endl;

        a.clear();
        if (vkey == 0) {
            a.push_back(Line());
        }
        else {
            for (size_t i=0; i<v.in.size();++i) {
                Lattice::EdgeKey edgekey = v.in[i];

                //map<Lattice::EdgeKey, list<Line> >::iterator it = L.find(edgekey);
                //if (it == L.end()) continue;

                //a.splice(a.end(), it->second );
                //L.erase(it);

                a.splice(a.end(), L[edgekey]);
            }
            sweepLine(a);
        }

        for (size_t i=0; i<v.out.size();++i) {
            Lattice::EdgeKey edgekey = v.out[i];
            Lattice::Edge &edge = lattice.getEdge(edgekey);

            L[edgekey] = a;
            list<Line> &lines = L[edgekey];

            if (edge.h.size() > 0) {
                double dot_dir = dotProduct(edge.h, dir);
                double dot_lambda = dotProduct(edge.h, lambda);
                for (list<Line>::iterator lit = lines.begin(); lit != lines.end(); ++lit) {
                    lit->m += dot_dir;
                    lit->y += dot_lambda;
                    lit->path.push_back(edgekey);
    //                cout << "    edge phrase " << edge.phrase << endl;
                }
            }
        }
        v_it.findNext();
    }

    avec.insert(avec.end(), a.begin(), a.end());

    delete[] L;

//    size_t K = avec.size();
//    for (size_t i = 0; i < K; i++) {
//        Line &l = avec[i];
//        cout << "  line " << l.x << endl;
//        cout << "  line " << l.x << " " << l.hypothesis << endl;
//    }
}

