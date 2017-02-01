//
// Created by bernardo on 1/25/17.
//

#ifndef TRIANGLEMODEL_SIMULATION_H
#define TRIANGLEMODEL_SIMULATION_H

#include "SystemComponents/WaterSources/WaterSource.h"
#include "Utils/Constants.h"
#include "SystemComponents/Utility.h"
#include "ContinuityModels/ContinuityModel.h"
#include "ContinuityModels/ContinuityModelRealization.h"
#include "DataColector.h"
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
    vector<ContinuityModelRealization *> realization_models;
    vector<ContinuityModelROF *> rof_models;
    DataColector *data_colector;
};


#endif //TRIANGLEMODEL_SIMULATION_H
