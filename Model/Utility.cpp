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

    for (map<int, Reservoir *>::value_type &r : reservoirs) {
        total_stored_volume += r.second->getAvailable_volume();
    }

    return;
}

/**
 * Connects a reservoir to the utility.
 * @param reservoir
 */
void Utility::addReservoir(Reservoir *reservoir) {
    reservoirs.insert(pair<int, Reservoir *>(reservoir->id, reservoir));
    if (reservoir->isOnline()) total_storage_capacity += reservoir->capacity;
}

/**
 * Assigns a fraction of the total weekly demand to a reservoir according to its current storage in relation
 * to the combined current stored of all reservoirs where the utility has .
 * @param week
 * @param reservoir_id
 * @return proportional demand.
 */
double Utility::getDemand(int week, int reservoir_id) {

    double reservoir_volume = reservoirs.at(reservoir_id)->getAvailable_volume();
    double demand = demand_series[week] * reservoir_volume / total_stored_volume;
    return demand;
}

const map<int, Reservoir *> &Utility::getReservoirs() const {
    return reservoirs;
}

