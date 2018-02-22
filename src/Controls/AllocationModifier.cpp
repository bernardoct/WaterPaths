//
// Created by David on 2/6/2018.
//

#include "AllocationModifier.h"

AllocationModifier::AllocationModifier(vector<int> *allocation_adjustment_weeks,
                                       vector<vector<double>> *new_capacity_allocations,
                                       vector<vector<double>> *new_treatment_capacities) :
        allocation_adjustment_weeks(allocation_adjustment_weeks),
        new_capacity_allocations(new_capacity_allocations),
        new_treatment_capacities(new_treatment_capacities) {}

AllocationModifier::AllocationModifier(const AllocationModifier &allocation_modifier)
        : ControlRules(allocation_modifier) {}

double AllocationModifier::get_dependent_variable(double water_source_ID) {
    __throw_invalid_argument("Allocation adjustments need a water source ID (int) "
                                     "and utility member IDs (int) "
                                     "in order to set changes in allocations.");
}

double AllocationModifier::get_dependent_variable(int water_source_ID) {
    __throw_invalid_argument("Allocation adjustments need a water source ID (int) "
                                     "and utility member IDs (int) "
                                     "in order to set changes in allocations.");
}

double AllocationModifier::get_dependent_variable(double x, int week) {
    __throw_invalid_argument("Allocation adjustments need a water source ID (int) "
                                     "and utility member IDs (int) "
                                     "in order to set changes in allocations.");
}

double AllocationModifier::get_dependent_variable(int x, int week) {
    __throw_invalid_argument("Allocation adjustments need a water source ID (int) "
                                     "and utility member IDs (int) "
                                     "in order to set changes in allocations.");
}
