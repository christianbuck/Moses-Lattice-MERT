#include <vector>
#include <string>

using std::vector;

typedef vector<string> Phrase;
typedef map<Phrase,size_t> NgramCounts;  

struct Line
{
    double  x;
    double  y;
    double  m;
    size_t  index;
    Phrase hypothesis;
    static bool CompareBySlope(const Line &a, const Line &b)
    {
        return a.m < b.m;
    }
};

struct Edge 
{
    vector<double> features;
    Phrase phrase;
    Vertex end;
    size_t idx;
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

    
    
    typedef map<Phrase,size_t> NgramCounts;  

    


