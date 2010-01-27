#include <limits>
#include <iostream>
#include <algorithm>

#include "Lattice.h"
#include "Types.h"

using std::numeric_limits;
using std::vector;
using std::map;
using std::ostream;
using std::cout;
using std::endl;

double dotProduct(const vector<double> &a, const vector<double> &b)
{
    size_t K = a.size();
    double sum = 0;
    for (size_t i = 0; i < K; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

ostream & operator << (ostream &os, const Phrase& p) {
    for (size_t i = 0; i < p.size(); i++) {
        if (i > 0) os << " + ";
        os << p[i];
    }
    return os;
}

void sweepLine(vector<Line> &a)
{
    sort (a.begin(), a.end(), Line::CompareBySlope);

    size_t j = 0;
    size_t K = a.size();
    for (size_t i = 0; i < K; i++) {
        Line &l = a[i];
//        cout << "    line " << l.m << " " << l.y << " " << l.hypothesis << endl;
        l.x = -numeric_limits<double>::max();
        if (0 < j) {
            if (a[j - 1].m == l.m) {
                if (l.y <= a[j - 1].y) continue;
                --j;
            }
            while (0 < j) {
                l.x = (l.y - a[j - 1].y) / (a[j - 1].m - l.m);
                if (a[j - 1].x < l.x) break;
                --j;
            }
            if (0 == j) l.x = -numeric_limits<double>::max();
        }
        a[j++] = l;
    }
    cout << "  SweepLine: |a| = " << a.size() << " -> " << j << endl;
    a.resize(j);
}

vector<Line> latticeEnvelope(Lattice &lattice, const vector<double>& d, const vector<double>& lambdas)
{
    map<Lattice::EdgeKey, vector<Line> > L;

    vector<size_t> start;
    start.push_back(0);

    TopoIterator v_it(lattice, start);
    while (true) {
        size_t vkey = v_it.get();
        Lattice::Vertex & v = lattice.getVertex(vkey);

        cout << "Visiting " << vkey << endl;

        cout << "  In: " << v.in.size() << " Out: " << v.out.size() << "|L|=" << L.size() << endl;
        vector<Line> a;
        for (size_t i=0; i<v.in.size();++i) {
            Lattice::EdgeKey edgekey(v.in[i], vkey);

            const vector<Line>& lines = L[edgekey];
            a.insert(a.end(), lines.begin(), lines.end() );
        }
        if (a.size() == 0) {
            Line line0;
            line0.x = -numeric_limits<double>::max();
            line0.y = 0;
            line0.m = 0;
            a.push_back(line0);
        }
        sweepLine(a);
        for (size_t i=0; i<v.in.size();++i) {
            Lattice::EdgeKey edgekey(v.in[i], vkey);
            L.erase(edgekey);
        }

        for (size_t i=0; i<v.out.size();++i) {
            Lattice::EdgeKey edgekey(vkey, v.out[i]);
            Lattice::Edge &edge = lattice.getEdge(edgekey);

            double dot_d = dotProduct(edge.features, d);
            double dot_lambda = dotProduct(edge.features, lambdas);

            vector<Line> &lines = L[edgekey];
            lines = a;
            for (size_t j = 0; j < a.size(); ++j) {
                lines[j].m += dot_d;
                lines[j].y += dot_lambda;
//                cout << "    edge phrase " << edge.phrase << endl;
                lines[j].hypothesis.push_back(edge.phrase);
            }
        }
        cout << "  |L| = " << L.size() << endl;
        v_it.findNext();
        if (v_it.isEnd())
            return a;
    }
}

