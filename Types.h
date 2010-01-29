#pragma once

#include <limits>
#include <vector>
#include <string>
#include <map>
#include <iostream>
// #include "BleuScorer.h"

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

const size_t bleuOrder = 4;
struct BleuStats
{
    double leftBoundary;
    size_t counts[bleuOrder*2];
    size_t length;

    BleuStats(const size_t length, const double leftBoundary) {
        this->leftBoundary = leftBoundary;
        this->length = length;
        for (size_t n = 0; n < bleuOrder; ++n) { counts[n]=0; }
    }
};

struct Interval
{
    double score;
    double left;
    double right;
};

ostream & operator << (ostream &os, const Phrase& p);



