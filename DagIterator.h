# dagiterator
#include "types.h"

class DagIterator {
    public:
        Vertex getVertex();
        bool next();
        DagIterator(Vertex);
        
    private:
        vector<Vertex> pendingVertices;
