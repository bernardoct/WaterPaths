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
    vector<Utility *> utilities_realization;
public:
    ContinuityModelROF(const vector<WaterSource *> &water_source,
                       const vector<vector<int>> &water_source_connectivity_matrix, const vector<Utility *> &utilities,
                       const vector<vector<int>> &water_source_utility_connectivity_matrix, const int rof_type);

    void setUtilities_realization(const vector<Utility *> &utilities_realization);

    vector<double> calculateROF(int week);

    void resetUtilitiesAndReservoirs();
};


#endif //TRIANGLEMODEL_CONTINUITYMODELROF_H
