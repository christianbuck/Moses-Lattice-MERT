#pragma once

#include <limits>
#include <vector>
#include <string>
#include <map>
#include <iostream>
using std::map;
using std::pair;
using std::vector;
using std::string;
using std::wstring;
using std::numeric_limits;
using std::ostream;

typedef wstring String;
typedef vector<string> Phrase;
typedef vector<double> FeatureVector;

typedef map<Phrase,int> NgramCounts;
typedef pair<double, vector<int> > boundary;

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

    BleuStats(const size_t length, const double leftBoundary) {
        this->leftBoundary = leftBoundary;
        this->length = length;
        for (size_t n = 0; n < 4; ++n) { counts[n]=0; }
    }
};

struct Interval
{
    double score;
    double left;
    double right;
};

ostream & operator << (ostream &os, const Phrase& p); 



