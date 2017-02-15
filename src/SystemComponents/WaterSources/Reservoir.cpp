//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Reservoir.h"

using namespace std;


Reservoir::Reservoir(const string &source_name, const int id, const double min_environmental_outflow,
                     const vector<Catchment *> &catchments, bool online, const double capacity) : WaterSource(
        source_name, id,
        min_environmental_outflow,
        catchments, online,
        capacity,
        RESERVOIR) {
}

/**
 * Copy constructor.
 * @param reservoir
 */
Reservoir::Reservoir(const Reservoir &reservoir) : WaterSource(reservoir) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
Reservoir &Reservoir::operator=(const Reservoir &reservoir) {
    WaterSource::operator=(reservoir);
    return *this;
}

/**
 * Destructor.
 */
Reservoir::~Reservoir() {
    catchments.clear();
}

/**
 * Reservoir mass balance. Gets releases from upstream reservoirs, demands from connected utilities, and
 * combines them with its catchments inflows.
 * @param week
 * @param upstream_source_inflow
 * @param demand_outflow
 */
void Reservoir::updateAvailableVolume(int week, double upstream_source_inflow, double demand_outflow) {

    double catchment_inflow = 0;
    for (Catchment *c : catchments) {
        catchment_inflow += c->getStreamflow((week));
    }

    double stored_volume_new =
            available_volume + upstream_source_inflow + catchment_inflow - demand_outflow - min_environmental_outflow;
    double outflow_new = min_environmental_outflow;

    this->week = week;

    if (online) {
        if (stored_volume_new > capacity) {
            outflow_new += stored_volume_new - capacity;
            stored_volume_new = capacity;
        }
    } else {
        stored_volume_new = available_volume;
        outflow_new = upstream_source_inflow + catchment_inflow;
    }

    demand_previous_week = demand_outflow;
    available_volume = stored_volume_new;
    total_outflow = outflow_new;
    this->upstream_source_inflow = upstream_source_inflow;
    upstream_catchment_inflow = catchment_inflow;
}

