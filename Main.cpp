#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <list>

#include "MosesGraphReader.h"
#include "Lattice.h"
#include "Types.h"

using std::ifstream;
using std::string;
using std::vector;
using std::list;
using std::cout;
using std::endl;
using std::ostream;

void test1(void)
{
    vector<double> lambdas;
    vector<double> dir;
    for (size_t i = 0; i < 15; i++) {
        lambdas.push_back(1.0);
        dir.push_back((i == 9) ? 1.0 : 0.0);
    }

    ifstream is("/home/karlos/Moses-Lattice-MERT/osg");
    MosesGraphReader reader(is);

    while (true) {
        // read lattice
        Lattice lattice;
        if (!reader.GetNextLattice(lattice)) break;

        // calculate upper envelope
        vector<Line> a;
        latticeEnvelope(lattice, dir, lambdas, a);

        // read reference
        Phrase reference;

        // gather scoring statistics
        vector<BleuStats> stats;
        //computeBleuStats(a, reference, stats);
    }
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

