#include <iostream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "Lattice.h"

using namespace boost;
using namespace std;

bool ReadGraph(istream &is)
{
    // 0 hyp=0 stack=0 forward=1 fscore=-10.5248
    // 0 hyp=21 stack=1 back=0 score=-7.46278 transition=-7.46278 forward=983 fscore=-8.85446 covered=1-1 out=iepakojumu , pC=-1.49323, c=-5.09407
    // 0 hyp=33 stack=1 back=0 score=-10.9963 transition=-10.9963 recombined=21 forward=983 fscore=-8.85446 covered=1-1 out=tādu iepakojumu , pC=-2.84394, c=-8.65174
    regex rx_line0("(\\d+) hyp=(\\d+) stack=(\\d+) ");
    regex rx_line1("back=(\\d+) score=(-?\\d+(\\.\\d+)?) ");
    regex rx_line2("transition=(-?\\d+(\\.\\d+)?) recombined=(\\d+)");

    Lattice lattice;

    while (!is.eof()) {
        string line;
        getline(is, line);
        if (is.eof()) break;

        smatch m;
        if (!regex_search(line, m, rx_line0, match_continuous)) {
            cout << "Line not matching!" << endl;
            return false;
        }
        string sentenceIdStr = string(m[1].first, m[1].second);
        string hypIdStr = string(m[2].first, m[2].second);

        size_t sentenceId = lexical_cast<size_t>(sentenceIdStr);
        size_t hypId = lexical_cast<size_t>(hypIdStr);

        if (hypIdStr == "0") continue;

        string suffix = m.suffix();
        if (!regex_search(suffix, m, rx_line1, match_continuous)) {
            cout << "Line not matching!" << endl;
            return false;
        }
        string backIdStr = string(m[1].first, m[1].second);

        size_t backId = lexical_cast<size_t>(backIdStr);

        cout << "Edge " << backId << " - " << hypId << endl;
        lattice.addEdge(backId, hypId);
    }
    cout << "Lattice vertices " << lattice.vertices.size() << endl;
    return true;
}
