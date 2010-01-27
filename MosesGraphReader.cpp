#include <iostream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "MosesGraphReader.h"

using namespace boost;
using namespace std;

// 0 hyp=0 stack=0 forward=1 fscore=-10.5248
// 0 hyp=37 stack=1 back=0 score=-0.975545 transition=-0.975545 [ w: -2 u: 0 d: -4 0 0 -1.03429 0 0 0 lm: -10.9675 tm: -0.442792 -0.794259 -1.87952 -2.90528 0.999896 ] forward=194 fscore=-6.27491 covered=4-4 out=die inflation , pC=-0.244107, c=-0.932361

regex MosesGraphReader::rx_line0("(\\d+) hyp=(\\d+) stack=(\\d+) ");
regex MosesGraphReader::rx_line1("back=(\\d+) score=(.*) transition=(.*) \\[ (.*) \\] ");
regex MosesGraphReader::rx_line4("recombined=(\\d+) ");
regex MosesGraphReader::rx_line5(" out=(.*) , pC");
regex MosesGraphReader::rx_line2("\\w+:");
regex MosesGraphReader::rx_line3("-?\\d+(\\.\\d+)?");

MosesGraphReader::MosesGraphReader(istream &is) : is(is)
{
    string line;
    getline(is, line);
}

bool MosesGraphReader::GetNextLattice(Lattice &lattice)
{
    while (!is.eof()) {
        string line;
        getline(is, line);
        if (is.eof()) return false;

        smatch m;
        if (!regex_search(line, m, rx_line0, match_continuous)) {
            cout << "Line not matching! [" << line << "]" << endl;
            return false;
        }
        string hypIdStr = string(m[2].first, m[2].second);
        size_t hypId = lexical_cast<size_t>(hypIdStr);

        if (hypId == 0) {
            string sentenceIdStr = string(m[1].first, m[1].second);
            size_t sentenceId = lexical_cast<size_t>(sentenceIdStr);
            cout << "Sentence " << sentenceId << endl;
            break;
        }

        string suffix = m.suffix();
        if (!regex_search(suffix, m, rx_line1, match_continuous)) {
            cout << "Line not matching! [" << suffix << "]" << endl;
            return false;
        }
        string backIdStr = string(m[1].first, m[1].second);
        size_t backId = lexical_cast<size_t>(backIdStr);

        string featureListStr = string(m[4].first, m[4].second);

        // look if there is recombination info
        suffix = m.suffix();
        if (regex_search(suffix, m, rx_line4, match_continuous)) {
            string recombIdStr = string(m[1].first, m[1].second);
            size_t recombId = lexical_cast<size_t>(recombIdStr);
//          cout << "  recombined " << hypId << " to " << recombId << endl;
            hypId = recombId;
        }

        if (!regex_search(suffix, m, rx_line5)) {
            cout << "Line not matching! [" << suffix << "]" << endl;
            return false;
        }
        string phrase = string(m[1].first, m[1].second);

//      cout << "Parsing feature string [" << featureListStr << "]" << endl;
        vector<double> featureList;
        while (featureListStr.size() > 0) {
            // really inefficient way to trim spaces from the start
            while (featureListStr.size() > 0) {
                if (featureListStr[0] != ' ') break;
                featureListStr = featureListStr.substr(1);
            }
            if (featureListStr.size() == 0) break;
            // try to match feature description
            if (regex_search(featureListStr, m, rx_line2, match_continuous)) {
                featureListStr = m.suffix();
                continue;
            }
            // try to match feature value
            if (regex_search(featureListStr, m, rx_line3, match_continuous)) {
                string featureStr = string(m[0].first, m[0].second);
//              cout << "Converting [" << featureStr << "]" << endl;
                double feature = lexical_cast<double>(featureStr);
                featureList.push_back(feature);
                featureListStr = m.suffix();
                continue;
            }
            cout << "feature string [" << featureListStr << "] not matching!" << endl;
            return false;
        }

        // add the edge to the lattice
//        cout << "Edge " << backId << " - " << hypId << " phrase [" << phrase << "]" << endl;
//        cout << "Lattice vertices " << lattice.getVertexCount() << " edges " << lattice.getEdgeCount() << endl;
        Lattice::Edge &edge = lattice.addEdge(backId, hypId);
        edge.features = featureList;
        edge.phrase = phrase;
    }
    cout << "Lattice vertices " << lattice.getVertexCount() << " edges " << lattice.getEdgeCount() << endl;
    return true;
}
