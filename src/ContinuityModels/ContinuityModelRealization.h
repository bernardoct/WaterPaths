//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H
#define TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H


#include "ContinuityModel.h"
#include "ContinuityModelROF.h"
#include "../DroughtMitigationInstruments/Restrictions.h"

class ContinuityModelRealization : public ContinuityModel {
private:
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;

public:
    ContinuityModelRealization(const vector<WaterSource *> &water_source,
                               const Graph &water_sources_graph,
                               const vector<vector<int>> &water_sources_to_utilities,
                               const vector<Utility *> &utilities,
                               vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                               const int realization_index);

    const int realization_id;

    vector<WaterSource *> getWater_sources();

    void setRisks_of_failure(const vector<double> &risks_of_failure);

    void applyRestrictionsAndTransfers(int week);

    const vector<DroughtMitigationPolicy *> getDrought_mitigation_policies() const;
};


#endif //TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H
