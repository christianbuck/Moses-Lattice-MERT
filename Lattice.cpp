#include <limits>
#include <iostream>
#include <cassert>
#include <list>
#include <queue>
#include <algorithm>
#include "Lattice.h"
#include "Types.h"
#include "Util.h"

using std::numeric_limits;
using std::vector;
using std::list;
using std::ostream;
using std::cout;
using std::endl;

struct compare_lists
{
  bool operator()(const list<Line*>* a, const list<Line*>* b)
  {
    // reverse order
    return !Line::CompareBySlope(*a->front(), *b->front());
  }
};

void mergeAndSweep(vector<list<Line*>* >& input, list<Line*>& a) {
  std::priority_queue<list<Line*>*, vector<list<Line*>* >, compare_lists> pq(input.begin(), input.end());
//  cout << endl;
  int nlines = 0;
  for (int i = 0; i < int(input.size()); ++i) {
    nlines+=input[i]->size();
  }
  while (!pq.empty()) {
    list<Line*>* p = pq.top();
    Line* const l = p->front();
//    cout << l->slope << "(" << a.size() << ") ";

    bool discard_line = false;
    if (!a.empty())
    {
      const Line* const prev = a.back();
      assert (prev->slope <= l->slope);
      if (prev->slope == l->slope) {
        if (l->offset <= prev->offset) {
          discard_line = true;
        } else {
          a.pop_back();
        }
      }
      while (!discard_line && !a.empty())
      {
        const Line* const prev = a.back();
        l->leftBound = (l->offset - prev->offset) / (prev->slope - l->slope);
        if (l->leftBound > prev->leftBound)
          break;
        a.pop_back();
      }
    }
    if (a.empty()) {
      l->leftBound = -numeric_limits<double>::infinity();
    }
    if (!discard_line) {
      a.push_back(l);
    }

    p->pop_front();
    pq.pop();
    if (!p->empty())
      pq.push(p);
  }
//  cout << endl << "bef:" << nlines << " after: " << a.size() << endl;
}

struct compare_vectors
{
  bool operator()(const vector<Line*>* a, const vector<Line*>* b)
  {
    // reverse order
    return Line::CompareBySlope(*a->back(), *b->back());
  }
};

struct compare_lineptr
{
  bool operator()(const Line* a, const Line* b)
  {
    // reverse order
    return !Line::CompareBySlope(*a, *b);
  }
};

void mergeAndSweep(vector<vector<Line*>* >& input, vector<Line*>& a) {
  std::priority_queue<vector<Line*>*, vector<vector<Line*>* >, compare_vectors> pq(input.begin(), input.end());
//  cout << endl;
  int nlines = 0;
  for (int i = 0; i < int(input.size()); ++i) {
    nlines+=input[i]->size();
  }
  while (!pq.empty()) {
    vector<Line*>* const pv = pq.top();
    Line* const l = pv->back();
//    cout << l->slope << "(" << a.size() << ") ";

    bool discard_line = false;
    if (!a.empty())
    {
      const Line* const prev = a.back();
      assert (prev->slope <= l->slope);
      if (prev->slope == l->slope) {
        if (l->offset <= prev->offset) {
          discard_line = true;
        } else {
          a.pop_back();
        }
      }
      while (!discard_line && !a.empty())
      {
        const Line* const prev = a.back();
        l->leftBound = (l->offset - prev->offset) / (prev->slope - l->slope);
        if (l->leftBound > prev->leftBound)
          break;
        a.pop_back();
      }
    }
    if (a.empty()) {
      l->leftBound = -numeric_limits<double>::infinity();
    }
    if (!discard_line) {
      a.push_back(l);
    }

    pv->pop_back();
    pq.pop();
    if (!pv->empty())
      pq.push(pv);
  }
//  cout << endl << "bef:" << nlines << " after: " << a.size() << endl;
  reverse(a.begin(),a.end());
}

void mergeAndSweep2(vector<vector<Line*>* >& input, vector<Line*>& a) {
  vector<Line*> lines;
  for (vector<vector<Line*>* >::const_iterator in_it = input.begin(); in_it < input.end(); in_it++ ) {
    lines.insert(lines.end(), (*in_it)->begin(), (*in_it)->end());
  }
  sort(lines.begin(), lines.end(), Line::ComparePtrBySlope);
//  cout << endl;
  int nlines = 0;
  for (int i = 0; i < int(input.size()); ++i) {
    nlines+=input[i]->size();
  }

  for (vector<Line*>::const_iterator it = lines.begin(); it<lines.end(); it++) {
    Line* const l = (*it);
//    cout << l->slope << "(" << a.size() << ") ";

    bool discard_line = false;
    if (!a.empty())
    {
      const Line* const prev = a.back();
      assert (prev->slope <= l->slope);
      if (prev->slope == l->slope) {
        if (l->offset <= prev->offset) {
          discard_line = true;
        } else {
          a.pop_back();
        }
      }
      while (!discard_line && !a.empty())
      {
        const Line* const prev = a.back();
        l->leftBound = (l->offset - prev->offset) / (prev->slope - l->slope);
        if (l->leftBound > prev->leftBound)
          break;
        a.pop_back();
      }
    }
    if (a.empty()) {
      l->leftBound = -numeric_limits<double>::infinity();
    }
    if (!discard_line) {
      a.push_back(l);
    }
  }
//  cout << endl << "bef:" << nlines << " after: " << a.size() << endl;
//  reverse(a.begin(),a.end());
}

void latticeEnvelope(Lattice& lattice, const FeatureVector& dir,
    const FeatureVector& lambda, vector<Line>& avec)
{
  // temporary object for storing hull lines that are associated with edges
  vector<vector<Line*> > L(lattice.getEdgeCount());
  vector<Line*> lineCache;
  vector<Line*> a;       // hull for the current vertex

  vector<size_t> start;
  start.push_back(0);     // 0 is the source node key in lattice
  TopoIterator v_it(lattice, start);

  // traverse the lattice in topological order
  while (!v_it.isEnd())
  {
    a.clear();
    const size_t vkey = v_it.get();
    Lattice::Vertex& v = lattice.getVertex(vkey);
    // special case for source node: insert horizontal line
    if (vkey == 0) {
      assert (lineCache.empty());
      Line* const l = new Line();
      lineCache.push_back(l);
      a.push_back(l);
    } else {
      // merge hulls associated with incoming edges into single sorted list of lines
      vector<vector<Line*>* > alines;
      for (size_t i = 0; i < v.in.size(); ++i)
      {
        const Lattice::EdgeKey edgekey = v.in[i];
        alines.push_back(&L[edgekey]);
      }
      mergeAndSweep2(alines, a);
    }
    assert (!a.empty());

    // update hulls associated with outgoing edges
    for (size_t i = 0; i < v.out.size(); ++i)
    {
      const Lattice::EdgeKey edgekey = v.out[i];
      const Lattice::Edge& edge = lattice.getEdge(edgekey);
      vector<Line*>& lines = L[edgekey];
      for (vector<Line*>::const_iterator li=a.begin(); li != a.end(); li++) {
        Line* const l = new Line(**li); // copy line
        lineCache.push_back(l);
        lines.push_back(l);
      }
      // update unless the edge leads to the sink node and has no feature score vector
      if (edge.scores.size() > 0)
      {
        const double dot_dir = dotProduct(edge.scores, dir);
        const double dot_lambda = dotProduct(edge.scores, lambda);
        for (vector<Line*>::const_iterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          Line& l = **lit;
          l.slope += dot_dir;
          l.offset += dot_lambda;
          l.addEdge(lattice, edgekey);
        }
      }
    }
    v_it.findNext();
  }
  for (vector<Line*>::const_iterator lit = a.begin(); lit != a.end(); ++lit) {
    // avec.push_back(Line(**lit));
    avec.push_back(**lit);
  }
  for (vector<Line*>::const_iterator cit = lineCache.begin(); cit != lineCache.end(); ++cit) {
    delete *cit;
  }
}

// Implementation of Algorithm 2 from the paper
// The function computes upper envelope for the best translations in the lattice
// Given feature weights lambda and direction vector dir,
// it finds the boundary points along the line described by dir.
// Each boundary point is described by a Line structure.
void latticeEnvelope2(Lattice& lattice, const FeatureVector& dir,
    const FeatureVector& lambda, vector<Line>& avec)
{
  // temporary object for storing hull lines that are associated with edges
  vector<vector<Line*> > L(lattice.getEdgeCount());
  vector<Line*> lineCache;
  vector<Line*> a;       // hull for the current vertex

  vector<size_t> start;
  start.push_back(0);     // 0 is the source node key in lattice
  TopoIterator v_it(lattice, start);

  // traverse the lattice in topological order
  while (!v_it.isEnd())
  {
    a.clear();
    const size_t vkey = v_it.get();
    Lattice::Vertex& v = lattice.getVertex(vkey);
    // special case for source node: insert horizontal line
    if (vkey == 0) {
      assert (lineCache.empty());
      Line* const l = new Line();
      lineCache.push_back(l);
      a.push_back(l);
    } else {
      // merge hulls associated with incoming edges into single sorted list of lines
      vector<vector<Line*>* > alines;
      for (size_t i = 0; i < v.in.size(); ++i)
      {
        const Lattice::EdgeKey edgekey = v.in[i];
        alines.push_back(&L[edgekey]);
      }
      mergeAndSweep(alines, a);
    }
    assert (!a.empty());
    // update hulls associated with outgoing edges
    for (size_t i = 0; i < v.out.size(); ++i)
    {
      const Lattice::EdgeKey edgekey = v.out[i];
      const Lattice::Edge& edge = lattice.getEdge(edgekey);
      vector<Line*>& lines = L[edgekey];
      for (vector<Line*>::const_iterator li=a.begin(); li != a.end(); li++) {
        Line* const l = new Line(**li); // copy line
        lineCache.push_back(l);
        lines.push_back(l);
      }
      // update unless the edge leads to the sink node and has no feature score vector
      if (edge.scores.size() > 0)
      {
        const double dot_dir = dotProduct(edge.scores, dir);
        const double dot_lambda = dotProduct(edge.scores, lambda);
        for (vector<Line*>::const_iterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          Line& l = **lit;
          l.slope += dot_dir;
          l.offset += dot_lambda;
          l.addEdge(lattice, edgekey);
        }
      }
    }
    v_it.findNext();
  }
  for (vector<Line*>::const_iterator lit = a.begin(); lit != a.end(); ++lit) {
    // avec.push_back(Line(**lit));
    avec.push_back(**lit);
  }
  for (vector<Line*>::const_iterator cit = lineCache.begin(); cit != lineCache.end(); ++cit) {
    delete *cit;
  }
}

void latticeEnvelope3(Lattice& lattice, const FeatureVector& dir,
    const FeatureVector& lambda, vector<Line>& avec)
{
  // temporary object for storing hull lines that are associated with edges
  vector<list<Line*> > L(lattice.getEdgeCount());
  vector<Line*> lineCache;
  list<Line*> a;       // hull for the current vertex

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
      assert (lineCache.empty());
      Line* const l = new Line();
      lineCache.push_back(l);
      a.push_back(l);
    } else {
      // merge hulls associated with incoming edges into single sorted list of lines
      vector<list<Line*>* > alines;
      for (size_t i = 0; i < v.in.size(); ++i)
      {
        const Lattice::EdgeKey edgekey = v.in[i];
        alines.push_back(&L[edgekey]);
      }
      mergeAndSweep(alines, a);
    }
    assert (!a.empty());
    // update hulls associated with outgoing edges
    for (size_t i = 0; i < v.out.size(); ++i)
    {
      const Lattice::EdgeKey edgekey = v.out[i];
      const Lattice::Edge& edge = lattice.getEdge(edgekey);
      list<Line*>& lines = L[edgekey];
      for (list<Line*>::const_iterator li=a.begin(); li != a.end(); li++) {
        Line* l = new Line(**li); // copy line
        lineCache.push_back(l);
        lines.push_back(l);
      }
      // update unless the edge leads to the sink node and has no feature score vector
      if (edge.scores.size() > 0)
      {
        const double dot_dir = dotProduct(edge.scores, dir);
        const double dot_lambda = dotProduct(edge.scores, lambda);
        for (list<Line*>::const_iterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          Line& l = **lit;
          l.slope += dot_dir;
          l.offset += dot_lambda;
          l.addEdge(lattice, edgekey);
        }
      }
    }
    v_it.findNext();
  }
  for (list<Line*>::const_iterator lit = a.begin(); lit != a.end(); ++lit) {
    avec.push_back(Line(**lit));
  }
  for (vector<Line*>::const_iterator cit = lineCache.begin(); cit != lineCache.end(); ++cit) {
    delete *cit;
  }

  // avec.insert(avec.end(), a.begin(), a.end());
  //for_each(lineCache.begin(), lineCache.end(), DeleteObject());
}
