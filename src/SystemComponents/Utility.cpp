//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include "Utility.h"
#include "../Utils/Utils.h"
#include "WaterSources/ReservoirExpansion.h"
#include "WaterSources/Quarry.h"

/**
 * Main constructor for the Utility class.
 * @param name Utility name (e.g. Raleigh_water)
 * @param id Numeric id assigned to that utility.
 * @param demands_all_realizations Text file containing utility's demand series.
 * @param number_of_week_demands Length of weeks in demand series.
 */

Utility::Utility(char *name, int id, vector<vector<double>> *demands_all_realizations, int number_of_week_demands,
                 const double percent_contingency_fund_contribution, const double water_price_per_volume,
                 WwtpDischargeRule wwtp_discharge_rule) :
        name(name), id(id), demands_all_realizations(demands_all_realizations),
        number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        water_price_per_volume(water_price_per_volume), infrastructure_discount_rate(NON_INITIALIZED),
        wwtp_discharge_rule(wwtp_discharge_rule) {

}


Utility::Utility(char *name, int id, vector<vector<double>> *demands_all_realizations, int number_of_week_demands,
                 const double percent_contingency_fund_contribution, const double water_price_per_volume,
                 const vector<int> infrastructure_build_order, double infrastructure_discount_rate,
                 WwtpDischargeRule wwtp_discharge_rule) :
        name(name), id(id), demands_all_realizations(demands_all_realizations),
        number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        water_price_per_volume(water_price_per_volume), infrastructure_construction_order(infrastructure_build_order),
        infrastructure_discount_rate(infrastructure_discount_rate),
        wwtp_discharge_rule(wwtp_discharge_rule) {

    if (infrastructure_build_order.empty())
        throw std::invalid_argument("Infrastructure construction order vector must have at least one water source ID. "
                                            "If there's not infrastructure to be build, use the other constructor "
                                            "instead.");
    if (infrastructure_discount_rate <= 0)
        throw std::invalid_argument("Infrastructure discount rate must be greater than 0.");

    total_stored_volume = NONE;
    total_storage_capacity = NONE;
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
                                     infrastructure_discount_rate(utility.infrastructure_discount_rate),
                                     demands_all_realizations(utility.demands_all_realizations),
                                     wwtp_discharge_rule(utility.wwtp_discharge_rule) {

    /// Create copies of sources
    water_sources.clear();
    for (map<int, WaterSource *>::value_type &ws : utility.water_sources) {
        if (ws.second->source_type == RESERVOIR) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Reservoir(*dynamic_cast<Reservoir *>(ws.second))));
        } else if (ws.second->source_type == INTAKE) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Intake(*dynamic_cast<Intake *>(ws.second))));
        } else if (ws.second->source_type == QUARRY) {
            water_sources.insert(pair<int, WaterSource *>
                                         (ws.first, new Quarry(*dynamic_cast<Quarry *>(ws.second))));
        } else {
            string error_message = "Water source  of type " + std::to_string(ws.second->source_type) + " cannot be "
                    "added to utility. \nSource ID: " + std::to_string(ws.second->id);
            throw logic_error(error_message);
        }
    }
}

/**
 * Destructor.
 */
Utility::~Utility() {
    if (demand_series)
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

    demands_all_realizations = utility.demands_all_realizations;

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
        total_stored_volume += (ws.second->isOnline() ? max(1.0e-6, ws.second->getAvailable_volume()) : 1.0e-6);
    }
}

void Utility::clearWaterSources() {
    water_sources.clear();
}

/**
 * Connects a reservoir to the utility.
 * @param water_source
 */
void Utility::addWaterSource(WaterSource *water_source) {
    if (water_sources.count(water_source->id)) {
        cout << "Water source ID: " << water_source->id << endl << "Utility ID: " << id << endl;
        __throw_invalid_argument("Attempt to add water source with duplicate ID to utility.");
    }

    /// Catch if user entered a water source as to be built but didn't enter bond parameters.
    for (int ws : infrastructure_construction_order)
        if (water_source->id == ws)
            if (water_source->construction_cost_of_capital == NON_INITIALIZED) {
                cout << "Utility " << id << " set to build water source " << ws << ", which has no bond "
                        "parameters set." << endl;
                __throw_invalid_argument("Water source with bond parameters but set as to be built.");
            }

    water_sources.insert(pair<int, WaterSource *>(water_source->id, water_source));

    if (water_source->isOnline()) {
        total_storage_capacity += water_source->getCapacity();
        total_treatment_capacity += water_source->raw_water_main_capacity;
    }
    total_stored_volume = total_storage_capacity;
}

/**
 * Splits demands among sources. Demand is allocated so that it used the river intakes to their capacity before
 * allocating to reservoirs. Also, whenever the on/offline status of sources starts being used, this will need to be
 * updated.
 * @param week
 */
void Utility::splitDemands(int week, double *water_sources_draws) {
    unrestricted_demand = demand_series[week];
    restricted_demand = unrestricted_demand * demand_multiplier - demand_offset;
    double reservoirs_demand = restricted_demand;

    /// Allocates demand to intakes.
    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->isOnline() && (ws.second->source_type == INTAKE || ws.second->source_type == WATER_REUSE)) {
            int i = ws.second->id;
            double intake_demand = min(restricted_demand, ws.second->getAvailable_volume());
            water_sources_draws[i] = intake_demand;
            reservoirs_demand -= intake_demand;
        }
    }

    /// Allocates remaining demand to reservoirs.
    for (map<int, WaterSource *>::value_type &ws : water_sources) {
        if (ws.second->isOnline() && ws.second->source_type == RESERVOIR) {
            int i = ws.second->id;
            double source_demand =
                    reservoirs_demand * max(1.0e-6, ws.second->getAvailable_volume()) / total_stored_volume;
            water_sources_draws[i] = source_demand;
        }
    }

    /// Update contingency fund
    updateContingencyFund(unrestricted_demand, demand_multiplier, demand_offset);
}

void Utility::resetDataColletionVariables() {
    /// Reset demand multiplier, offset from transfers and insurance payout and price.
    demand_multiplier = 1;
    demand_offset = 0;
    insurance_payout = 0;
    insurance_purchase = 0;
    drought_mitigation_cost = 0;
    infrastructure_built = {};
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
    /// If reservoir expansion, add its capacity to the corresponding existing reservoir.
    if (water_sources.at(source_id)->source_type != RESERVOIR_EXPANSION)
        water_sources.at(source_id)->setOnline();
    else {
        ReservoirExpansion reservoir_expansion = *dynamic_cast<ReservoirExpansion *>(water_sources.at(source_id));
        water_sources.at(reservoir_expansion.parent_reservoir_ID)->addCapacity(reservoir_expansion.getCapacity());
    }
    /// Updates total storage and treatment capacities.
    total_storage_capacity += water_sources.at(source_id)->getCapacity();
    total_treatment_capacity += water_sources.at(source_id)->raw_water_main_capacity;
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

        /// Checks if ROF threshold for next infrastructure in line has been reached and if
        /// there is already infrastructure being built.
        if (long_term_rof > water_sources.at(
                infrastructure_construction_order[0])->construction_rof && !under_construction) {

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
            beginConstruction(week, infrastructure_construction_order[0]);
        }

        /// If there's a water source under construction, check if it's ready and, if so, clear it from the to-build list.
        if (under_construction && week > construction_end_date) {
            setWaterSourceOnline(infrastructure_construction_order[0]);

            /// Record ID of and when infrastructure option construction was completed.
            infrastructure_built = {id, week, infrastructure_construction_order[0]};

            /// Erase infrastructure option from vector of infrastructure to be built.
            infrastructure_construction_order.erase(infrastructure_construction_order.begin());
            under_construction = false;
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
void Utility::beginConstruction(int week, int infra_id) {
    under_construction = true;
    construction_end_date = week + (int) water_sources[infra_id]->construction_time;
}

void Utility::getWastewater_releases(int week, double *discharges) {
    for (int id : *wwtp_discharge_rule.discharge_to_source_ids)
        discharges[id] = restricted_demand * wwtp_discharge_rule.get_dependent_variable(id, week);
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

void Utility::setDemand_multiplier(double demand_multiplier) {
    Utility::demand_multiplier = demand_multiplier;
}

double Utility::getContingency_fund() const {
    return contingency_fund;
}

void Utility::addInsurancePayout(double payout_value) {
    contingency_fund += payout_value;
    insurance_payout = payout_value;
}

void Utility::purchaseInsurance(double insurance_price) {
    contingency_fund -= insurance_price;
    insurance_purchase = insurance_price;
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

double Utility::getInsurance_payout() const {
    return insurance_payout;
}

double Utility::getInsurance_purchase() const {
    return insurance_purchase;
}

const vector<int> &Utility::getInfrastructure_construction_order() const {
    return infrastructure_construction_order;
}

const vector<int> Utility::getInfrastructure_built() const {
    return infrastructure_built;
}

/**
 * Get time series corresponding to realization index and eliminate reference to comprehensive demand
 * data set.
 * @param r
 */
void Utility::setRelization(unsigned long r) {
    demand_series = new double[demands_all_realizations->at(r).size()];
    std::copy(demands_all_realizations->at(r).begin(), demands_all_realizations->at(r).end(), demand_series);
    demands_all_realizations = NULL;
}
