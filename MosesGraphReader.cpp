#include <iostream>

#include <assert.h>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "MosesGraphReader.h"
#include "Types.h"
using namespace boost;
using namespace std;

// 0 hyp=0 stack=0
// 0 hyp=37 stack=1 back=0 [ w: -2 u: 0 d: -4 0 0 -1.03429 0 0 0 lm: -10.9675 tm: -0.442792 -0.794259 -1.87952 -2.90528 0.999896 ] out=die inflation

regex MosesGraphReader::rx_line0("^(\\d+) hyp=(\\d+) ");
regex MosesGraphReader::rx_line1("^back=(\\d+) \\[ (.*) \\] out=(.*) $");
regex MosesGraphReader::rx_line2("\\w+:");
regex MosesGraphReader::rx_line3("-?\\d+(\\.\\d+)?");

MosesGraphReader::MosesGraphReader(istream &is) : is(is)
{
    string line;
    getline(is, line);
}

void MosesGraphReader::parseLine(const string &line, Entry &e)
{
    string suffix = line;

    getSentenceNumber(suffix, e);
    getHypothesis(suffix, e);
    if (e.hyp == 0) return;
    getBackRef(suffix, e);
    getFeatures(suffix, e);
    getPhrase(suffix, e);
}

void MosesGraphReader::getSentenceNumber(string &suffix, Entry &e)
{
    size_t pos = suffix.find_first_of(' ');
    e.sentence = lexical_cast<size_t>(suffix.substr(0, pos));
    suffix = suffix.substr(pos + 1);
}

void MosesGraphReader::getHypothesis(string &suffix, Entry &e)
{
    size_t pos1 = suffix.find_first_of('=');
    size_t pos2 = suffix.find_first_of(' ');
    pos1++;
    e.hyp = lexical_cast<size_t>(suffix.substr(pos1, pos2 - pos1));
    suffix = suffix.substr(pos2 + 1);
}

void MosesGraphReader::getBackRef(string &suffix, Entry &e)
{
    size_t pos1 = suffix.find_first_of('=');
    size_t pos2 = suffix.find_first_of(' ');
    pos1++;
    e.back = lexical_cast<size_t>(suffix.substr(pos1, pos2 - pos1));
    suffix = suffix.substr(pos2 + 1);
}

void MosesGraphReader::getFeatures(string &suffix, Entry &e)
{
    // suffix[0] == '['
    size_t pos1 = 0;
    while (pos1 < suffix.size()) {
        size_t pos2 = suffix.find_first_of(' ', pos1);
        if (pos2 == string::npos) break;

        // get the token
        const string &token = suffix.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;

        // check for the brackets
        if (token == "[") continue;
        if (token == "]") break;

        // check if it is not a descriptor
        if (token[token.size() - 1] != ':') {
            FeatureValue feature = lexical_cast<FeatureValue>(token);
            e.features.push_back(feature);
//            cout << "  [" << feature << "]" << endl;
        }
    }
    suffix = suffix.substr(pos1);
}

void MosesGraphReader::getPhrase(string &suffix, Entry &e)
{
    size_t pos1 = suffix.find_first_of('=');
    pos1++;
    while (pos1 < suffix.size()) {
        size_t pos2 = suffix.find_first_of(' ', pos1);

        // get the token
        const string &token = (pos2 != string::npos) ? suffix.substr(pos1, pos2 - pos1) : suffix.substr(pos1);
        pos1 = pos2 + 1;

//        cout << "  [" << token << "]" << endl;
        e.phrase.push_back(token);
    }
    suffix = suffix.substr(pos1);
}

bool MosesGraphReader::GetNextLattice(Lattice &lattice)
{
    if (is.eof()) return false;
    while (!is.eof()) {
        string line;
        getline(is, line);
        if (is.eof()) break;

        Entry e;
        parseLine(line, e);

        if (e.hyp == 0) break;

        // add the edge to the lattice
//        cout << "Edge " << backId << " - " << hypId << " phrase [" << phrase << "]" << endl;
//        cout << "Lattice vertices " << lattice.getVertexCount() << " edges " << lattice.getEdgeCount() << endl;

        Lattice::Edge edge;
        edge.h = e.features;
        edge.phrase = e.phrase;
        edge.from = e.back;
        edge.to = e.hyp;
        lattice.addEdge(edge);

        for (size_t i = 0; i < edge.phrase.size(); i++)
            assert( edge.phrase[i].length() > 0 );
    }
    cout << "Lattice vertices " << lattice.getVertexCount() << " edges " << lattice.getEdgeCount() << endl;
    return true;
}
