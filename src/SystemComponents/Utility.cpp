//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include "Utility.h"
#include "../Utils/Aux.h"

/**
 * Main constructor for the Utility class.
 * @param name Utility name (e.g. Raleigh_water)
 * @param id Numeric id assigned to that utility.
 * @param demand_file_name Text file containing utility's demand series.
 * @param number_of_week_demands Length of weeks in demand series.
 */
Utility::Utility(char const *name, int id, char const *demand_file_name, int number_of_week_demands) :
        name(name), id(id), number_of_week_demands(number_of_week_demands) {

    demand_series = Aux::parse1DCsvFile(demand_file_name, number_of_week_demands);
    cout << "Utility " << name << " created." << endl;

}

/**
 * Copy constructor.
 * @param utility
 */
Utility::Utility(Utility &utility) : id(utility.id), number_of_week_demands(utility.number_of_week_demands),
                                     total_storage_capacity(utility.total_storage_capacity),
                                     total_stored_volume(utility.total_stored_volume),
                                     demand_series(new double[utility.number_of_week_demands]) {

    water_sources.clear();
    for (map<int, WaterSource *>::value_type &ws : utility.water_sources) {
        if (ws.second->source_type.compare("Reservoir") == 0) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Reservoir(*dynamic_cast<Reservoir *>(ws.second))));
        }
    }

    std::copy(utility.demand_series, utility.demand_series + utility.number_of_week_demands, demand_series);
}

/**
 * Destructor.
 */
Utility::~Utility() {
    delete[] demand_series;
}

/**
 * Copy assignment operator.
 * @param utility
 * @return Copy of utility.
 */
Utility &Utility::operator=(const Utility &utility) {
    double *demand_series_temp = demand_series;

    water_sources.clear();
    for (auto &ws : utility.water_sources) {
        if (ws.second->source_type.compare("Reservoir")) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Reservoir(*dynamic_cast<Reservoir *>(ws.second))));
        }
    }

    std::copy(utility.demand_series, utility.demand_series + utility.number_of_week_demands, demand_series_temp);
    delete[] demand_series;
    demand_series = demand_series_temp;

    return *this;
}

/**
 * Updates the total current storage held by the utility and all its reservoirs.
 */
void Utility::updateTotalStoredVolume() {
    total_stored_volume = 0;

    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        total_stored_volume += max(1.0e-6, ws.second->getAvailable_volume());
    }
}

/**
 * Connects a reservoir to the utility.
 * @param water_source
 */
void Utility::addWaterSource(WaterSource *water_source) {
    water_sources.insert(pair<int, WaterSource *>(water_source->id, water_source));
    split_demands_among_sources.insert(pair<int, double>(water_source->id, 0));

    if (water_source->isOnline()) total_storage_capacity += water_source->capacity;
}

/**
 * Assigns a fraction of the total weekly demand to a reservoir according to its current storage in relation
 * to the combined current stored of all reservoirs where the utility has .
 * @param week
 * @param water_source_id
 * @return proportional demand.
 */
double Utility::getDemand(int water_source_id) {
    return split_demands_among_sources.at(water_source_id);
}

/**
 * Splits demands among sources. As different source types are added, this will need to be updated. Also,
 * whenever the on/offline status of sources starts being used, this will need to be updated.
 * @param week
 */
void Utility::splitDemands(int week) {
    int i, number_of_reservoir_with_water = 0;

    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->getAvailable_volume() > 0) {
            number_of_reservoir_with_water++;
        }
    }

    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        i = ws.second->id;
        split_demands_among_sources.at(i) = demand_series[week] *
                                            max(1.0e-6, water_sources.at(i)->getAvailable_volume()) /
                                            total_stored_volume;
    }

}

const map<int, WaterSource *> &Utility::getWaterSource() const {
    return water_sources;
}

double Utility::getStorageToCapacityRatio() const {
    return total_stored_volume / total_storage_capacity;
}

double Utility::getTotal_storage_capacity() const {
    return total_storage_capacity;
}

