//
// Created by bernardo on 1/25/17.
//

#ifndef TRIANGLEMODEL_SIMULATION_H
#define TRIANGLEMODEL_SIMULATION_H

#include "../Utils/Constants.h"
#include "../Utils/DataCollector.h"
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../SystemComponents/Utility.h"
#include "../DroughtMitigationInstruments/Restrictions.h"
#include "../ContinuityModels/Base/ContinuityModel.h"
#include "../ContinuityModels/ContinuityModelRealization.h"
#include "../ContinuityModels/ContinuityModelROF.h"
#include "../Controls/Base/MinEnvironFlowControl.h"
#include <vector>

using namespace Constants;
using namespace std;

class Simulation {
public:

    Simulation(
            vector<WaterSource *> &water_sources, Graph &water_sources_graph,
            const vector<vector<int>> &water_sources_to_utilities,
            vector<Utility *> &utilities,
            const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
            vector<MinEnvironFlowControl *> &min_env_flow_controls,
            const int total_simulation_time, const int number_of_realizations);

    Simulation &operator=(const Simulation &simulation);

    DataCollector *runFullSimulation(int num_threads = 2);

    virtual ~Simulation();

private:

    int total_simulation_time;
    int number_of_realizations;
    vector<ContinuityModelRealization *> realization_models;
    vector<ContinuityModelROF *> rof_models;
    DataCollector *data_collector;

};


#endif //TRIANGLEMODEL_SIMULATION_H
