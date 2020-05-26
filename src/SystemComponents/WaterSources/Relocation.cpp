//
// Created by bernardoct on 7/25/17.
//

#include "Relocation.h"

Relocation::Relocation(string name, const int id,
                       unsigned long parent_reservoir_ID,
                       vector<double> *allocated_fractions,
                       vector<int> *utilities_with_allocations,
                       vector<int> construction_prerequisites,
                       const vector<double> &construction_time_range, double permitting_period,
                       Bond &bond)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NONE,
                      construction_prerequisites, SOURCE_RELOCATION,
                      construction_time_range, permitting_period, bond),
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

    throw logic_error("Source relocations just changes allocated fractions "
                                "in the source they're assigned to. Continuity "
                                "cannot be called on it, but only on the "
                                "source it's assigned to relocate.");
}

unsigned long Relocation::getParent_reservoir_ID() const {
    return parent_reservoir_ID;
}

