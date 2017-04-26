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
    double insurance_price;
    vector<int> ids_of_utilities_with_policies;
    vector<vector<int>> sources_to_utilities_ids;
    vector<vector<double>> storages;
    vector<double> rof_levels;

public:
    const vector<double> rof_triggers;
    const double insurance_premium;

    Insurance(const int id, const vector<double> &rof_triggers, const double insurance_premium,
              const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
              const Graph &water_sources_graph, const vector<vector<int>> &water_sources_to_utilities);

    Insurance(const Insurance &insurance);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) override;

    double priceInsurance(int week);
};


#endif //TRIANGLEMODEL_INSURANCE_H
