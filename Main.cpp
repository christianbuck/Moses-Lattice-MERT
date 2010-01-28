#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <list>

#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "MosesGraphReader.h"
#include "BleuScorer.h"
#include "Lattice.h"
#include "Types.h"

using std::ifstream;
using std::istream;
using std::string;
using std::vector;
using std::list;
using std::cout;
using std::endl;
using std::ostream;

void readReference(istream &is_ref, Phrase &reference)
{
    string line;
    getline(is_ref, line);
    boost::split(reference, line, boost::is_any_of(" "));
}

void test1(void)
{
    vector<double> lambdas;
    vector<double> dir;
    for (size_t i = 0; i < 15; i++) {
        lambdas.push_back(1.0);
        dir.push_back((i == 9) ? 1.0 : 0.0);
    }

    ifstream is_ref("/home/karlos/Moses-Lattice-MERT/case1.ref");

    ifstream is_osg("/home/karlos/Moses-Lattice-MERT/case1.osg");
    MosesGraphReader reader(is_osg);

    vector<boundary> cumulatedCounts;

    while (true) {
        Lattice lattice;
        if (!reader.GetNextLattice(lattice)) break;

        vector<Line> envelope;
        latticeEnvelope(lattice, dir, lambdas, envelope);

        Phrase reference;
        readReference(is_ref, reference);
        // read corresponding reference translation

        vector<BleuStats> stats;
        computeBleuStats(envelope, reference, stats);

        accumulateBleu(stats, cumulatedCounts);
    }
    Interval bestInterval;
    optimizeBleu(cumulatedCounts, bestInterval);
}

int main(int argc, char **argv)
{
    // Read options

    // Choose directions
    // For each sentence
        // Read lattice into memory
        // For each direction
            // Propagate envelope through the lattice
            // Get gamma boundaries gamma_1..gamma_n
            // Count BLEU n-gram statistics for e_1..e_n

    // For each direction
        // Merge gamma points
        // Maximimize BLEU over gamma and direction

    // Update lambdas

    test1();

    return 0;
}

