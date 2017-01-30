//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H
#define TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H


#include "ContinuityModel.h"
#include "ContinuityModelROF.h"

class ContinuityModelRealization : public ContinuityModel {
public:
    const int realization_id;

    ContinuityModelRealization(const vector<WaterSource *> &water_source,
                               const vector<vector<int>> &water_source_connectivity_matrix,
                               const vector<Utility *> &utilities,
                               const vector<vector<int>> &water_source_utility_connectivity_matrix,
                               const int realization_index);

};


#endif //TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H
