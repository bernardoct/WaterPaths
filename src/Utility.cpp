//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include "Utility.h"
#include "Aux.h"

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
Utility::Utility(Utility &utility) : id(utility.id), number_of_week_demands(number_of_week_demands),
                                     total_storage_capacity(utility.total_storage_capacity),
                                     total_stored_volume(utility.total_stored_volume) {

    water_sources.clear();
    for (map<int, WaterSource *>::value_type &ws : utility.water_sources) {
        if (ws.second->source_type.compare("Reservoir") == 0) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Reservoir(*dynamic_cast<Reservoir *>(ws.second))));
        }
    }

    // The problem is in this copy line.
    demand_series = new double[utility.number_of_week_demands];
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
