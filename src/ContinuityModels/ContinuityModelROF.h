//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "ContinuityModel.h"
#include "ContinuityModelRealization.h"

class ContinuityModelROF : public ContinuityModel {

protected:

    const int rof_type;
    const int realization_id;
    vector<WaterSource *> water_sources_realization;
public:
    ContinuityModelROF(const vector<WaterSource *> &water_source, const Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, const vector<Utility *> &utilities,
                       const int rof_type, const int realization_id);

    vector<double> calculateROF(int week);

    void resetUtilitiesAndReservoirs();

    void setWater_sources_realization(const vector<WaterSource *> &water_sources_realization);
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
