//
// Created by bernardo on 1/25/17.
//

#ifndef TRIANGLEMODEL_SIMULATION_H
#define TRIANGLEMODEL_SIMULATION_H

#include "../Utils/Constants.h"
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../SystemComponents/Utility.h"
#include "../DroughtMitigationInstruments/Restrictions.h"
#include "../ContinuityModels/Base/ContinuityModel.h"
#include "../ContinuityModels/ContinuityModelRealization.h"
#include "../ContinuityModels/ContinuityModelROF.h"
#include "../Controls/Base/MinEnvironFlowControl.h"
#include "../DataCollector/MasterDataCollector.h"
#include "../Controls/AllocationModifier.h"
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
            vector<vector<double>> *utilities_rdm,
            vector<vector<double>> *water_sources_rdm,
            vector<vector<double>> *policies_rdm,
            unsigned long total_simulation_time, unsigned long number_of_realizations);

    Simulation &operator=(const Simulation &simulation);

    virtual ~Simulation();

    MasterDataCollector *runFullSimulation();

private:

    unsigned long total_simulation_time;
    unsigned long number_of_realizations;
    vector<ContinuityModelRealization *> realization_models;
    vector<ContinuityModelROF *> rof_models;
    MasterDataCollector *master_data_collector;

};


#endif //TRIANGLEMODEL_SIMULATION_H
