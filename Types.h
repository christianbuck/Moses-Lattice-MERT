#pragma once

#include <limits>
#include <vector>
#include <string>
//#include <map>
#include <iostream>
// #include "BleuScorer.h"

//using std::map;
using std::pair;
using std::vector;
using std::string;
using std::wstring;
using std::numeric_limits;
using std::ostream;

typedef wstring String;
typedef vector<string> Phrase;
typedef double FeatureValue; 
typedef vector<FeatureValue> FeatureVector;

typedef pair<double, vector<int> > boundary;

const size_t bleuOrder = 4;

struct Interval
{
    Interval() {
        score = -numeric_limits<double>::infinity();
        left = 0.0;
        right = 0.0;
    }
    double score;
    double left;
    double right;
};

struct Result
{
    double score;
    FeatureVector lambdas;
    Result(double s) {
        score = s;
    }
};
    
    
    
    
ostream & operator << (ostream &os, const Phrase& p);



