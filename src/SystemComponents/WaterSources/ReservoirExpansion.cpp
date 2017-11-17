//
// Created by bernardoct on 5/3/17.
//

#include "ReservoirExpansion.h"

ReservoirExpansion::ReservoirExpansion(
        const char *name, const int id,
        const unsigned int parent_reservoir_ID,
        const float capacity,
        const float construction_rof_or_demand,
        const vector<float> &construction_time_range, float permitting_period,
        float construction_cost, float bond_term,
        float bond_interest_rate)
        : WaterSource(name, id, vector<Catchment *>(), capacity,
                      NON_INITIALIZED, RESERVOIR_EXPANSION,
                      construction_time_range, permitting_period,
                      construction_cost, bond_term, bond_interest_rate),
          parent_reservoir_ID(parent_reservoir_ID) {}

/**
 * Copy constructor.
 * @param reservoir
 */
ReservoirExpansion::ReservoirExpansion(const ReservoirExpansion &reservoir_expansion) :
        WaterSource(reservoir_expansion),
        parent_reservoir_ID(reservoir_expansion.parent_reservoir_ID) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
ReservoirExpansion &ReservoirExpansion::operator=(const ReservoirExpansion &reservoir_expansion) {
    WaterSource::operator=(reservoir_expansion);
    return *this;
}

void ReservoirExpansion::applyContinuity(int week, float upstream_source_inflow,
                                         float wastewater_discharge,
                                         vector<float> &demand_outflow) {
    __throw_logic_error("Reservoir expansion only add storage volume to the "
                                "reservoir they're assigned to.  Continuity "
                                "cannot be called on it, but only on the "
                                "reservoir it's  assigned to expand.");
}
