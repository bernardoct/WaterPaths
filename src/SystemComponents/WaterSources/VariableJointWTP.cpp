//
// Created by dgorelic on 10/29/2019.
//

#include <numeric>
#include <iostream>
#include "VariableJointWTP.h"

VariableJointWTP::VariableJointWTP(const char *name, const int id, const int parent_reservoir_ID,
                                   const int expansion_sequence_id, const double total_treatment_capacity,
                                   vector<int> connected_sources, vector<int> &agreement_utility_ids,
                                   vector<double> &initial_treatment_capacity_allocations, vector<Bond *> &bonds,
                                   const vector<double> &construction_time_range, double permitting_period)
        : JointWTP(name, id, NEW_JOINT_WATER_TREATMENT_PLANT_VARIABLE_ALLOCATIONS,
                   parent_reservoir_ID, expansion_sequence_id,
                   total_treatment_capacity, connected_sources,
                   agreement_utility_ids, initial_treatment_capacity_allocations,
                   bonds,
                   construction_time_range, permitting_period),
          initial_treatment_allocations(initial_treatment_capacity_allocations) {
}

VariableJointWTP::VariableJointWTP(const VariableJointWTP &variable_joint_water_treatment_plant) :
        JointWTP(variable_joint_water_treatment_plant),
        initial_treatment_allocations(variable_joint_water_treatment_plant.initial_treatment_allocations) {
}

VariableJointWTP::~VariableJointWTP() = default;

VariableJointWTP &VariableJointWTP::operator=(const VariableJointWTP &variable_joint_water_treatment_plant) {
    JointWTP::operator=(variable_joint_water_treatment_plant);
    return *this;
}

void VariableJointWTP::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                                       vector<double> &demand_outflow) {
    JointWTP::applyContinuity(week, upstream_source_inflow, wastewater_discharge, demand_outflow);
}

double VariableJointWTP::implementInitialTreatmentCapacity(int utility_id) {
    return initial_treatment_allocations[utility_id];
}

void VariableJointWTP::resetAllocations(const vector<double> *demand_deltas) {
    // take estimate of future expected next-year demand compared to current demand to set
    // treatment allocations for the JointWTP for the rest of the year
    for (int u : *utilities_with_allocations) {
        allocated_treatment_capacities[u] += demand_deltas->at(u);

        if (allocated_treatment_capacities[u] < 0) {
//            allocated_treatment_capacities[u] = 0;
            cout << "Utility " << u << " allocated treatment capacity is now "
                 << allocated_treatment_capacities[u] << " after a change of " << demand_deltas->at(u) << endl;
            throw logic_error("Error in VariableJointWTP::resetAllocations, "
                              "allocated treatment capacity for at least one utility is negative.");
        }
        allocated_treatment_fractions[u] = allocated_treatment_capacities[u] / total_treatment_capacity;
    }

    // check for over-allocation and set fractions
    double temporary_total_allocated_capacity =
            accumulate(allocated_treatment_capacities.begin(),allocated_treatment_capacities.end(),0.0);
    if (temporary_total_allocated_capacity > total_treatment_capacity)
        for (unsigned long i = 0; i < utilities_with_allocations->size(); ++i) {
            int u = utilities_with_allocations->at(i);
            allocated_treatment_fractions[u] = allocated_treatment_capacities[u] / temporary_total_allocated_capacity;
            allocated_treatment_capacities[u] = allocated_treatment_fractions[u] * total_treatment_capacity;
        }

}
