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
	size_t nDimensions = params.lambdas.size();
	size_t nDirections = nDimensions; // might be higher of lower in case of random directions
    vector< vector<double> > directions(nDirections);
    for (size_t d=0;d<nDirections;d++) {
        for (size_t i = 0; i < nDimensions; i++) {
            directions[d].push_back((i == d) ? 1.0 : 0.0);
        }
    }

    ifstream is_ref(params.referencePath);
    ifstream is_osg(params.inputPath);
    MosesGraphReader reader(is_osg);

    vector<vector<boundary> > differenceVectors(directions.size());
//    vector<boundary> &cumulatedCounts;

    while (true) {
        Lattice lattice;
        if (!reader.GetNextLattice(lattice)) break;

        Phrase reference;
        readReference(is_ref, reference);
        // cout << "Reference: [" << reference << "]" << endl;

		for (size_t d=0;d<nDirections;d++) {
		    cout << d << " ";
		    cout.flush();
            vector<boundary> &cumulatedCounts = differenceVectors[d];
			FeatureVector &dir = directions[d];
		    vector<Line> envelope;
		    latticeEnvelope(lattice, dir, params.lambdas, envelope);

		    vector<BleuStats> stats;
		    computeBleuStats(lattice, envelope, reference, stats);
		    accumulateBleu(stats, cumulatedCounts);
		}
	    cout << endl;
    }
    Interval bestInterval;
    size_t bestDirection = 0;
	for (size_t d=0;d<nDirections;d++) {
        vector<boundary> &cumulatedCounts = differenceVectors[d];
        Interval currInterval;
        optimizeBleu(cumulatedCounts, currInterval);
        if (d==0 || currInterval.score > bestInterval.score) {
            bestInterval = currInterval;
            bestDirection = d;
        }
    }

    // Print great results
    for (size_t i = 0; i < nDimensions; i++) {
        if (i>0) {
            cout << " ";
        }
        cout << directions[bestDirection][i];
    }
    cout << endl << "FINAL BestInterval [" << bestInterval.left << " - " << bestInterval.right << "] score: " << bestInterval.score << endl;
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
