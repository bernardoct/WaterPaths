//
// Created by bernardoct on 5/1/17.
//

#ifndef TRIANGLEMODEL_INSURANCESTORAGETOROF_H
#define TRIANGLEMODEL_INSURANCESTORAGETOROF_H


#include "Base/DroughtMitigationPolicy.h"
#include "../ContinuityModels/ContinuityModelROF.h"

class InsuranceStorageToROF : public DroughtMitigationPolicy,
                              public ContinuityModelROF {
private:

    double *insurance_price;
    const double *rof_triggers;
    const double insurance_premium;
    const double *fixed_payouts;
    double *utilities_revenue_update;
    double *utilities_revenue_last_year;
    Matrix3D<double> storage_to_rof_table_prev_year;

public:

    InsuranceStorageToROF(
            const int id,
            vector<WaterSource *> &water_sources,
            const Graph &water_sources_graph,
            const vector<vector<int>> &water_sources_to_utilities,
            vector<Utility *> &utilities,
            vector<MinEnvironFlowControl *>
            min_env_flow_controls, double *rof_triggers,
            const double insurance_premium,
            const double *fixed_payouts);

    InsuranceStorageToROF(InsuranceStorageToROF &insurance);

    ~InsuranceStorageToROF() override;

    void priceInsurance(int week);

    void getUtilitiesApproxROFs(double *u_storage_capacity_ratio, const Matrix3D<double> *storage_to_rof_table,
                                int week, double *utilities_approx_rof);

    double *UtilitiesStorageCapacityRatio();

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) override;

    void setRealization(unsigned int realization_id) override;

};


#endif //TRIANGLEMODEL_INSURANCESTORAGETOROF_H
