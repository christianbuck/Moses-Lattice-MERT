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

Result doIteration(const Parameters &params)
{
	size_t nDimensions = params.lambdas.size();
	size_t nDirections = nDimensions; // might be higher of lower in case of random directions
//	nDirections = 13;
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

    cout << endl;
    while (true) {
        Lattice lattice;
        if (!reader.GetNextLattice(lattice)) break;

        Phrase reference;
        readReference(is_ref, reference);
        // cout << "Reference: [" << reference << "]" << endl;
	    cout << "#";
	    cout.flush();

		for (size_t d=0;d<nDirections;d++) {
            vector<boundary> &cumulatedCounts = differenceVectors[d];
			FeatureVector &dir = directions[d];
		    vector<Line> envelope;
		    latticeEnvelope(lattice, dir, params.lambdas, envelope);

		    vector<BleuStats> stats;
		    computeBleuStats(lattice, envelope, reference, stats);
		    accumulateBleu(stats, cumulatedCounts);
		}
    }
    cout << endl;
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
    Result result(bestInterval.score);
    double bestVal;
    if (bestInterval.right == numeric_limits<double>::infinity() && bestInterval.left == -numeric_limits<double>::infinity()) {
        bestVal = 0.0;
    } else {
        if (bestInterval.left == -numeric_limits<double>::infinity()) {
            bestVal = bestInterval.right - params.eps;
        } else if (bestInterval.right == numeric_limits<double>::infinity()) {
            bestVal = bestInterval.left + params.eps;
        } else  {
            bestVal = (bestInterval.right+bestInterval.left)/2;
        }
    }
    for (size_t i=0;i<nDimensions;i++) {
        result.lambdas.push_back(params.lambdas[i] + directions[bestDirection][i] * bestVal);
    }
    return result;
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

void updateParameters(Parameters &params, FeatureVector &newLambdas)
{
    for (size_t i=0; i<newLambdas.size();i++) {
        params.lambdas[i] = newLambdas[i];
    }
}

int main(int argc, char **argv)
{
    Parameters params;

    params.parse(argc, argv);

    printParams(params);

    double oldScore=0.0;
    for (size_t iteration=0; iteration<params.maxIters; iteration++) {
        Result res = doIteration(params);
        updateParameters(params, res.lambdas);
        printParams(params);
        if (iteration>0 && (res.score - oldScore) < params.eps) {
            cout << "bleu improvement too small (" << res.score-oldScore << ")- i will quit. " << endl;
            break;
        }
        oldScore = res.score;
    }

    cout << "NEW LAMBDAS: ";
    for (size_t i = 0; i < params.lambdas.size(); i++)
        cout << params.lambdas[i] << " ";
    cout << endl;

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
