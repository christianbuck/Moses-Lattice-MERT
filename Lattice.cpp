#include <limits>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "Lattice.h"
#include "Types.h"

using std::numeric_limits;
using std::vector;
using std::map;
using std::ostream;
using std::cout;
using std::endl;

double dotProduct(const vector<double>& a,const vector<double>& b)
{
    size_t d = a.size();
    assert(b.size() == d);
    double p = 0.0;
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

void sweepLine(vector<Line> &a)
{
    sort (a.begin(), a.end(), Line::CompareBySlope);

    size_t j = 0;
    size_t K = a.size();
    for (size_t i = 0; i < K; i++) {
        Line &l = a[i];
//        cout << "    line " << l.m << " x + " << l.y << " " << l.hypothesis << endl;
        l.x = -numeric_limits<double>::infinity();
        if (0 < j) {
            if (a[j - 1].m == l.m) {
                if (l.y <= a[j - 1].y) continue;
                --j;
            }
            while (0 < j) {
                l.x = (l.y - a[j - 1].y) / (a[j - 1].m - l.m);
//                cout << "    x = " << l.x << " j = " << j << " a[j-1].x = " << a[j-1].x << endl;
                if (a[j - 1].x < l.x) break;
                --j;
            }
//            cout << "    j = " << j << endl;
            if (0 == j) l.x = -numeric_limits<double>::infinity();
        }
        a[j++] = l;
    }
//    cout << "  SweepLine: |a| = " << a.size() << " -> " << j << endl;
    for (size_t i = 0; i < j; i++) {
        Line &l = a[i];
//        cout << "    line " << l.x << " " << l.hypothesis << endl;
    }
    a.resize(j);
}

void latticeEnvelope(Lattice &lattice, FeatureVector &dir, FeatureVector &lambda, vector<Line> &a)
{
    map<Lattice::EdgeKey, vector<Line> > L;

    vector<size_t> start;
    start.push_back(0);

    TopoIterator v_it(lattice, start);
    while (!v_it.isEnd()) {
        size_t vkey = v_it.get();
        Lattice::Vertex & v = lattice.getVertex(vkey);

//        cout << "Visiting " << vkey
//            << " In: " << v.in.size()
//            << " Out: " << v.out.size()
//            << " |L|=" << L.size() << endl;

        a.clear();
        if (vkey == 0) {
            a.push_back(Line());
        }
        else {
            for (size_t i=0; i<v.in.size();++i) {
                Lattice::EdgeKey edgekey(v.in[i], vkey);

                const vector<Line>& lines = L[edgekey];
                a.insert(a.end(), lines.begin(), lines.end() );
            }
            sweepLine(a);
            for (size_t i=0; i<v.in.size();++i) {
                Lattice::EdgeKey edgekey(v.in[i], vkey);
                L.erase(edgekey);
            }
        }

        for (size_t i=0; i<v.out.size();++i) {
            Lattice::EdgeKey edgekey(vkey, v.out[i]);
            Lattice::Edge &edge = lattice.getEdge(edgekey);

            double dot_dir = dotProduct(edge.h, dir);
            double dot_lambda = dotProduct(edge.h, lambda);

            vector<Line> &lines = L[edgekey];
            lines = a;
            for (size_t j = 0; j < a.size(); ++j) {
                lines[j].m += dot_dir;
                lines[j].y += dot_lambda;
//                cout << "    edge phrase " << edge.phrase << endl;
                lines[j].hypothesis.insert(
                    lines[j].hypothesis.end(), edge.phrase.begin(), edge.phrase.end());
            }
        }
        v_it.findNext();
    }
    size_t K = a.size();
    for (size_t i = 0; i < K; i++) {
        Line &l = a[i];
        cout << "  line " << l.x << " " << l.hypothesis << endl;
    }
}

