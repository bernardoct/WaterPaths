//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include "Utility.h"
#include "Aux.h"


Utility::Utility(char const *name, int id, char const *demand_file_name, int number_of_weeks) :
        name(name), id(id) {

    demand_series = Aux::parse1DCsvFile(demand_file_name, number_of_weeks);
    cout << "Utility " << name << " created." << endl;
}

/**
 * Updates the total current storage held by the utility and all its reservoirs.
 */
void Utility::updateTotalStoredVolume() {
    total_stored_volume = 0;

    for (map<int, WaterSource *>::value_type &r : water_sources) {
        total_stored_volume += r.second->getAvailable_volume();
    }

    return;
}

/**
 * Connects a reservoir to the utility.
 * @param water_source
 */
void Utility::addWaterSource(WaterSource *water_source) {
    water_sources.insert(pair<int, WaterSource *>(water_source->id, water_source));
    if (water_source->isOnline()) total_storage_capacity += water_source->capacity;
}

/**
 * Assigns a fraction of the total weekly demand to a reservoir according to its current storage in relation
 * to the combined current stored of all reservoirs where the utility has .
 * @param week
 * @param reservoir_id
 * @return proportional demand.
 */
double Utility::getDemand(int week, int reservoir_id) {

    double reservoir_volume = water_sources.at(reservoir_id)->getAvailable_volume();
    double demand = demand_series[week] * reservoir_volume / total_stored_volume;
    return demand;
}

const map<int, WaterSource *> &Utility::getWaterSource() const {
    return water_sources;
}

