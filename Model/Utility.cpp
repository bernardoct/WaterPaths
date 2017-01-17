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
 * Generates week's random inflow, which is then split among reservoirs in getDemand. Needs to be completed by
 * making it generate a demand based on a combination of inflows from 2 or more connected reservoirs.
 * @param week
 */
void Utility::updateCurrentDemandAndTotalStoredVolume(int week) {
    current_total_demand = demand_series[week];

    total_stored_volume = 0;
    for (map<int, Reservoir>::value_type &r : reservoirs) {
        total_stored_volume += r.second.getStored_volume();
    }
}

//void Utility::assignReservoirs(map<int, Reservoir> reservoirs) {
//
//    this->reservoirs = reservoirs;
//
//    total_storage_capacity = 0;
//    for (map<int, Reservoir>::value_type &r : reservoirs) {
//        if (r.second.isOnline()) total_storage_capacity += r.second.capacity;
//    }
//    total_stored_volume = total_storage_capacity;
//}

void Utility::addReservoir(Reservoir reservoir) {
    reservoirs.insert(pair<int, Reservoir>(reservoir.id, reservoir));
    if (reservoir.isOnline()) total_storage_capacity += reservoir.capacity;
}

const map<int, Reservoir> &Utility::getReservoirs() const {
    return reservoirs;
}

double Utility::getDemand(int reservoir_id) {
    return current_total_demand * reservoirs.at(reservoir_id).getStored_volume() / total_stored_volume;
}

