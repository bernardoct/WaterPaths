//
// Created by bernardo on 1/25/17.
//

#ifndef TRIANGLEMODEL_SIMULATION_H
#define TRIANGLEMODEL_SIMULATION_H

#include "../SystemComponents/WaterSources/WaterSource.h"
#include "../Utils/Constants.h"
#include "../SystemComponents/Utility.h"
#include "../ContinuityModels/ContinuityModel.h"
#include "../ContinuityModels/ContinuityModelRealization.h"
#include "../Utils/DataCollector.h"
#include "../DroughtMitigationInstruments/Restrictions.h"
#include <vector>

using namespace Constants;
using namespace std;

class Simulation {
public:

    Simulation(vector<WaterSource *> &water_sources, Graph &water_sources_graph,
               const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> &utilities,
               vector<DroughtMitigationPolicy *> &drought_mitigation_policies, const int total_simulation_time,
               const int number_of_realizations, DataCollector *data_collector);

    const int total_simulation_time;
    const int number_of_realizations;

    void runFullSimulation();
    
    static double calculateReliabilityObjective(Utility_t utility_data);

private:
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    vector<ContinuityModelRealization *> realization_models;
    vector<ContinuityModelROF *> rof_models;
    DataCollector * data_collector;

    bool isFirstWeekOfTheYear(int week);

//    static double calculateReliabilityObjective(vector<Utility_t> utilities_data);

    vector<double> calculateObjectives();
};


#endif //TRIANGLEMODEL_SIMULATION_H
