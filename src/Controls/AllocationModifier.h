//
// Created by David on 2/6/2018.
//

#ifndef TRIANGLEMODEL_ALLOCATIONMODIFIER_H
#define TRIANGLEMODEL_ALLOCATIONMODIFIER_H


#include "Base/ControlRules.h"
#include <vector>

using namespace std;

class AllocationModifier : public ControlRules {
public:

    vector<int> *allocation_adjustment_weeks;
    vector<vector<double>> *new_capacity_allocations;
    vector<vector<double>> *new_treatment_capacities;

    AllocationModifier(vector<int> *allocation_adjustment_weeks,
                       vector<vector<double>> *new_capacity_allocations,
                       vector<vector<double>> *new_treatment_capacities);

    AllocationModifier(const AllocationModifier &allocation_modifier);

    double get_dependent_variable(double water_source_ID) override;

    double get_dependent_variable(int water_source_ID) override;

    double get_dependent_variable(double x, int week) override;

    double get_dependent_variable(int x, int week) override;
};


#endif //TRIANGLEMODEL_ALLOCATIONMODIFIER_H
