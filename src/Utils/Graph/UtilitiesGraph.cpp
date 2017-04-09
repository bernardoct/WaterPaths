//
// Created by bernardoct on 4/8/17.
//

#include "UtilitiesGraph.h"

UtilitiesGraph::UtilitiesGraph(int V, const int source_id) : Graph(V), source_id(source_id) {
    pipe_info = new vector<map<int, vector<double>>>((unsigned long) V, *new map<int, vector<double>>);
}

void UtilitiesGraph::addEdge(int u, int v, double capacity, int owning_utility_id, double conveyance_price_per_volume) {
    /// Add edge to graph.
    addEdgeToEdgesList(u, v);
    //FIXME: MAP NODE TO VERTEX IDS TO COMPENSATE FOR TRANSFER AGREEMENTS THAT HAVE ONLY PART OF UTILITIES.

    /// Add pipe info to vector of maps of downstream and pipe properties.
    vector<double> pipe_uv_info = {capacity, owning_utility_id, conveyance_price_per_volume};
    pipe_info->at((unsigned long) u).insert(pair<int, vector<double>>(v, pipe_uv_info));
}

void UtilitiesGraph::getContinuityMatrix() {

}
