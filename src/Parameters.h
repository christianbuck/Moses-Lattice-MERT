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

#include "Types.h"

class Parameters
{
public:
  const char *inputPath;
  const char *referencePath;
  //const char *lambda;
  FeatureVector lambdas;
  int verbose;
  size_t maxIters;
  double eps;
  size_t randomVectorCount;

  Parameters() :
      inputPath("-"), referencePath(0), verbose(0),
      maxIters(1), eps(0.0001), randomVectorCount(0)
  {
  }

  void Parse(int argc, char **argv);

  void ParseLambdas(const char *str);
private:
};

