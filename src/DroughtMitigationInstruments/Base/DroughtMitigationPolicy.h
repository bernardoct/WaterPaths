//
// Created by bernardo on 2/6/17.
//

#ifndef TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
#define TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H

#include "../../SystemComponents/Utility/Utility.h"
#include "../../Utils/Constants.h"
#include "../../Utils/Graph/Graph.h"
#include "../../Utils/Matrices.h"
#include "../../Controls/Base/MinEnvFlowControl.h"

class DroughtMitigationPolicy {
protected:
    DroughtMitigationPolicy(const DroughtMitigationPolicy &drought_mitigation_policy);

    vector<int> utilities_ids;
    vector<Utility *> realization_utilities;
    vector<WaterSource *> realization_water_sources;
    vector<MinEnvFlowControl *> realization_min_env_flow_controls;
    vector<Matrix2D<double>> storage_to_rof_table_;
    vector<vector<double>> *rdm_factors_all;
    double *rdm_factors_realization;

public:
    const int id;
    const int type;

    DroughtMitigationPolicy(const int id, const int type);

    virtual void applyPolicy(int week)= 0;

    virtual void addSystemComponents(vector<Utility *> utilities,
                                         vector<WaterSource *> water_sources,
                                         vector<MinEnvFlowControl *> min_env_flow_controls)= 0;

    const vector<int> &getUtilities_ids() const;

    bool operator<(const DroughtMitigationPolicy *other);

    bool operator>(const DroughtMitigationPolicy *other);

    virtual ~DroughtMitigationPolicy();

    void setStorage_to_rof_table_(const vector<Matrix2D<double>> &storage_to_rof_table_);

    virtual void setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                                vector<double> &water_sources_rdm, vector<double> &policy_rdm)= 0;

};


#endif //TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
