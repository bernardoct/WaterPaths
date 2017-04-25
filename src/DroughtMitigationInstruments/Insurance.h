//
// Created by bernardoct on 4/24/17.
//

#ifndef TRIANGLEMODEL_INSURANCE_H
#define TRIANGLEMODEL_INSURANCE_H

#include "../SystemComponents/Utility.h"
#include "../Utils/Constants.h"
#include "../Utils/DataCollector.h"



class Insurance : public DroughtMitigationPolicy {
private:
    Insurance(const Insurance &insurance);

    vector<WaterSource *> water_sources;
    const Graph *water_sources_graph;
    vector<int> ids_of_utilities_with_policies;
    vector<vector<int>> sources_to_utilities_ids;
    vector<vector<double>> storages;
    vector<vector<double>> rofs;


public:
    const vector<double> rof_triggers;
    const double insurance_premium;

    Insurance(const int id, const vector<double> &rof_triggers, const double insurance_premium);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources,
                             const Graph *water_sources_graph) override;

    double insurancePricing(vector<vector<double>> storage_levels);
};


#endif //TRIANGLEMODEL_INSURANCE_H
