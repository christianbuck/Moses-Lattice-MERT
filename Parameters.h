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
  size_t randomVectorCount;
  double eps;

  Parameters() :
      inputPath("-"), referencePath(0), verbose(0), maxIters(1), eps(0.0001), randomVectorCount(
          0)
  {
  }

  void parse(int argc, char **argv);

  void parseLambdas(const char *str);
private:
};

