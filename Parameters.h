#pragma once

#include "Types.h"

class Parameters
{
public:
    const char *inputPath;
    const char *referencePath;
    //const char *lambda;
    FeatureVector lambdas;
    int  verbose;

    Parameters()
        : inputPath("-"), referencePath(0),
            verbose(0)
    {}

    void parse(int argc, char **argv);

    void parseLambdas(const char *str);
private:
};


