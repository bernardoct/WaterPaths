//
// Created by bernardo on 1/25/17.
//

#ifndef TRIANGLEMODEL_SIMULATION_H
#define TRIANGLEMODEL_SIMULATION_H

#include "WaterSources/WaterSource.h"
#include "Constants.h"
#include "Utility.h"
#include "ContinuityModels/ContinuityModel.h"
#include "ContinuityModels/ContinuityModelRealization.h"
#include <vector>

using namespace Constants;
using namespace std;

class Simulation {
public:
    Simulation(const vector<WaterSource *> &water_sources, const vector<vector<int>> &water_sources_adjacency_matrix,
               const vector<Utility *> &utilities, const vector<vector<int>> &water_sources_utility_adjacency_matrix,
               const int total_simulation_time, const int number_of_realizations);

    const int total_simulation_time;
    const int number_of_realizations;

    void runFullSimulation();

private:
    vector<WaterSource *> water_sources;
    vector<Utility *> utilities;
    vector<ContinuityModelRealization *> realization_models;

    vector<double> calculateRisksOfFailure(ContinuityModel realization_regional_model);

};


#endif //TRIANGLEMODEL_SIMULATION_H
