//
// Created by bernardoct on 4/30/17.
//

#ifndef TRIANGLEMODEL_INSURANCE_H
#define TRIANGLEMODEL_INSURANCE_H


#include "DroughtMitigationPolicy.h"
#include "../ContinuityModels/ContinuityModelROF.h"

class InsurancePseudoROF : public DroughtMitigationPolicy, ContinuityModelROF {
protected:
    vector<double> insurance_prices;
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

    vector<double> priceInsurance(int week);

    void calculateRealizationsStorages(std::vector<std::vector<std::vector<double>>> *realizations_storages,
                                       int week);

    void calculatePseudoRofs(vector<vector<vector<double>>> *realizations_storages,
                             vector<vector<double>> *realizations_rofs,
                             unsigned long r);


    vector<vector<double>> shiftStorageCurves(vector<vector<double>> *storage_curves_2yrs, vector<double> storage0,
                                              unsigned long first_week);

    const double getInsurancePrice(int u) const;

    virtual ~InsurancePseudoROF();
};


#endif //TRIANGLEMODEL_INSURANCE_H
