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
#include <vector>

using namespace Constants;
using namespace std;

class Simulation {
private:
    unsigned long total_simulation_time;
    unsigned long n_realizations;
    vector<ContinuityModelRealization *> realization_models;
    vector<ContinuityModelROF *> rof_models;
    MasterDataCollector* master_data_collector;
    vector<unsigned long> realizations_to_run;

    const int import_export_rof_tables;
    vector<Matrix3D<double>> precomputed_rof_tables;
    vector<vector<double>> table_storage_shift;

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
            int import_export_rof_tables,
            const unsigned long total_simulation_time,
            vector<unsigned long> &realizations_to_run);

    Simulation &operator=(const Simulation &simulation);

    virtual ~Simulation();

    MasterDataCollector* runFullSimulation(unsigned long n_threads);

    void setPrecomputed_rof_tables(const vector<Matrix3D<double>> &precomputed_rof_tables,
                                   vector<vector<double>> &table_storage_shift);

    void setRealizations_to_run(const vector<unsigned long> &realizations_to_run);


};


#endif //TRIANGLEMODEL_SIMULATION_H
