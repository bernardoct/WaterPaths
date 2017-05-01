//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "ContinuityModel.h"
#include "ContinuityModelRealization.h"

class ContinuityModelROF : public ContinuityModel {

protected:
    ContinuityModelROF(ContinuityModelROF &continuity_model_rof);

    const int realization_id;
    vector<WaterSource *> realization_water_sources;
    vector<vector<double>> *storage_to_rof_table;
    vector<int> downstream_sources;
    vector<int> sources_topological_order;
    vector<double> capacities;

public:
    ContinuityModelROF(const vector<WaterSource *> &water_sources, const Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, const vector<Utility *> &utilities,
                       const int realization_id);

    vector<double> calculateROF(int week, int rof_type);

    void resetUtilitiesAndReservoirs(int rof_type);

    void setRealization_water_sources(const vector<WaterSource *> &water_sources_realization);

    void updateOnlineInfrastructure();

    virtual ~ContinuityModelROF();

    void updateStorageToROFTable(double storage_percent_increment);
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
