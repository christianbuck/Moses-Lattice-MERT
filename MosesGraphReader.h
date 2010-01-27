#pragma once

#include <iostream>
#include <boost/regex.hpp>

#include "Lattice.h"

class MosesGraphReader {
public:
    MosesGraphReader(std::istream &is);

    bool GetNextLattice(Lattice &lattice);

private:
    std::istream &is;

    static boost::regex rx_line0, rx_line1, rx_line2, rx_line3, rx_line4;
};
