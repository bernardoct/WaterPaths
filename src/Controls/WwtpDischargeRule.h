//
// Created by bernardoct on 6/26/17.
//

#ifndef TRIANGLEMODEL_WWTPDISCHARGERULES_H
#define TRIANGLEMODEL_WWTPDISCHARGERULES_H


#include "Base/ControlRules.h"
#include <vector>

using namespace std;

class WwtpDischargeRule : public ControlRules {
private:
    vector<int> source_id_to_vector_index;

public:
    const vector<vector<float>> *year_series_fraction_discharge;
    const vector<int> *discharge_to_source_ids;

    WwtpDischargeRule(const vector<vector<float>> *year_series_fraction_discharge,
                      const vector<int> *discharge_to_source_ids);

    WwtpDischargeRule(WwtpDischargeRule &wwtp_discharge_rule);

    WwtpDischargeRule &operator=(const WwtpDischargeRule &wwtp_discharge_rule);

    float get_dependent_variable(float water_source_id) override;

    float get_dependent_variable(int water_source_id) override;

    float get_dependent_variable(float x, int week) override;

    float get_dependent_variable(int x, int week) override;

};


#endif //TRIANGLEMODEL_WWTPDISCHARGERULES_H
