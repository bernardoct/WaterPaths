//
// Created by bernardoct on 5/3/17.
//

#include "ReservoirExpansion.h"

ReservoirExpansion::ReservoirExpansion(const string &source_name, const int id, const int parent_reservoir_ID,
                                       const double capacity, const double construction_rof,
                                       const vector<double> &construction_time_range,
                                       double construction_cost, double bond_term,
                                       double bond_interest_rate)
        : WaterSource(source_name, id, NON_INITIALIZED, vector<Catchment *>(), capacity, NON_INITIALIZED,
                      RESERVOIR_EXPANSION, construction_rof, construction_time_range, construction_cost,
                      bond_term, bond_interest_rate), parent_reservoir_ID(parent_reservoir_ID) {}

/**
 * Copy constructor.
 * @param reservoir
 */
ReservoirExpansion::ReservoirExpansion(const ReservoirExpansion &reservoir_expansion) :
        WaterSource(reservoir_expansion), parent_reservoir_ID(reservoir_expansion.parent_reservoir_ID) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
ReservoirExpansion &ReservoirExpansion::operator=(const ReservoirExpansion &reservoir_expansion) {
    WaterSource::operator=(reservoir_expansion);
    return *this;
}

void ReservoirExpansion::applyContinuity(int week, double upstream_source_inflow, double demand_outflow) {

}
