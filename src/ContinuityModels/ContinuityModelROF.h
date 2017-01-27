//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELROF_H
#define TRIANGLEMODEL_CONTINUITYMODELROF_H


#include "ContinuityModel.h"

class ContinuityModelROF : public ContinuityModel {

private:
    const int rof_type;

public:
    ContinuityModelROF(const vector<WaterSource *> &water_source,
                       vector<vector<int>> &water_source_connectivity_matrix, const vector<Utility *> &utilities,
                       vector<vector<int>> &water_source_utility_connectivity_matrix, const int rof_type);

    void calculateROF();
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
