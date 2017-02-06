//
// Created by bernardo on 2/2/17.
//

#ifndef TRIANGLEMODEL_GRAPH_H
#define TRIANGLEMODEL_GRAPH_H

#include <list>
#include <vector>

using namespace std;

class Graph {
    int V;    // Number of water sources'

    // Pointer to an array containing adjacency lists
    list<int> *adj;

public:
    Graph(int V);   // Constructor

    // function to add an edge to graph
    void addEdge(int u, int v);

    // returns and prints a Topological Sort of the complete graph
    vector<int> topologicalSort() const;

    vector<int> getUpstreamSources(int id);
};


#endif //TRIANGLEMODEL_GRAPH_H
