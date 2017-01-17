//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_REGION_H
#define TRIANGLEMODEL_REGION_H

#include "Reservoir.h"
#include "Constants.h"
#include "Utility.h"
#include <vector>

using namespace Constants;
using namespace std;

class Region {
public:
    Region(int realization_index,
           vector<Reservoir> reservoirs,
           vector<vector<int> > reservoir_connectivity_matrix_,
           vector<Utility> utilities,
           vector<vector<int> > reservoir_utility_connectivity_matrix,
           const int total_simulation_time);

    const int total_simulation_time;
    const int realization_index;

    void runSimpleContinuitySimulation(int total_simulation_time);

private:
    vector<Reservoir> reservoirs;
    vector<Utility> utilities;
    vector<vector<int> > reservoir_adjacency_list;
    vector<vector<int> > reservoir_utility_adjacency_list;
    vector<vector<int> > reservoir_adjacency_matrix; // 1 for receive from, -1 to release to, and 0 for no connection.
    vector<vector<int> > reservoir_utility_connectivity_matrix; // Reservoir in rows and utilities in columns.

    void continuityStep(int i);

};


#endif //TRIANGLEMODEL_REGION_H
