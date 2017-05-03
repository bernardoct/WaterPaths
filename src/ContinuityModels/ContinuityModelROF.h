//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "ContinuityModel.h"
#include "ContinuityModelRealization.h"
#include "../Utils/Matrix3D.h"


class ContinuityModelROF : public ContinuityModel {

protected:
    ContinuityModelROF(ContinuityModelROF &continuity_model_rof);

    const int realization_id;
    vector<WaterSource *> realization_water_sources;
    Matrix3D<double> storage_to_rof_realization;
    Matrix3D<double> storage_to_rof_table;
    int beginning_res_level;

public:
    ContinuityModelROF(const vector<WaterSource *> &water_sources, const Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, const vector<Utility *> &utilities,
                       const int realization_id);

    vector<double> calculateROF(int week, int rof_type);

    void resetUtilitiesAndReservoirs(int rof_type);

    void setRealization_water_sources(const vector<WaterSource *> &water_sources_realization);

    void updateOnlineInfrastructure(int week);

    virtual ~ContinuityModelROF();

    void updateStorageToROFTable(double storage_percent_decrement, int week_of_the_year);

    void shiftStorages(double *storages, double *deltas);
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
