//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_REGION_H
#define TRIANGLEMODEL_REGION_H

#include "WaterSources/Reservoir.h"
#include "Constants.h"
#include "Utility.h"
#include <vector>

using namespace Constants;
using namespace std;

class ContinuityModel {
public:
    ContinuityModel(int realization_index,
                    vector<Reservoir> water_source,
                    vector<vector<int> > &reservoir_connectivity_matrix,
                    vector<Utility> utilities,
                    vector<vector<int> > &reservoir_utility_connectivity_matrix,
                    const int total_simulation_time);

    const int total_simulation_time;
    const int realization_index;

    void runSimpleContinuitySimulation(int total_simulation_time);

private:
    vector<Reservoir> water_sources;
    vector<Utility> utilities;
    vector<vector<int> > water_sources_adjacency_list;
    vector<vector<int> > water_sources_utility_adjacency_list;

    void continuityStep(int i);

};


#endif //TRIANGLEMODEL_REGION_H
