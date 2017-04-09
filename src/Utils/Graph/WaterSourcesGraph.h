//
// Created by bernardoct on 4/8/17.
//

#ifndef TRIANGLEMODEL_UNDIRECTEDGRAPH_H
#define TRIANGLEMODEL_UNDIRECTEDGRAPH_H


#include "Graph.h"

//FIXME: UTILITIES GRAPH THAT RETURN CONTINUITY MATRIX AND PIPE PROPERTIES. TO BE IMPLEMENTED.

class WaterSourceGraph : Graph {
public:
    WaterSourceGraph(int V);
    vector<vector<int>> upstream_sources;
    vector<int> topological_order;

    const vector<int> &getTopologial_order() const;

    // function to add an edge to graph
    void addEdge(int u, int v);

    // returns and prints a Topological Sort of the complete graph
    vector<int> topologicalSort();

    vector<int> findUpstreamSources(int id)const;

    vector<int> &getUpstream_sourses(int i);


};


#endif //TRIANGLEMODEL_UNDIRECTEDGRAPH_H
