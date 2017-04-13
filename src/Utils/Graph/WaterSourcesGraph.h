//
// Created by bernardoct on 4/8/17.
//

#ifndef TRIANGLEMODEL_UNDIRECTEDGRAPH_H
#define TRIANGLEMODEL_UNDIRECTEDGRAPH_H


#include "Graph.h"

//FIXME: UTILITIES GRAPH THAT RETURN CONTINUITY MATRIX AND PIPE PROPERTIES. TO BE IMPLEMENTED.

class WaterSourceGraph : Graph {
private:
    vector<vector<int>> upstream_sources;
    vector<int> topological_order;
    vector<vector<double>> continuity_matrix_transpose;

    // returns and prints a Topological Sort of the complete graph
    vector<int> topologicalSort();

    vector<int> findUpstreamSources(int id)const;

public:
    WaterSourceGraph(int V);
    const vector<int> &getTopological_order() const;

    // function to add an edge to graph
    void addEdge(int u, int v);

    vector<int> &getUpstream_sources(int i);


    const vector<vector<double>> getContinuityMatrix() const;
};


#endif //TRIANGLEMODEL_UNDIRECTEDGRAPH_H
