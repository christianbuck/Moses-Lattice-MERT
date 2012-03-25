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

#pragma once

#include <limits>
#include <vector>
#include <string>
#include <iostream>

using std::pair;
using std::vector;
using std::string;
using std::wstring;
using std::numeric_limits;
using std::ostream;

typedef wstring String;
typedef string Word;
typedef vector<Word> Phrase;
typedef double FeatureValue;
typedef vector<FeatureValue> FeatureVector;

typedef pair<double, vector<int> > boundary;

const size_t bleuOrder = 4;

struct Interval
{
  Interval()
  {
    score = -numeric_limits<double>::infinity();
    left = 0.0;
    right = 0.0;
  }
  double score;
  double left;
  double right;
};

struct Result
{
  double score;
  FeatureVector lambdas;
  Result(double s)
  {
    score = s;
  }
};

ostream & operator <<(ostream &os, const Phrase& p);

