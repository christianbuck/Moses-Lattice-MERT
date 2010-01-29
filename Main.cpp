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
#include "Parameters.h"

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
    for (size_t i = 0; i < reference.size(); i++)
        assert( reference[i].length() > 0 );
}

void doMagic(const Parameters &params)
{
    vector<double> dir;
//    vector<double> lambdas;
//    lambdas.push_back(-0.21);
//    lambdas.push_back(1.0);
//    lambdas.push_back(0.02);
//    lambdas.push_back(0.05);
//    lambdas.push_back(0.01);
//    lambdas.push_back(0.21);
//    lambdas.push_back(0.05);
//    lambdas.push_back(0.05);
//    lambdas.push_back(0.15);
//    lambdas.push_back(0.08);
//    lambdas.push_back(0.02);
//    lambdas.push_back(0.05);
//    lambdas.push_back(0.06);
//    lambdas.push_back(0.03);
//    lambdas.push_back(0.01);
    for (size_t i = 0; i < 15; i++) {
        dir.push_back((i == 9) ? 1.0 : 0.0);
    }

    ifstream is_ref(params.referencePath);
    ifstream is_osg(params.inputPath);
    MosesGraphReader reader(is_osg);

    vector<boundary> cumulatedCounts;

    while (true) {
        Lattice lattice;
        if (!reader.GetNextLattice(lattice)) break;

        Phrase reference;
        readReference(is_ref, reference);
        cout << "Reference: [" << reference << "]" << endl;

        vector<Line> envelope;
        latticeEnvelope(lattice, dir, params.lambdas, envelope);

        vector<BleuStats> stats;
        computeBleuStats(lattice, envelope, reference, stats);

        accumulateBleu(stats, cumulatedCounts);
    }
    Interval bestInterval;
    optimizeBleu(cumulatedCounts, bestInterval);
}

void printParams(const Parameters &params)
{
    cout << "Parameters:" << endl;
    cout << "  Input path: " << params.inputPath << endl;
    cout << "  Reference path: " << params.referencePath << endl;
    cout << "  Lambda: ";
    for (size_t i = 0; i < params.lambdas.size(); i++)
        cout << params.lambdas[i] << " ";
    cout << endl;
}

int main(int argc, char **argv)
{
    Parameters params;

    params.parse(argc, argv);

    printParams(params);

    doMagic(params);

    return 0;
}


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
