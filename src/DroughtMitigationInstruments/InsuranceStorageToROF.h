//
// Created by bernardoct on 5/1/17.
//

#ifndef TRIANGLEMODEL_INSURANCESTORAGETOROF_H
#define TRIANGLEMODEL_INSURANCESTORAGETOROF_H


#include "DroughtMitigationPolicy.h"
#include "../ContinuityModels/ContinuityModelROF.h"

class InsuranceStorageToROF : public DroughtMitigationPolicy, public ContinuityModelROF {
private:

    double *insurance_price;

public:
    InsuranceStorageToROF(const int id, const vector<WaterSource *> &water_sources, const Graph &water_sources_graph,
                          const vector<vector<int>> &water_sources_to_utilities, const vector<Utility *> &utilities,
                          vector<int> insured_utilities_ids, double *rof_triggers, const double insurance_premium,
                          const double *fixed_payouts);

    InsuranceStorageToROF(const InsuranceStorageToROF &insurance);

    virtual ~InsuranceStorageToROF();

//    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) override;
//
//    void applyPolicy(int week) override;

    const double *rof_triggers;
    const double insurance_premium;
    const double *fixed_payouts;

    void priceInsurance(int week);

    void getUtilitiesApproxROFs(double *u_storage_capacity_ratio, const Matrix3D<double> *storage_to_rof_table,
                                int week, double *utilities_approx_rof);

    double *UtilitiesStorageCapacityRatio();

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) override;


};


#endif //TRIANGLEMODEL_INSURANCESTORAGETOROF_H
