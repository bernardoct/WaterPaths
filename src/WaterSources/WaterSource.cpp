//
// Created by bernardo on 1/22/17.
//

#include <iostream>
#include "WaterSource.h"

using namespace std;

WaterSource::WaterSource(const string &reservoir_name, const int id, const double min_environmental_outflow,
                         const vector<Catchment *> &catchments, bool online, const double capacity,
                         const string source_type)
        : source_name(reservoir_name), capacity(capacity),
          min_environmental_outflow(min_environmental_outflow),
          catchments(catchments), online(online),
          available_volume(capacity), id(id), source_type(source_type) {
    cout << "Reservoir " << reservoir_name << " created" << endl;
    outflow_previous_week = min_environmental_outflow;
}

/**
 * Water source mass balance. Gets releases from upstream water sources, demands from connected utilities, and
 * combines them with its catchments inflows.
 * @param week
 * @param upstream_reservoir_inflow
 * @param demand_outflow
 */
//void WaterSource::updateAvailableVolume(int week, double upstream_reservoir_inflow, double demand_outflow) {
//    cout << "Available volume update must be called from a WaterSource child class" << endl;
//}

double WaterSource::getAvailable_volume() const {
    return available_volume;
}

bool WaterSource::isOnline() const {
    return online;
}

void WaterSource::setOnline(bool online_status) {
    online = online_status;
}

double WaterSource::getOutflow_previous_week() const {
    return outflow_previous_week;
}

void WaterSource::toString() {
    cout << source_type << " " << id << " - " << this->source_name << endl;
    cout << "Status: " << online << endl;
    cout << "Last update: week " << week << endl;
    cout << "Stored volume: " << available_volume << endl;
    cout << "Outflow previous week: " << outflow_previous_week << endl;
    cout << endl;

}

