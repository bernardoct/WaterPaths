//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "Base/ContinuityModel.h"
#include "../Utils/Matrices.h"


class ContinuityModelROF : public ContinuityModel {
private:
    Matrix3D<double> *storage_to_rof_table;
    Matrix3D<double> storage_to_rof_realization;
    vector<int> online_downstream_sources;
    bool *storage_wout_downstream;
    const int n_topo_sources;

protected:
    int beginning_tier = 0;
    vector<WaterSource *> realization_water_sources;

public:
    ContinuityModelROF(
            vector<WaterSource *> water_sources,
            const Graph &water_sources_graph,
            const vector<vector<int>> &water_sources_to_utilities,
            vector<Utility *> utilities,
            vector<MinEnvironFlowControl *> &min_env_flow_controls,
            vector<vector<double>> *utilities_rdm,
            vector<vector<double>> *water_sources_rdm,
            unsigned int realization_id);

    ContinuityModelROF(ContinuityModelROF &continuity_model_rof);

    vector<double> calculateShortTermROF(int week);

    vector<double> calculateLongTermROF(int week);

    void resetUtilitiesAndReservoirs(int rof_type);

    void connectRealizationWaterSources(const vector<WaterSource *> &water_sources_realization);

    void updateOnlineInfrastructure(int week);

    virtual ~ContinuityModelROF();

    void updateStorageToROFTable(double storage_percent_decrement,
                                 int week_of_the_year,
                                 const double *to_full_toposort);

    const Matrix3D<double> *getStorage_to_rof_table() const;

    void shiftStorages(double *available_volumes_shifted, const double
    *delta_storage);

    void printROFTable(int week, const string &folder);
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
