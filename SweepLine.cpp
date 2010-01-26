#include <vector>
#include <algorithm>
#include <limits>

using std::vector;
using std::numeric_limits;

struct Line
{
    double  x;
    double  y;
    double  m;
    size_t  index;
    
    static bool CompareBySlope(const Line &a, const Line &b)
    {
        return a.m < b.m;
    }
};

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
    a.resize(j);
}

