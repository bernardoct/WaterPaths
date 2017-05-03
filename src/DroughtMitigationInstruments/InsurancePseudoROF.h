//
// Created by bernardoct on 4/30/17.
//

#ifndef TRIANGLEMODEL_INSURANCE_H
#define TRIANGLEMODEL_INSURANCE_H


#include "DroughtMitigationPolicy.h"
#include "../ContinuityModels/ContinuityModelROF.h"

class InsurancePseudoROF : public DroughtMitigationPolicy, ContinuityModelROF {
protected:
    double *insurance_prices;
    vector<int> ids_of_utilities_with_policies;
    const vector<double> rof_triggers;
    const double insurance_premium;
    const vector<double> fixed_payouts;
    vector<int> downstream_sources;
    vector<int> sources_topological_order;
    vector<double> capacities;

public:
    InsurancePseudoROF(const int id, const vector<double> &rof_triggers, const double insurance_premium,
                       const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
                       const Graph &water_sources_graph, const vector<vector<int>> &water_sources_to_utilities,
                       vector<double> fixed_payouts);

    InsurancePseudoROF(const InsurancePseudoROF &insurance);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) override;

    void priceInsurance(int week);

    const double getInsurancePrice(int u) const;

    virtual ~InsurancePseudoROF();

    void calculateRealizationsStorages(Matrix3D<double> *realizations_storages, int week);

    void calculatePseudoRofs(Matrix3D<double> *realizations_storages, Matrix3D<double> *realizations_rofs, int r);

    Matrix2D<double>
    shiftStorageCurves(Matrix3D<double> *storage_curves_2yrs, double *storage0, int first_week, int rr);
};


#endif //TRIANGLEMODEL_INSURANCE_H
