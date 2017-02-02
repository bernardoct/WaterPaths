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
        "Reservoir") {
    storage_records.assign((unsigned long) catchments[0]->series_length, 0);
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
 * @param upstream_reservoir_inflow
 * @param demand_outflow
 */
void Reservoir::updateAvailableVolume(int week, double upstream_reservoir_inflow, double demand_outflow) {

    double total_inflow = upstream_reservoir_inflow;
    for (Catchment *c : catchments) {
        total_inflow += c->getStreamflow((week));
    }
    demand_previous_week = demand_outflow;
    double new_volume = available_volume + total_inflow - demand_outflow - min_environmental_outflow;
    double released_volume = min_environmental_outflow;
    double spillage = 0;

    this->week = week;

    if (online) {
        if (new_volume > capacity) {
            spillage = new_volume - capacity;
            released_volume += spillage;
            new_volume = capacity;
        }
    } else {
        new_volume = available_volume;
        released_volume = total_inflow;
    }

    available_volume = new_volume;
    outflow_previous_week = released_volume;
    upstream_inflow_previous_week = upstream_reservoir_inflow;
    catchment_inflow_previous_week = total_inflow - upstream_reservoir_inflow;
}

void Reservoir::setAvailableVolumeAndOutflowPreviousRelease(double available_volume, double outflow_previous_week) {
    this->available_volume = available_volume;
    this->outflow_previous_week = outflow_previous_week;
}

