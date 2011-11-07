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

struct compare_lineptr
{
  bool operator()(const Line* a, const Line* b)
  {
    // reverse order
    return !Line::CompareBySlope(*a, *b);
  }
};

void mergeAndSweep(vector<vector<Line*>*>& input, vector<Line*>& a)
{
  vector<Line*> lines;
  for (vector<vector<Line*>*>::const_iterator in_it = input.begin();
      in_it < input.end(); ++in_it)
  {
    lines.insert(lines.end(), (*in_it)->begin(), (*in_it)->end());
  }
  sort(lines.begin(), lines.end(), Line::ComparePtrBySlope);
  size_t a_size = a.size();
  for (vector<Line*>::const_iterator it = lines.begin(); it < lines.end(); ++it)
  {
    Line* const l = (*it);
    bool discard_line = false;
    if (a_size>0)
    {
      const Line* const prev = a.back();
      assert(prev->slope <= l->slope);
      if (prev->slope == l->slope)
      {
        if (l->offset <= prev->offset)
        {
          discard_line = true;
        }
        else
        {
          a.pop_back();
          --a_size;
        }
      }
      while (!discard_line && a_size>0)
      {
        const Line* const prev = a.back();
        l->leftBound = (l->offset - prev->offset) / (prev->slope - l->slope);
        if (l->leftBound > prev->leftBound)
          break;
        a.pop_back();
        --a_size;
      }
    }
    if (a_size==0)
    {
      l->leftBound = -numeric_limits<double>::infinity();
    }
    if (!discard_line)
    {
      a.push_back(l);
      ++a_size;
    }
  }
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
    if (vkey == 0)
    {
      assert(lineCache.empty());
      Line* const l = new Line();
      lineCache.push_back(l);
      a.push_back(l);
    }
    else
    {
      // merge hulls associated with incoming edges into single sorted list of lines
      vector<vector<Line*>*> alines;
      for (size_t i = 0; i < v.in.size(); ++i)
      {
        const Lattice::EdgeKey edgekey = v.in[i];
        alines.push_back(&L[edgekey]);
      }
      mergeAndSweep(alines, a);
    }
    assert(!a.empty());

    // update hulls associated with outgoing edges
    const size_t n_outedges = v.out.size();
    //lineCache.reserve(lineCache.size()+(n_outedges-1)*a.size());
    for (size_t i = 0; i < n_outedges; ++i)
    {
      const Lattice::EdgeKey edgekey = v.out[i];
      const Lattice::Edge& edge = lattice.getEdge(edgekey);
      vector<Line*>& lines = L[edgekey];
      lines.reserve(a.size());
      //lineCache.reserve(lineCache.size()+a.size());
      for (vector<Line*>::const_iterator li = a.begin(); li != a.end(); ++li)
      {
        if (i==n_outedges-1) {
          lines.push_back(*li); // re-use lines from a
        } else {
          Line* const l = new Line(**li); // copy line
          lineCache.push_back(l);
          lines.push_back(l);
        }
      }
      // update unless the edge leads to the sink node and has no feature score vector
      const bool is_sinknode = (edge.scores.size()==0);
      if (!is_sinknode)
      {
        const double dot_dir = dotProduct(edge.scores, dir);
        const double dot_lambda = dotProduct(edge.scores, lambda);
        for (vector<Line*>::const_iterator lit = lines.begin();
            lit != lines.end(); ++lit)
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
  for (vector<Line*>::const_iterator lit = a.begin(); lit != a.end(); ++lit)
  {
    avec.push_back(**lit);
  }
  for (vector<Line*>::const_iterator cit = lineCache.begin();
      cit != lineCache.end(); ++cit)
  {
    delete *cit;
  }
}
