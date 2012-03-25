#pragma once 
// dagiterator
#include "Types.h"

class DagIterator
{
public:
  Vertex getVertex();
  bool next();
  DagIterator(Vertex);

private:
  vector<Vertex> pendingVertices;
};

