//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "Base/ContinuityModel.h"
#include "../Utils/Matrices.h"


class ContinuityModelROF : public ContinuityModel {
private:
    Matrix3D<float> *storage_to_rof_table;
    Matrix3D<float> storage_to_rof_realization;
//    __declspec(aligned(64)) float *capacities_toposorted;
    float *capacities_toposorted __attribute__ ((aligned(64)));
    int *downstream_sources_toposort;
    int *topo_sorted_to_all_sources;
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
            unsigned int realization_id);

    ContinuityModelROF(ContinuityModelROF &continuity_model_rof);

    vector<float> calculateShortTermROF(int week);

    vector<float> calculateLongTermROF(int week);

    void resetUtilitiesAndReservoirs(int rof_type);

    void setRealization_water_sources(const vector<WaterSource *> &water_sources_realization);

    void updateOnlineInfrastructure(int week);

    virtual ~ContinuityModelROF();

    void updateStorageToROFTable(float storage_percent_decrement, int week_of_the_year,
                                 const float *to_full_toposort);

    void shiftStorages(float *storages, const float *deltas, const float *spillage);

    const Matrix3D<float> *getStorage_to_rof_table() const;
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
