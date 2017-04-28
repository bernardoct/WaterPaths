//
// Created by bernardoct on 4/24/17.
//

#ifndef TRIANGLEMODEL_INSURANCE_H
#define TRIANGLEMODEL_INSURANCE_H

#include <x86intrin.h>
#include "../SystemComponents/Utility.h"
#include "../Utils/Constants.h"
#include "../Utils/DataCollector.h"

class Insurance : public DroughtMitigationPolicy, ContinuityModelROF {
private:
    vector<double> insurance_prices;
    vector<int> ids_of_utilities_with_policies;

public:
    const vector<double> rof_triggers;
    const double insurance_premium;
    const vector<vector<int>> sources_to_utilities_ids;
    const vector<double> fixed_payouts;

    Insurance(const int id, const vector<double> &rof_triggers, const double insurance_premium,
              const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
              const Graph &water_sources_graph, const vector<vector<int>> &water_sources_to_utilities,
              vector<double> fixed_payouts);

    Insurance(const Insurance &insurance);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) override;

    vector<double> priceInsurance(int week);

    double runRofSubRealization(vector<vector<bool>> *realizations_utility_week_fail, int rr, int w);

    void
    calculateRealizationsStoragesAndFailures(vector<vector<vector<bool>>> *realizations_utility_week_fail,
                                             vector<vector<vector<double>>> *realizations_storages, int week);

    double calculateUtilityInsurancePrice(vector<vector<vector<bool>>> *realizations_utility_week_fail,
                                          vector<vector<vector<double>>> *distances_between_realizations, int u);
};


#endif //TRIANGLEMODEL_INSURANCE_H
