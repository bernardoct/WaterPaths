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
Utility::Utility(string name, int id, char const *demand_file_name, int number_of_week_demands,
                 const double percent_contingency_fund_contribution, const double water_price_per_volume) :
        name(name), id(id), number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        water_price_per_volume(water_price_per_volume) {

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
                                     demand_series(new double[utility.number_of_week_demands]),
                                     percent_contingency_fund_contribution(utility.percent_contingency_fund_contribution),
                                     water_price_per_volume(utility.water_price_per_volume) {

    /// Create copies of sources
    water_sources.clear();
    for (map<int, WaterSource *>::value_type &ws : utility.water_sources) {
        if (ws.second->source_type == RESERVOIR) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Reservoir(*dynamic_cast<Reservoir *>(ws.second))));
        } else {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Intake(*dynamic_cast<Intake *>(ws.second))));
        }
    }

    /// Copy demand series so that restrictions in one realization do not affect other realizations.
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

    /// Create copies of sources
    water_sources.clear();
    for (auto &ws : utility.water_sources) {
        if (ws.second->source_type == RESERVOIR) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Reservoir(*dynamic_cast<Reservoir *>(ws.second))));
        } else {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Intake(*dynamic_cast<Intake *>(ws.second))));
        }
    }


    /// Copy demand series so that restrictions in one realization do not affect other realizations.
    std::copy(utility.demand_series, utility.demand_series + utility.number_of_week_demands, demand_series_temp);
    delete[] demand_series;
    demand_series = demand_series_temp;

    return *this;
}

/**
 * Sorting by id compare operator.
 * @param other
 * @return
 */
bool Utility::operator<(const Utility *other) {
    return id < other->id;
}

/**
 * Updates the total current storage held by the utility and all its reservoirs.
 */
void Utility::updateTotalStoredVolume() {
    total_stored_volume = 0.0;

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
    if (water_source->isOnline()) {
        total_storage_capacity += water_source->capacity;
        total_treatment_capacity += water_source->max_treatment_capacity;
    }
}

/**
 * Splits demands among sources. Demand is allocated so that it used the river intakes to their capacity before
 * allocating to reservoirs. Also, whenever the on/offline status of sources starts being used, this will need to be
 * updated.
 * @param week
 */
void Utility::splitDemands(int week) {
    int i;
    double intake_demand;
    double remaining_demand = demand_series[week] * demand_multiplier;

    /// Allocates demand to intakes.
    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->source_type == INTAKE) {
            i = ws.second->id;
            intake_demand = min(remaining_demand, ws.second->getAvailable_volume());
            split_demands_among_sources.at(i) = intake_demand;
            remaining_demand -= intake_demand;
        }
    }

    /// Allocates remaining demand to reservoirs.
    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->source_type == RESERVOIR) {
            i = ws.second->id;
            split_demands_among_sources.at(i) = remaining_demand *
                                                max(1.0e-6, water_sources.at(i)->getAvailable_volume()) /
                                                total_stored_volume;
        }
    }

    /// Update contingency fund
    this->updateContingencyFund(week);

    /// Reset demand multiplier.
    demand_multiplier = 1;
}

void Utility::updateContingencyFund(int week) {
    contingency_fund += percent_contingency_fund_contribution * demand_series[week] * water_price_per_volume;
    contingency_fund -= demand_series[week] * (1 - demand_multiplier) * water_price_per_volume;
}

void Utility::setWaterSourceOnline(int source_id) {
    water_sources.at(source_id)->setOnline();
    total_storage_capacity += water_sources.at(source_id)->capacity;
    total_treatment_capacity += water_sources.at(source_id)->max_treatment_capacity;
}

/**
 * Assigns a fraction of the total weekly demand to a reservoir according to its current storage in relation
 * to the combined current stored of all reservoirs where the utility has .
 * @param week
 * @param water_source_id
 * @return proportional demand.
 */
double Utility::getReservoirDraw(const int water_source_id) {
    return split_demands_among_sources.at(water_source_id);
}

const map<int, WaterSource *> &Utility::getWaterSource() const {
    return water_sources;
}

double Utility::getDemand(const int week) {
    return demand_series[week];
}

double Utility::getStorageToCapacityRatio() const {
    return total_stored_volume / total_storage_capacity;
}

double Utility::getTotal_storage_capacity() const {
    return total_storage_capacity;
}

double Utility::getRisk_of_failure() const {
    return risk_of_failure;
}

void Utility::setRisk_of_failure(double risk_of_failure) {
    this->risk_of_failure = risk_of_failure;
}

double Utility::getTotal_treatment_capacity() const {
    return total_treatment_capacity;
}

double Utility::getTotal_available_volume() const {
    double total_available_volume = 0;
    for (auto & ws : water_sources) {
        total_available_volume += ws.second->getAvailable_volume();
    }

    return total_available_volume;
}

double Utility::getWater_price_per_volume() const {
    return water_price_per_volume;
}

void Utility::drawFromContingencyFund(double amount) {

}

void Utility::addToContingencyFund(double amount) {

}

void Utility::setDemand_multiplier(double demand_multiplier) {
    Utility::demand_multiplier = demand_multiplier;
}

double Utility::getContingency_fund() const {
    return contingency_fund;
}
