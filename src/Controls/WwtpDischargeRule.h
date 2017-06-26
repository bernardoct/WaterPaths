//
// Created by bernardoct on 6/26/17.
//

#ifndef TRIANGLEMODEL_WWTPDISCHARGERULES_H
#define TRIANGLEMODEL_WWTPDISCHARGERULES_H


#include "ControlRules.h"
#include <vector>

using namespace std;

class WwtpDischargeRule : public ControlRules {
private:
    double *source_id_to_vector_index;

public:
    const vector<vector<double>> *year_series_fraction_discharge;
    const vector<int> *discharge_to_source_ids;

    WwtpDischargeRule(const vector<vector<double>> *year_series_fraction_discharge,
                      const vector<int> *discharge_to_source_ids);

    WwtpDischargeRule(WwtpDischargeRule &wwtp_discharge_rule);

    WwtpDischargeRule &operator=(const WwtpDischargeRule &wwtp_discharge_rule);

    double get_dependent_variable(double water_source_id) override;

    double get_dependent_variable(int water_source_id) override;

    double get_dependent_variable(double x, int week) override;

    double get_dependent_variable(int x, int week) override;

};


#endif //TRIANGLEMODEL_WWTPDISCHARGERULES_H
