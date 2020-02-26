//
// Created by dgorelic on 10/28/2019.
//

#include "FixedJointWTP.h"

FixedJointWTP::FixedJointWTP(const char *name, const int id, const int parent_reservoir_ID,
                             const int expansion_sequence_id, const double total_treatment_capacity,
                             vector<int> connected_sources, vector<int> &agreement_utility_ids,
                             vector<double> &fixed_treatment_capacity_allocations,
                             vector<Bond *> &bonds, const vector<double> &construction_time_range,
                             double permitting_period)
        : JointWTP(name, id, NEW_JOINT_WATER_TREATMENT_PLANT_FIXED_ALLOCATIONS,
                   parent_reservoir_ID, expansion_sequence_id,
                   total_treatment_capacity, connected_sources,
                   agreement_utility_ids, fixed_treatment_capacity_allocations, bonds,
                   construction_time_range, permitting_period),
          permanent_treatment_allocations(&fixed_treatment_capacity_allocations) {
}

FixedJointWTP::FixedJointWTP(const FixedJointWTP &fixed_joint_water_treatment_plant) :
        JointWTP(fixed_joint_water_treatment_plant),
        permanent_treatment_allocations(fixed_joint_water_treatment_plant.permanent_treatment_allocations) {
}

FixedJointWTP::~FixedJointWTP() = default;

FixedJointWTP &FixedJointWTP::operator=(const FixedJointWTP &fixed_joint_water_treatment_plant) {
    JointWTP::operator=(fixed_joint_water_treatment_plant);
    return *this;
}

void FixedJointWTP::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                                    vector<double> &demand_outflow) {
    JointWTP::applyContinuity(week, upstream_source_inflow, wastewater_discharge, demand_outflow);
}

double FixedJointWTP::implementInitialTreatmentCapacity(int utility_id) {
    // NOTE: these will be incorrect if the indices in this vector do not match the order
    // their respective utility ids are in the agreement_utility_ids vector.
    return permanent_treatment_allocations->at(utility_id);
}
