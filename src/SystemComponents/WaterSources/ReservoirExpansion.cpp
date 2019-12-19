//
// Created by bernardoct on 5/3/17.
//

#include "ReservoirExpansion.h"

ReservoirExpansion::ReservoirExpansion(string name, const int id,
                                       const unsigned int parent_reservoir_ID,
                                       const double capacity,
                                       vector<int> construction_prerequisites,
                                       const vector<double> &construction_time_range,
                                       double permitting_period, Bond &bond)
        : WaterSource(name, id, vector<Catchment *>(), capacity,
                      NON_INITIALIZED, construction_prerequisites,
                      RESERVOIR_EXPANSION,
                      construction_time_range, permitting_period, bond),
          parent_reservoir_ID(parent_reservoir_ID) {}

/**
 * Copy constructor.
 * @param reservoir
 */
ReservoirExpansion::ReservoirExpansion(
        const ReservoirExpansion &reservoir_expansion) :
        WaterSource(reservoir_expansion),
        parent_reservoir_ID(reservoir_expansion.parent_reservoir_ID) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
ReservoirExpansion &
ReservoirExpansion::operator=(const ReservoirExpansion &reservoir_expansion) {
    WaterSource::operator=(reservoir_expansion);
    return *this;
}

void
ReservoirExpansion::applyContinuity(int week, double upstream_source_inflow,
                                    double wastewater_discharge,
                                    vector<double> &demand_outflow) {
    throw logic_error("Reservoir expansion only add storage volume to the "
                      "reservoir they're assigned to.  Continuity "
                      "cannot be called on it, but only on the "
                      "reservoir it's  assigned to expand.");
}
