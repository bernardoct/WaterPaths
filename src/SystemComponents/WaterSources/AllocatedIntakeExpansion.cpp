//
// Created by dgorelic on 3/2/2020.
//

#include <algorithm>
#include "AllocatedIntakeExpansion.h"

AllocatedIntakeExpansion::AllocatedIntakeExpansion(const char *name, const int id, const unsigned int parent_intake_ID,
                                                   const double total_supply_capacity_added,
                                                   double total_treatment_capacity_added,
                                                   vector<int> &partner_utility_ids,
                                                   vector<double> &supply_capacity_allocations_added,
                                                   vector<double> &treatment_capacity_allocations_added,
                                                   const vector<double> &construction_time_range,
                                                   vector<int> connected_sources, double permitting_period,
                                                   vector<Bond *> &bonds)
        : WaterSource(name, id, vector<Catchment *>(), total_supply_capacity_added, total_treatment_capacity_added,
                connected_sources, &partner_utility_ids,
                &supply_capacity_allocations_added, &treatment_capacity_allocations_added,
                ALLOCATED_INTAKE_EXPANSION, construction_time_range, permitting_period, bonds),
          parent_intake_ID(parent_intake_ID),
          supply_capacity_added(supply_capacity_allocations_added),
          treatment_capacity_added(treatment_capacity_allocations_added) {

}

AllocatedIntakeExpansion::AllocatedIntakeExpansion(const AllocatedIntakeExpansion &intake_expansion) :
        WaterSource(intake_expansion),
        parent_intake_ID(intake_expansion.parent_intake_ID),
        supply_capacity_added(intake_expansion.supply_capacity_added),
        treatment_capacity_added(intake_expansion.treatment_capacity_added) {}

AllocatedIntakeExpansion &AllocatedIntakeExpansion::operator=(const AllocatedIntakeExpansion &intake_expansion) {
    WaterSource::operator=(intake_expansion);
    return *this;
}

void AllocatedIntakeExpansion::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                                               vector<double> &demand_outflow) {

}

double AllocatedIntakeExpansion::getAllocatedCapacity(int utility_id) {
    // locate element index of utility id
    auto it = find(utilities_with_allocations->begin(), utilities_with_allocations->end(), utility_id);

    // return the index within to get the allocation
    int u_index = -9999;
    if (it != utilities_with_allocations->end()) {
        u_index = distance(utilities_with_allocations->begin(), it);
    } else {
        __throw_logic_error("Error in AllocatedIntakeExpansion::getAllocatedCapacity, can't locate allocation.");
    }

    return supply_capacity_added[u_index];
}

double AllocatedIntakeExpansion::getAllocatedTreatmentCapacity(int utility_id) const {
    // locate element index of utility id
    auto it = find(utilities_with_allocations->begin(), utilities_with_allocations->end(), utility_id);

    // return the index within to get the allocation
    int u_index = -9999;
    if (it != utilities_with_allocations->end()) {
        u_index = distance(utilities_with_allocations->begin(), it);
    } else {
        __throw_logic_error("Error in AllocatedIntakeExpansion::getAllocatedCapacity, can't locate allocation.");
    }

    return treatment_capacity_added[u_index];
}

Bond &AllocatedIntakeExpansion::getBond(int utility_id) {
    // locate element index of utility id
    auto it = find(utilities_with_allocations->begin(), utilities_with_allocations->end(), utility_id);

    // return the index within to get the bond object
    int u_index = -9999;
    if (it != utilities_with_allocations->end()) {
        u_index = distance(utilities_with_allocations->begin(), it);
    } else {
        __throw_logic_error("Error in AllocatedIntakeExpansion::getBond, can't find bond object for this utility.");
    }

    // get Bond
    return WaterSource::getBond(u_index);
}
