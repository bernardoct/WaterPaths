//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Reservoir.h"

using namespace std;

Reservoir::Reservoir(const string &reservoir_name, const int id, const double min_environmental_flow,
                     const Catchment &catchment, bool online, const double capacity)
        : reservoir_name(reservoir_name), capacity(capacity),
          min_environmental_flow(min_environmental_flow),
          catchment(catchment), online(online),
          stored_volume(capacity), id(id) {
    cout << "Reservoir " << reservoir_name << " created" << endl;
    release_previous_week = min_environmental_flow;
}

void Reservoir::applyContinuity(int week, double upstream_reservoir_inflow, double demand_outflow) {

    double total_inflow = upstream_reservoir_inflow + catchment.getStreamflow((week));
    double new_volume = stored_volume + total_inflow - demand_outflow - min_environmental_flow;
    double released_volume = min_environmental_flow;
    double spillage = 0;

    this->week = week;

    if (online) {
        if (new_volume > capacity) {
            spillage = new_volume - capacity;
            released_volume += spillage;
            new_volume = capacity;
        }
    } else {
        new_volume = stored_volume;
        released_volume = total_inflow;
    }

    stored_volume = new_volume;
    release_previous_week = released_volume;

}

double Reservoir::getStored_volume() const {
    return stored_volume;
}

bool Reservoir::isOnline() const {
    return online;
}

void Reservoir::setOnline(bool online_status) {
    Reservoir::online = online_status;
}

double Reservoir::getRelease_previous_week() const {
    return release_previous_week;
}

void Reservoir::toString() {
    cout << "Reservoir " << id << ", " << this->reservoir_name << endl;
    cout << "Status: " << online << endl;
    cout << "Last update: week " << week << endl;
    cout << "Stored volume: " << stored_volume << endl;
    cout << "Release previous week: " << release_previous_week << endl;
    cout << endl;

}

