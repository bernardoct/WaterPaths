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
    vector<vector<int>> upstream_sources;
    vector<int> topological_order;
    vector<vector<double>> continuity_matrix_transpose;

    void addEdgeToEdgesList(int u, int v);

    // returns and prints a Topological Sort of the complete graph
    vector<int> topologicalSort();

    vector<int> findUpstreamSources(int id)const;
public:
    Graph(int V);   // Constructor

    const vector<int> getTopological_order() const;

    // function to add an edge to graph
    void addEdge(int u, int v);

    vector<int> &getUpstream_sources(int i);


    const vector<vector<double>> getContinuityMatrix() const;
};


#endif //TRIANGLEMODEL_GRAPH_H
