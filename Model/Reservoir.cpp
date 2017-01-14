//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Reservoir.h"

using namespace std;

Reservoir::Reservoir(const string reservoir_name, double stored_volume, const double capacity,
                     const double min_environmental_flow, bool online) : reservoir_name(reservoir_name),
                                                                         stored_volume(stored_volume),
                                                                         capacity(capacity),
                                                                         min_environmental_flow(min_environmental_flow),
                                                                         online(online) {

    cout << "Reservoir " << reservoir_name << " created" << endl;
}

double Reservoir::applyContinuity(double inflow, double demand_outflow) {
    double new_volume = stored_volume + inflow - demand_outflow - min_environmental_flow;
    double released_volume = min_environmental_flow;

    if (online) {
        if (new_volume > capacity) {
            released_volume += new_volume - capacity;
            new_volume = capacity;
        }
    } else {
        new_volume = stored_volume;
        released_volume = inflow;
    }

    stored_volume = new_volume;

    return released_volume;
}

double Reservoir::getStored_volume() const {
    return stored_volume;
}

