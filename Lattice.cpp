#include <limits>
#include <iostream>
#include <algorithm>
#include <cassert>

#include <list>
#include <queue>

#include "Lattice.h"
#include "Types.h"
#include "Util.h"

using std::numeric_limits;
using std::vector;
using std::list;
using std::map;
using std::ostream;
using std::cout;
using std::endl;

ostream & operator <<(ostream &os, const Phrase& p)
{
  for (size_t i = 0; i < p.size(); i++)
  {
    if (i > 0)
      os << " ";
    os << p[i];
  }
  return os;
}

struct compare_lists
{
  bool operator()(const list<Line>* a, const list<Line>* b)
  {
    // reverse order
    return !Line::CompareBySlope(a->front(), b->front());
  }
};

void mergeAndSweep(vector<list<Line>* >& input, list<Line>& a) {
  std::priority_queue<list<Line>*, vector<list<Line>* >, compare_lists> pq(input.begin(), input.end());
  while (!pq.empty()) {
    list<Line>* p = pq.top();
    Line& l = p->front();

    bool discard_line = false;
    if (!a.empty())
    {
      if (a.back().slope == l.slope) {
        if (l.offset <= a.back().offset) {
          discard_line = true;
        } else {
          a.pop_back();
        }
      }
      while (!discard_line && !a.empty())
      {
        l.leftBound = (l.offset - a.back().offset) / (a.back().slope - l.slope);
        if (l.leftBound > a.back().leftBound)
          break;
        a.pop_back();
      }
    }
    if (a.empty()) {
      l.leftBound= -numeric_limits<double>::infinity();
    }
    if (!discard_line) {
      a.push_back(l);
    }

    p->pop_front();
    pq.pop();
    if (!p->empty())
      pq.push(p);
  }
}

// Implementation of Algorithm 2 from the paper
// The function computes upper envelope for the best translations in the lattice
// Given feature weights lambda and direction vector dir,
// it finds the boundary points along the line described by dir.
// Each boundary point is described by a Line structure.

void latticeEnvelope(Lattice &lattice, const FeatureVector &dir,
    const FeatureVector &lambda, vector<Line> &avec)
{
  // temporary object for storing hull lines that are associated with edges
  vector<list<Line> > L(lattice.getEdgeCount());
  list<Line> a;       // hull for the current vertex

  vector<size_t> start;
  start.push_back(0);     // 0 is the source node key in lattice
  TopoIterator v_it(lattice, start);

  // traverse the lattice in topological order
  while (!v_it.isEnd())
  {
    a.clear();
    const size_t vkey = v_it.get();
    Lattice::Vertex & v = lattice.getVertex(vkey);
    // special case for source node: insert horizontal line
    if (vkey == 0) {
      a.push_back(Line());
    } else {
      // merge hulls associated with incoming edges into single sorted list of lines
      vector<list<Line>* > alines;
      for (size_t i = 0; i < v.in.size(); ++i)
      {
        const Lattice::EdgeKey edgekey = v.in[i];
        alines.push_back(&L[edgekey]);
      }
      mergeAndSweep(alines, a);
    }

    // update hulls associated with outgoing edges
    for (size_t i = 0; i < v.out.size(); ++i)
    {
      const Lattice::EdgeKey edgekey = v.out[i];
      Lattice::Edge &edge = lattice.getEdge(edgekey);

      L[edgekey] = a; // copies all the lines
      // update unless the edge leads to the sink node and has no feature score vector
      if (edge.scores.size() > 0)
      {
        list<Line>& lines = L[edgekey];
        const double dot_dir = dotProduct(edge.scores, dir);
        const double dot_lambda = dotProduct(edge.scores, lambda);
        for (list<Line>::iterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          lit->slope += dot_dir;
          lit->offset += dot_lambda;
          lit->addEdge(lattice, edgekey);
        }
      }
    }
    v_it.findNext();
  }
  avec.insert(avec.end(), a.begin(), a.end());
}
