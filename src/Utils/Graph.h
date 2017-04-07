//
// Created by bernardo on 2/2/17.
//

#ifndef TRIANGLEMODEL_GRAPH_H
#define TRIANGLEMODEL_GRAPH_H

#include <list>
#include <vector>

using namespace std;

class Graph {
private:
    int V;    // Number of water sources'

    // Pointer to an array containing adjacency lists
    list<int> *adj;
    vector<vector<int>> edges;
    vector<vector<int>> upstream_sources;
    vector<int> topologial_order;
public:
    const vector<int> &getTopologial_order() const;

public:
    Graph(int V);   // Constructor

    // function to add an edge to graph
    void addEdge(int u, int v);

    // returns and prints a Topological Sort of the complete graph
    vector<int> topologicalSort();

    vector<int> findUpstreamSources(int id)const;

    vector<vector<int>> getTriangularAdjacencyMatrix(int index);

    const vector<vector<int>> &getEdges() const;

    vector<int> &getUpstream_sourses(int i);
};


#endif //TRIANGLEMODEL_GRAPH_H
