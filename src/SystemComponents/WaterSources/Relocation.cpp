//
// Created by bernardoct on 7/25/17.
//

#include "Relocation.h"

Relocation::Relocation(const char *name, const int id, unsigned long parent_reservoir_ID,
                       vector<double> *allocated_fractions, vector<int> *utilities_with_allocations,
                       const vector<double> &construction_time_range, double permitting_period,
                       double construction_cost_of_capital)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NONE, vector<int>(), SOURCE_RELOCATION,
                      construction_time_range, permitting_period, construction_cost_of_capital),
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


void Relocation::applyContinuity(int week, double upstream_source_inflow,
                                 double wastewater_discharge,
                                 vector<double> &demand_outflow) {

    __throw_logic_error("Source relocations just changes allocated fractions "
                                "in the source they're assigned to. Continuity "
                                "cannot be called on it, but only on the "
                                "source it's assigned to relocate.");
}

unsigned long Relocation::getParent_reservoir_ID() const {
    return parent_reservoir_ID;
}

