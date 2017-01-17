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

void Utility::updateCurrentDemandAndTotalStoredVolume(int week, double inflow) {
    current_total_demand = demand_series[week];

    total_storage_capacity = 0;
    for (map<int, Reservoir>::value_type &r : reservoirs) {
        total_storage_capacity += r.second.getStored_volume();
    }
}

void Utility::assignReservoirs(map<int, Reservoir> reservoirs) {

    this->reservoirs = reservoirs;

    total_storage_capacity = 0;
    for (map<int, Reservoir>::value_type &r : reservoirs) {
        total_storage_capacity += r.second.capacity;
    }
    total_stored_volume = total_storage_capacity;
}

const map<int, Reservoir> &Utility::getReservoirs() const {
    return reservoirs;
}

double Utility::getDemand(int reservoir_id) {
    return current_total_demand * reservoirs.at(reservoir_id).getStored_volume() / total_stored_volume;
}

