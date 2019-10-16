//
// Created by bernardo on 1/26/17.
//

#ifndef TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H
#define TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H


#include "Base/ContinuityModel.h"
#include "../DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h"

class ContinuityModelRealization : public ContinuityModel {
private:
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;

public:
    ContinuityModelRealization(
            vector<WaterSource *> &water_sources,
            const Graph &water_sources_graph,
            const vector<vector<int>> &water_sources_to_utilities,
            vector<Utility *> &utilities,
            const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
            vector<MinEnvFlowControl *> &min_env_flow_control,
            vector<double>& utilities_rdm,
            vector<double>& water_sources_rdm,
            vector<double>& policy_rdm,
            const unsigned int realization_index);

    ~ContinuityModelRealization() override;

    void setShortTermROFs(const vector<double> &risks_of_failure);

    void applyDroughtMitigationPolicies(int week);

    const vector<DroughtMitigationPolicy *> getDrought_mitigation_policies() const;

    void setLongTermROFs(const vector<vector<double>> &risks_of_failure, const int week);


    void setLongTermROFDemandProjectionEstimate(const vector<Utility *> &rof_utilities);
};


#endif //TRIANGLEMODEL_CONTINUITYMODELREALIZATION_H
