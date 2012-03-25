/*
 * LatticeMERT
 * Copyright (C)  2010-2012 
 *   Christian Buck
 *   Kārlis Goba <karlis.goba@gmail.com> 
 * 
 * LatticeMERT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LatticeMERT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <list>
#include <cmath>
#include <assert.h>

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
  std::istringstream iss(line);
  std::copy(std::istream_iterator<Word>(iss), std::istream_iterator<Word>(),
      std::back_inserter<Phrase>(reference));
  for (size_t i = 0; i < reference.size(); i++)
  {
    assert(reference[i].length() > 0);
  }
}

double randomDouble(const double low, const double high)
{
  return (rand() / (static_cast<double>(RAND_MAX) + 1.0)) * (high - low) + low;
}

void generateRandomVector(vector<double> &vector)
{
  for (size_t i = 0; i < vector.size(); i++)
  {
    vector[i] = randomDouble(0.0, 1.0);
  }
}

void getDirectionsAxes(vector<vector<double> >& directions, const size_t dim, const size_t n) {
  directions.resize(n);
  for (size_t d = 0; d < n; d++)
  {
    directions[d].clear();
    for (size_t i = 0; i < dim; i++)
    {
      directions[d].push_back((i == d) ? 1.0 : 0.0);
    }
  }
}

void getDirectionsRandom(vector<vector<double> >& directions, const size_t dim, const size_t n) {
  directions.resize(n);
  for (size_t d = 0; d < n; d++)
  {
    directions[d].resize(dim);
    generateRandomVector(directions[d]);
  }
}


Result doIteration(const Parameters &params)
{
  size_t nDimensions = params.lambdas.size();
  size_t nDirections = params.randomVectorCount; // might be higher of lower in case of random directions
  vector<vector<double> > directions;

  if (nDirections == 0)
  {
    nDirections = nDimensions;
    getDirectionsAxes(directions, nDimensions, nDirections);
  }
  else
  {
    getDirectionsRandom(directions, nDimensions, nDirections);
  }

  ifstream is_ref(params.referencePath);
  ifstream is_osg(params.inputPath);
  MosesGraphReader reader(is_osg);

  vector<vector<boundary> > differenceVectors(directions.size());

  cout << endl;
  size_t refLength = 0;
  while (true)
  {
    Lattice lattice;
    if (!reader.GetNextLattice(lattice))
    {
      break;
    }
    Phrase reference;
    readReference(is_ref, reference);
    refLength += reference.size();
    // cout << "Reference: [" << reference << "]" << endl;
    cout << "#";
    cout.flush();

    #pragma omp parallel for shared(directions, params, reference) firstprivate(lattice)
    for (size_t d = 0; d < nDirections; d++)
    {
      const FeatureVector& dir = directions[d];
      vector<Line> envelope;
      LatticeEnvelope(lattice, dir, params.lambdas, envelope);

      vector<BleuStats> stats;
      ComputeBleuStats(lattice, envelope, reference, stats);
      #pragma omp critical
      {

        vector<boundary>& cumulatedCounts = differenceVectors[d];
        AccumulateBleu(stats, cumulatedCounts);
      }
    }
  }
  cout << endl;

  Interval bestInterval;
  size_t bestDirection = 0;
  for (size_t d = 0; d < nDirections; d++)
  {
    vector<boundary> &cumulatedCounts = differenceVectors[d];
    Interval currInterval;
    OptimizeBleu(cumulatedCounts, currInterval, refLength);
    cout << "BestInterval [" << currInterval.left << " - " << currInterval.right
        << "] score: " << currInterval.score << endl;
    if (d == 0 || currInterval.score > bestInterval.score)
    {
      bestInterval = currInterval;
      bestDirection = d;
    }
  }

  // Print great results
  for (size_t i = 0; i < nDimensions; i++)
  {
    if (i > 0)
    {
      cout << " ";
    }
    cout << directions[bestDirection][i];
  }
  cout << endl << "FINAL BestInterval [" << bestInterval.left << " - "
      << bestInterval.right << "] score: " << bestInterval.score << endl;
  Result result(bestInterval.score);
  double bestVal;
  if (bestInterval.right == numeric_limits<double>::infinity()
      && bestInterval.left == -numeric_limits<double>::infinity())
  {
    bestVal = 0.0;
  }
  else
  {
    if (bestInterval.left == -numeric_limits<double>::infinity())
    {
      bestVal = bestInterval.right - params.eps;
    }
    else if (bestInterval.right == numeric_limits<double>::infinity())
    {
      bestVal = bestInterval.left + params.eps;
    }
    else
    {
      bestVal = (bestInterval.right + bestInterval.left) / 2;
    }
  }
  // Normalise lambdas
  double norm1 = 0;
  FeatureVector bestLambdas(nDimensions);
  for (size_t i = 0; i < nDimensions; i++)
  {
    double newValue = params.lambdas[i]
        + directions[bestDirection][i] * bestVal;
    bestLambdas[i] = newValue;
    norm1 += fabs(newValue);
  }
  if (norm1 < 1E-6)
    norm1 = 1E-6;
  for (size_t i = 0; i < nDimensions; i++)
  {
    bestLambdas[i] /= norm1;
  }
  result.lambdas = bestLambdas;
  return result;
}

void printParams(const Parameters &params)
{
  cout << "Parameters:" << endl;
  cout << "  Input path: " << params.inputPath << endl;
  cout << "  Reference path: " << params.referencePath << endl;
  cout << "  Random vector count: " << params.randomVectorCount << endl;
  cout << "  Lambda: ";
  for (size_t i = 0; i < params.lambdas.size(); i++)
    cout << params.lambdas[i] << " ";
  cout << endl;
}

void updateParameters(Parameters &params, FeatureVector &newLambdas)
{
  for (size_t i = 0; i < newLambdas.size(); i++)
  {
    params.lambdas[i] = newLambdas[i];
  }
}

int main(int argc, char **argv)
{
  Parameters params;

  params.Parse(argc, argv);

  printParams(params);

  double oldScore = 0.0;
  for (size_t iteration = 0; iteration < params.maxIters; iteration++)
  {
    cout << "Doing iteration " << (iteration + 1) << endl;
    Result res = doIteration(params);
    updateParameters(params, res.lambdas);

    cout << "Current point: ";
    for (size_t i = 0; i < params.lambdas.size(); i++)
      cout << params.lambdas[i] << " ";
    cout << " => " << res.score << endl;

    if (iteration > 0 && (res.score - oldScore) < params.eps)
    {
      cout << "Improvement too small (" << res.score - oldScore
          << ")- I will quit. " << endl;
      oldScore = res.score;
      break;
    }
    oldScore = res.score;
  }

  cout << "Best point: ";
  for (size_t i = 0; i < params.lambdas.size(); i++)
    cout << params.lambdas[i] << " ";
  cout << " => " << oldScore << endl;

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
