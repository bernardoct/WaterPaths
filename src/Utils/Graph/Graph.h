//
// Created by bernardo on 2/2/17.
//

#ifndef TRIANGLEMODEL_GRAPH_H
#define TRIANGLEMODEL_GRAPH_H

#include <list>
#include <vector>
#include "../Constants.h"

using namespace std;

class Graph {
protected:

    int V;    // Number of water sources'
    int n_edges = 0;
    // Pointer to an array containing adjacency lists
    list<int> *adj;

    void addEdgeToEdgesList(int u, int v);
public:
    Graph(int V);   // Constructor
};


#endif //TRIANGLEMODEL_GRAPH_H
