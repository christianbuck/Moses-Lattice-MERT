#pragma once

#include "Types.h"

class Parameters
{
public:
    const char *inputPath;
    //const char *lambda;
    FeatureVector lambdas;
    int  verbose;

    Parameters()
        : inputPath("-"),
            verbose(0)
    {}

    void parse(int argc, char **argv);

    void parseLambdas(const char *str);
private:
};


