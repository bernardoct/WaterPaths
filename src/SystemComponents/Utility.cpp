//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include "Utility.h"
#include "../Utils/Utils.h"

/**
 * Main constructor for the Utility class.
 * @param name Utility name (e.g. Raleigh_water)
 * @param id Numeric id assigned to that utility.
 * @param demand_file_name Text file containing utility's demand series.
 * @param number_of_week_demands Length of weeks in demand series.
 */

Utility::Utility(char *name, int id, char const *demand_file_name, int number_of_week_demands,
                 const double percent_contingency_fund_contribution, const double water_price_per_volume) :
        name(name), id(id), number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        water_price_per_volume(water_price_per_volume), infrastructure_discount_rate(NON_INITIALIZED) {

    demand_series = Utils::parse1DCsvFile(demand_file_name, number_of_week_demands);
}


Utility::Utility(char *name, int id, char const *demand_file_name, int number_of_week_demands,
                 const double percent_contingency_fund_contribution, const double water_price_per_volume,
                 const vector<int> infrastructure_build_order, double infrastructure_discount_rate) :
        name(name), id(id), number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        water_price_per_volume(water_price_per_volume), infrastructure_construction_order(infrastructure_build_order),
        infrastructure_discount_rate(infrastructure_discount_rate) {

    if (infrastructure_build_order.size() == 0)
        throw std::invalid_argument("Infrastructure construction order vector must have at least one water source ID. "
                                            "If there's not infrastructure to be build, use the other constructor "
                                            "instead.");
    if (infrastructure_discount_rate <= 0)
        throw std::invalid_argument("Infrastructure discount rate must be greater than 0.");

    demand_series = Utils::parse1DCsvFile(demand_file_name, number_of_week_demands);
    total_stored_volume = -1;
    total_storage_capacity = 1;
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
                                     water_price_per_volume(utility.water_price_per_volume),
                                     infrastructure_construction_order(utility.infrastructure_construction_order),
                                     infrastructure_discount_rate(utility.infrastructure_discount_rate) {

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

//    if (demand_series)
    //FIXME: I'M GETTING SEGMENTATION FAULT HERE WHEN RUNNING IT WITH VALGRIND.
    delete[] demand_series;
}

/**
 * Copy assignment operator.
 * @param utility
 * @return Copy of utility.
 */
Utility &Utility::operator=(const Utility &utility) {

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
 * Sorting by id compare operator.
 * @param other
 * @return
 */
bool Utility::operator>(const Utility *other) {
    return id > other->id;
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
    if (water_sources.count(water_source->id))
        throw std::logic_error("Attempt to add water source with duplicate ID was added to utility.");
    water_sources.insert(pair<int, WaterSource *>(water_source->id, water_source));
    split_demands_among_sources.insert(pair<int, double>(water_source->id, 0));
    if (water_source->isOnline()) {
        total_storage_capacity += water_source->capacity;
        total_treatment_capacity += water_source->max_treatment_capacity;
    }
    total_stored_volume = total_storage_capacity;
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
    unrestricted_demand = demand_series[week];
    restricted_demand = unrestricted_demand * demand_multiplier - demand_offset;

    /// Allocates demand to intakes.
    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->source_type == INTAKE) {
            i = ws.second->id;
            intake_demand = min(restricted_demand, ws.second->getAvailable_volume());
            split_demands_among_sources.at(i) = intake_demand;
            restricted_demand -= intake_demand;
        }
    }

    /// Allocates remaining demand to reservoirs.
    double demand;
    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->source_type == RESERVOIR) {
            i = ws.second->id;
            demand = restricted_demand *
                     max(1.0e-6, ws.second->getAvailable_volume()) /
                     total_stored_volume;
            if (demand > 0)
                split_demands_among_sources.at(i) = demand;
            else
                split_demands_among_sources.at(i) = 0;
        }
    }

    /// Update contingency fund
    this->updateContingencyFund(unrestricted_demand, demand_multiplier, demand_offset);

    /// Reset demand multiplier and offset.
    demand_multiplier = 1;
    demand_offset = 0;
}

/**
 * Update contingency fund based on regular contribution, restrictions, and transfers. This function works for both
 * sources and receivers of transfers, and the transfer water prices are different than regular prices for both
 * sources and receivers. It also stores the cost of drought mitigation.
 * @param unrestricted_demand
 * @param demand_multiplier
 * @param demand_offset
 * @return contingency fund contribution or draw.
 */
void Utility::updateContingencyFund(double unrestricted_demand, double demand_multiplier, double demand_offset) {
    double fund_contribution = percent_contingency_fund_contribution * unrestricted_demand * water_price_per_volume;
    double revenue_losses = unrestricted_demand * (1 - demand_multiplier) * water_price_per_volume;
    double transfer_costs = demand_offset * (offset_rate_per_volume - water_price_per_volume);

    /// contingency fund cannot get negative.
    contingency_fund = max(contingency_fund + fund_contribution - revenue_losses - transfer_costs, 0.0);

    drought_mitigation_cost = max(revenue_losses + transfer_costs - insurance_payout, 0.0);
}

void Utility::setWaterSourceOnline(int source_id) {
    /// Sets water source online.
    water_sources.at(source_id)->setOnline();
    /// Updates total storage and treatment capacities.
    total_storage_capacity += water_sources.at(source_id)->capacity;
    total_treatment_capacity += water_sources.at(source_id)->max_treatment_capacity;
}

/**
 * Checks if infrastructure should be built, triggers construction, sets it online, and handles accounting/financing
 * of it.
 * @param long_term_rof
 * @param week
 */
void Utility::infrastructureConstructionHandler(double long_term_rof, int week) {

    /// Checks whether the long erm ROF has been exceeded for the next infrastructure option in the list and, if not
    /// already under construction, starts building it.
    if (!infrastructure_construction_order.empty()) { // if there is anything to be built

        /// Checks if ROF threshold for next infrastructure in line has been reached.
        if (long_term_rof > water_sources.at(
                infrastructure_construction_order[0])->construction_rof && !underConstruction) {

            /// Add water source construction cost to the books.
            double level_debt_service_payments;
            infrastructure_net_present_cost += water_sources[infrastructure_construction_order[0]]->
                    calculateNetPresentConstructionCost(week, infrastructure_discount_rate,
                                                        &level_debt_service_payments);

            /// Create stream of level debt service payments for water source.
            debt_payment_streams.push_back(vector<double>(
                    (unsigned long) water_sources.at(infrastructure_construction_order[0])->bond_term,
                    level_debt_service_payments));

            /// Begin construction.
            beginConstruction(week);
        }

        /// If there's a water source under construction, check if it's ready and, if so, clear it from the to-build list.
        if (underConstruction) {
            if (week > construction_start_date + water_sources[infrastructure_construction_order[0]]->construction_time) {
                setWaterSourceOnline(infrastructure_construction_order[0]);

                /// Record ID of and when infrastructure option construction was completed.
                vector<int> infra_added = {week, infrastructure_construction_order[0]};
                infrastructure_built.push_back(infra_added);

                /// Erase infrastructure option from vector of infrastructure to be built.
                infrastructure_construction_order.erase(infrastructure_construction_order.begin());
            }
        }
    }

    /// Calculate current annual debt payment to be made on that week (it first week of year), if any.
    current_debt_payment = updateCurrent_debt_payment(week, debt_payment_streams);
}

/**
 * Calculates total debt payments to be made in a week, if that's the first week of the year.
 * @param week
 * @param debt_payment_streams
 * @return
 */
double Utility::updateCurrent_debt_payment(int week, vector<vector<double>> debt_payment_streams) {

    double current_debt_payment = 0;

    /// Checks if it's the first week of the year, when outstanding debt payments should be made.
    if (Utils::isFirstWeekOfTheYear(week + 1)) {
        /// Checks if there's outstanding debt.
        if (!debt_payment_streams.empty()) {
            /// Iterate over all outstanding debt.
            for (vector<double> &pmt : debt_payment_streams) {
                if (!pmt.empty()) {
                    /// Adds outstanding debt payment to total current annual payment.
                    current_debt_payment += pmt.at(0);
                    /// Scratch outstanding debt.
                    pmt.erase(pmt.begin());
                }
            }
        }
    }

    return current_debt_payment;
}

/**
 * Kicks off construction and records when it was initiated.
 * @param week
 */
void Utility::beginConstruction(int week) {
    underConstruction = true;
    construction_start_date = week;
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

const map<int, WaterSource *> &Utility::getWaterSources() const {
    return water_sources;
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

void Utility::setDemand_multiplier(double demand_multiplier) {
    Utility::demand_multiplier = demand_multiplier;
}

double Utility::getContingency_fund() const {
    return contingency_fund;
}

void Utility::setDemand_offset(double demand_offset, double offset_rate_per_volume) {
    Utility::demand_offset = demand_offset;
    Utility::offset_rate_per_volume = offset_rate_per_volume;
}

double Utility::getUnrestrictedDemand() const {
    return unrestricted_demand;
}

double Utility::getRestrictedDemand() const {
    return restricted_demand;
}

double Utility::getDemand_multiplier() const {
    return demand_multiplier;
}

double Utility::getUnrestrictedDemand(int week) const {
    return demand_series[week];
}

double Utility::getInfrastructure_net_present_cost() const {
    return infrastructure_net_present_cost;
}

double Utility::getCurrent_debt_payment() const {
    return current_debt_payment;
}

double Utility::getCurrent_contingency_fund_contribution() const {
    return restricted_demand * percent_contingency_fund_contribution * water_price_per_volume;
}

double Utility::getDrought_mitigation_cost() const {
    return drought_mitigation_cost;
}
