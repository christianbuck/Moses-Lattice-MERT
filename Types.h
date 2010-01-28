#pragma once

#include <limits>
#include <vector>
#include <string>
#include <map>

using std::map;
using std::pair;
using std::vector;
using std::string;
using std::wstring;
using std::numeric_limits;

typedef wstring String;
typedef vector<string> Phrase;
typedef vector<double> FeatureVector;

typedef map<Phrase, size_t> NgramCounts;

struct Line
{
    double  x;
    double  y;
    double  m;
    size_t  index;
    Phrase hypothesis;

    Line() : x(-numeric_limits<double>::infinity()), y(0), m(0) {}

    static bool CompareBySlope(const Line &a, const Line &b)
    {
        return a.m < b.m;
    }
};

struct Vertex;

struct Edge
{
    vector<double> features;
    Phrase phrase;
    Vertex* end;
    size_t idx;

    Edge(Vertex* end) : end(end) {}
};

struct Vertex
{
    vector<Edge> in;
    vector<Edge> out;
    int nIncomingVisited;
};

struct BleuStats
{
    double leftBoundary;
    size_t counts[4];
    size_t length;
};

struct Interval
{
    double score;
    double left;
    double right;
};

typedef map<Phrase,size_t> NgramCounts;
typedef pair<double, vector<int> > boundary;




