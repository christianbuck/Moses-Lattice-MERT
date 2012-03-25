#!/bin/bash

LAMBDAS="-0.01 1 0.02 0.037222 0.01 0.21 0.05 0.131572 0.15 0.08 0.0615917 0.05 0.06 0.16 0.01"

time ./LatticeMERT -m 20 -l "$LAMBDAS" -r case4.ref case4.osg

