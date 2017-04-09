//
// Created by bernardoct on 4/8/17.
//

#ifndef TRIANGLEMODEL_DIRECTEDGRAPH_H
#define TRIANGLEMODEL_DIRECTEDGRAPH_H


#include "Graph.h"

class UtilitiesGraph : Graph {
private:
    vector<map<int, vector<double>>>* pipe_info;
    map<int, int> map_uv_ids;
public:
    const int source_id;

    UtilitiesGraph(int V, const int source_id);

    void addEdge(int u, int v, double capacity, int owning_utility_id, double conveyance_price_per_volume);


    void getContinuityMatrix();
};


#endif //TRIANGLEMODEL_DIRECTEDGRAPH_H
