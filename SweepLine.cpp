#include <vector>
#include <algorithm>
#include <limits>

using std::vector;
using std::numeric_limits;

struct Line
{
    // describes a line y = y0 + m * x
    double  x;      // x intercept with the left adjacent line
    double  y0;     // y intercept with x = 0 (y-offset)
    double  m;      // slope coefficient
    size_t  index;
    
    static bool CompareBySlope(const Line &a, const Line &b)
    {
        return a.m < b.m;
    }
};


// Implementation of Algorithm 1
// Calculates the upper envelope for a set of lines 
// The algorithm modifies the line array in place

void algorithm1(vector<Line> &a)
{
    sort (a.begin(), a.end(), Line::CompareBySlope);

    size_t j = 0;
    size_t K = a.size();
    for (size_t i = 0; i < K; i++) {
        Line &l = a[i];
        l.x = -numeric_limits<double>::max();
        if (0 < j) {
            if (a[j - 1].m == l.m) {
                if (l.y0 <= a[j - 1].y0) continue;
                --j;
            }
            while (0 < j) {
                l.x = (l.y0 - a[j - 1].y0) / (a[j - 1].m - l.m);
                if (a[j - 1].x < l.x) break;
                --j;
            }
            if (0 == j) l.x = -numeric_limits<double>::max();
        } 
        a[j++] = l;
    }
    a.resize(j);
}

