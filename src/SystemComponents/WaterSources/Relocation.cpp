//
// Created by bernardoct on 7/25/17.
//

#include "Relocation.h"

Relocation::Relocation(
        const char *name, const int id, unsigned long parent_reservoir_ID,
        vector<float> *allocated_fractions,
        vector<int> *utilities_with_allocations,
        const float construction_rof_or_demand,
        const vector<float> &construction_time_range, float permitting_period,
        float construction_cost_of_capital, float bond_term,
        float bond_interest_rate)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NONE,
                      SOURCE_RELOCATION, construction_time_range,
                      permitting_period, construction_cost_of_capital,
                      bond_term, bond_interest_rate),
          parent_reservoir_ID(parent_reservoir_ID), new_allocated_fractions
                  (allocated_fractions),
          utilities_with_allocations(utilities_with_allocations) {}

/**
 * Copy constructor.
 * @param reservoir
 */
Relocation::Relocation(const Relocation &relocation) :
        WaterSource(relocation),
        parent_reservoir_ID(relocation.parent_reservoir_ID),
        new_allocated_fractions(relocation.new_allocated_fractions),
        utilities_with_allocations(relocation.utilities_with_allocations) {}


void Relocation::applyContinuity(int week, float upstream_source_inflow,
                                 float wastewater_discharge,
                                 vector<float> &demand_outflow) {

    __throw_logic_error("Source relocations just changes allocated fractions "
                                "in the source they're assigned to. Continuity "
                                "cannot be called on it, but only on the "
                                "source it's assigned to relocate.");
}

unsigned long Relocation::getParent_reservoir_ID() const {
    return parent_reservoir_ID;
}

