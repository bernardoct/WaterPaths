//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include <numeric>
#include <algorithm>
#include "Utility.h"
#include "../../Utils/Utils.h"
#include "InfrastructureManager.h"

/**
 * Main constructor for the Utility class.
 * @param name Utility name (e.g. Raleigh_water)
 * @param id Numeric ID assigned to that utility.
 * @param demands_all_realizations Text file containing utility's demand series.
 * @param number_of_week_demands Length of weeks in demand series.
 * @param typesMonthlyDemandFraction Table of size 12 (months in year) by
 * number of consumer tiers with the fraction of the total demand consumed by
 * each tier in each month of the year. The last column must be the fraction
 * of the demand treated as sewage. The summation of all number in a row but
 * the last one, therefore, must sum to 1.
 * @param typesMonthlyWaterPrice Monthly water price for each tier. The last
 * column is the price charged for waste water treatment.
 * @param wwtp_discharge_rule 53 weeks long time series according to which
 * fractions of sewage is discharged in different water sources (normally one
 * for each WWTP).
 */

Utility::Utility(
        string name, int id,
        vector<vector<double>> &demands_all_realizations,
        int number_of_week_demands,
        const double percent_contingency_fund_contribution,
        const vector<vector<double>> &typesMonthlyDemandFraction,
        const vector<vector<double>> &typesMonthlyWaterPrice,
        WwtpDischargeRule wwtp_discharge_rule,
        double demand_buffer,
        vector<vector<int>> water_source_to_wtp,
        vector<double> utility_owned_wtp_capacities) :
        total_storage_capacity(NONE),
        total_available_volume(NONE),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demands_all_realizations(demands_all_realizations),
        infra_discount_rate(NON_INITIALIZED),
        id(id),
        number_of_week_demands(number_of_week_demands),
        name(name),
        percent_contingency_fund_contribution(
                percent_contingency_fund_contribution),
        demand_buffer(demand_buffer),
        utility_owned_wtp_capacities(utility_owned_wtp_capacities) {
    calculateWeeklyAverageWaterPrices(typesMonthlyDemandFraction,
                                      typesMonthlyWaterPrice);
    unrollWaterSourceToWtpVector(water_source_to_wtp,
                                 utility_owned_wtp_capacities);
}

void Utility::unrollWaterSourceToWtpVector(
        const vector<vector<int>> &water_source_to_wtp,
        const vector<double> &utility_owned_wtp_capacities) {

    if (water_source_to_wtp.size() != utility_owned_wtp_capacities.size()) {
        char error[512];
        sprintf(error, "Utility %s has %lu WTPs but %lu water sources (or "
                       "groups of) assigned to WTPs.", name.c_str(),
                utility_owned_wtp_capacities.size(),
                water_source_to_wtp.size());
        throw invalid_argument(error);
    }

    for (int i = 0; i < water_source_to_wtp.size(); ++i) {
        for (int ws : water_source_to_wtp[i]) {
            if (ws >= this->water_source_to_wtp.size()) {
                this->water_source_to_wtp.resize(ws + 1, NON_INITIALIZED);
            }
            this->water_source_to_wtp[ws] = i;
        }
    }
}

/**
 * Constructor for when there is infrastructure to be built.
 * @param name Utility name (e.g. Raleigh_water)
 * @param id Numeric id assigned to that utility.
 * @param demands_all_realizations Text file containing utility's demand series.
 * @param number_of_week_demands Length of weeks in demand series.
 * @param percent_contingency_fund_contribution
 * @param typesMonthlyDemandFraction Table of size 12 (months in year) by
 * number of consumer tiers with the fraction of the total demand consumed by
 * each tier in each month of the year. The last column must be the fraction
 * of the demand treated as sewage. The summation of all number in a row but
 * the last one, therefore, must sum to 1.
 * @param typesMonthlyWaterPrice Monthly water price for each tier. The last
 * column is the price charged for waste water treatment.
 * @param wwtp_discharge_rule 53 weeks long time series according to which
 * fractions of sewage is discharged in different water sources (normally one
 * for each WWTP).
 * @param rof_infra_construction_order
 * @param infra_discount_rate
 * @param infra_if_built_remove if infra option in position 0 of a row is
 * built, remove infra options of IDs in remaining positions of the same row.
 */
Utility::Utility(string name, int id,
                 vector<vector<double>> &demands_all_realizations,
                 int number_of_week_demands,
                 const double percent_contingency_fund_contribution,
                 const vector<vector<double>> &typesMonthlyDemandFraction,
                 const vector<vector<double>> &typesMonthlyWaterPrice,
                 WwtpDischargeRule wwtp_discharge_rule,
                 double demand_buffer,
                 vector<vector<int>> water_source_to_wtp,
                 vector<double> utility_owned_wtp_capacities,
                 const vector<int> &rof_infra_construction_order,
                 const vector<int> &demand_infra_construction_order,
                 const vector<double> &infra_construction_triggers,
                 double infra_discount_rate,
                 const vector<vector<int>> &infra_if_built_remove) :
        total_storage_capacity(NONE),
        total_available_volume(NONE),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demands_all_realizations(demands_all_realizations),
        infra_discount_rate(infra_discount_rate),
        id(id),
        number_of_week_demands(number_of_week_demands),
        name(name),
        percent_contingency_fund_contribution(
                percent_contingency_fund_contribution),
        demand_buffer(demand_buffer),
        utility_owned_wtp_capacities(utility_owned_wtp_capacities) {

    // Check if sources were passed to be triggered by both rof and demand, and
    // if so throw an error. If only one rof/demand trigger value was passed,
    // assign it to all infrastructure options.
    auto expanded_infra_construction_triggers = infra_construction_triggers;
    unsigned long size = max(rof_infra_construction_order.size(),
                             demand_infra_construction_order.size());
    if (infra_construction_triggers.size() == 1 && size > 1) {
        unsigned long size = max(rof_infra_construction_order.size(),
                                 demand_infra_construction_order.size());
        expanded_infra_construction_triggers.resize(
                size, expanded_infra_construction_triggers[0]);
    } else if (infra_construction_triggers.size() == 1 &&
               !rof_infra_construction_order.empty() &&
               !demand_infra_construction_order.empty()) {
        char error[500];
        sprintf(error, "Utility %s has infrastructure options to be "
                       "triggered by both ROF and demand but only one trigger "
                       "value was passed, which can be either. Please stick to "
                       "either ROF or demand.", name.c_str());
    }
    infrastructure_construction_manager =
            InfrastructureManager(name, id, expanded_infra_construction_triggers,
                                  infra_if_built_remove,
                                  infra_discount_rate,
                                  rof_infra_construction_order,
                                  demand_infra_construction_order);

    unrollWaterSourceToWtpVector(water_source_to_wtp,
                                 utility_owned_wtp_capacities);

    infrastructure_construction_manager.connectWaterSourcesVectorsToUtilitys(
            water_sources,
            priority_draw_water_source,
            non_priority_draw_water_source);

    if (rof_infra_construction_order.empty() &&
        demand_infra_construction_order.empty())
        throw std::invalid_argument("At least one infrastructure construction "
                                    "order vector  must have at least "
                                    "one water source ID. If there's "
                                    "not infrastructure to be build, "
                                    "use other constructor "
                                    "instead.");
    if (infra_discount_rate <= 0)
        throw std::invalid_argument("Infrastructure discount rate must be "
                                    "greater than 0.");

    if (demands_all_realizations.empty()) {
        char error[256];
        sprintf(error, "Empty demand vectors passed to utility %d", id);
        throw std::invalid_argument(error);
    }

    calculateWeeklyAverageWaterPrices(typesMonthlyDemandFraction,
                                      typesMonthlyWaterPrice);
}

Utility::Utility(Utility &utility) :
        weekly_average_volumetric_price(
                utility.weekly_average_volumetric_price),
        total_storage_capacity(utility.total_storage_capacity),
        total_available_volume(utility.total_available_volume),
        total_treatment_capacity(utility.total_treatment_capacity),
        wwtp_discharge_rule(utility.wwtp_discharge_rule),
        demands_all_realizations(utility.demands_all_realizations),
        demand_series_realization(utility.demand_series_realization),
        infra_discount_rate(utility.infra_discount_rate),
        bond_term_multiplier(utility.bond_term_multiplier),
        bond_interest_rate_multiplier(utility.bond_interest_rate_multiplier),
        id(utility.id),
        number_of_week_demands(utility.number_of_week_demands),
        name(utility.name),
        percent_contingency_fund_contribution(
                utility.percent_contingency_fund_contribution),
        demand_buffer(utility.demand_buffer),
        infrastructure_construction_manager(
                utility.infrastructure_construction_manager),
        water_source_to_wtp(
                utility.water_source_to_wtp),
        utility_owned_wtp_capacities(utility.utility_owned_wtp_capacities) {

    infrastructure_construction_manager.connectWaterSourcesVectorsToUtilitys(
            water_sources,
            priority_draw_water_source,
            non_priority_draw_water_source);

    // Create copies of sources
    water_sources.clear();
}

Utility::~Utility() {
    water_sources.clear();
    delete[] utility_owned_wtp_capacities_tmp;
    delete[] available_treated_flow_rate;
}

Utility &Utility::operator=(const Utility &utility) {
    demand_series_realization = vector<double>(
            (unsigned long) utility.number_of_week_demands);

    infrastructure_construction_manager.connectWaterSourcesVectorsToUtilitys(
            water_sources,
            priority_draw_water_source,
            non_priority_draw_water_source);

    // Create copies of sources
    water_sources.clear();

    return *this;
}

bool Utility::operator<(const Utility *other) {
    return id < other->id;
}

bool Utility::operator>(const Utility *other) {
    return id > other->id;
}

bool Utility::compById(Utility *a, Utility *b) {
    return a->id < b->id;
}

void Utility::updateTreatmentAndNumberOfStorageSources() {
    n_storage_sources = non_priority_draw_water_source.size();
    delete[] available_treated_flow_rate;
    available_treated_flow_rate = new double[non_priority_draw_water_source.size()];
    for (int i = 0; i < n_storage_sources; ++i) {
        auto ws = water_sources[non_priority_draw_water_source[i]];
        available_treated_flow_rate[i] = utility_owned_wtp_capacities[water_source_to_wtp[ws->id]];
        total_storage_treatment_capacity += available_treated_flow_rate[i];
    }

    delete[] utility_owned_wtp_capacities_tmp;
    n_wtp = utility_owned_wtp_capacities.size();
    utility_owned_wtp_capacities_tmp = new double[n_wtp];

    total_treatment_capacity = accumulate(utility_owned_wtp_capacities.begin(),
                                          utility_owned_wtp_capacities.end(),
                                          0.);

    //TODO: IMPLEMENT HERE QP PROBLEM UPDATE
//    P_x = new double[n_storage_sources];
//    A_x = new double[n_storage_sources];
}

/**
 * Calculates average water price from consumer types and respective prices.
 * @param typesMonthlyDemandFraction
 * @param typesMonthlyWaterPrice
 */
void Utility::calculateWeeklyAverageWaterPrices(
        const vector<vector<double>> &typesMonthlyDemandFraction,
        const vector<vector<double>> &typesMonthlyWaterPrice) {
    priceCalculationErrorChecking(typesMonthlyDemandFraction,
                                  typesMonthlyWaterPrice);

    weekly_average_volumetric_price = vector<double>((int) WEEKS_IN_YEAR + 1,
                                                     0.);
    double monthly_average_price[NUMBER_OF_MONTHS] = {};
    int n_tiers = static_cast<int>(typesMonthlyWaterPrice.at(0).size());

    // Calculate monthly average prices across consumer types.
    for (int m = 0; m < NUMBER_OF_MONTHS; ++m) {
        for (int t = 0; t < n_tiers; ++t) {
            monthly_average_price[m] += typesMonthlyDemandFraction[m][t] *
                                        typesMonthlyWaterPrice[m][t];
        }
    }
    // Create weekly price table from monthly prices.
    for (int w = 0; w < (int) (WEEKS_IN_YEAR + 1); ++w) {
        weekly_average_volumetric_price[w] =
                monthly_average_price[(int) (w / WEEKS_IN_MONTH)] /
                WEEKS_IN_MONTH;
    }
}

/**
 * Checks price calculation input matrices for errors.
 * @param typesMonthlyDemandFraction
 * @param typesMonthlyWaterPrice
 */
void Utility::priceCalculationErrorChecking(
        const vector<vector<double>> &typesMonthlyDemandFraction,
        const vector<vector<double>> &typesMonthlyWaterPrice) {
    if (typesMonthlyDemandFraction.size() != NUMBER_OF_MONTHS) {
        char error[500];
        sprintf(error, "Error in utility %s. There must be 12 "
                       "total demand fractions per tier but only %lu were "
                       "found.", name.c_str(),
                typesMonthlyDemandFraction.size());
        throw invalid_argument(error);
    }
    if (typesMonthlyWaterPrice.size() != NUMBER_OF_MONTHS) {
        char error[500];
        sprintf(error, "Error in utility %s. There must be 12 water "
                       "prices per tier but only %lu were found.", name.c_str(),
                typesMonthlyWaterPrice.size());
        throw invalid_argument(error);
    }
    if ((&typesMonthlyWaterPrice)[0].size() !=
        (&typesMonthlyDemandFraction)[0].size()) {
        char error[500];
        sprintf(error, "There must be demand fractions and water prices"
                       " for the same number of tiers but %lu fractions and %lu"
                       " tiers were found.",
                (&typesMonthlyWaterPrice)[0].size(),
                (&typesMonthlyDemandFraction)[0].size());
        throw invalid_argument(error);
    }
}

/**
 * updates combined stored volume for this utility.
 */
void Utility::updateTotalAvailableVolume() {
    total_available_volume = 0.0;
    total_stored_volume = 0.0;
    net_stream_inflow = 0.0;

    for (int ws : priority_draw_water_source) {
        total_available_volume +=
                max(1.0e-6,
                    water_sources[ws]->getAvailableAllocatedVolume(id));
        net_stream_inflow += water_sources[ws]->getAllocatedInflow(id);
    }

    for (int i = 0; i < non_priority_draw_water_source.size(); ++i) {
        auto ws = water_sources[non_priority_draw_water_source[i]];
        double stored_volume = max(1.0e-6,
                                   ws->getAvailableAllocatedVolume(id));
        total_available_volume += stored_volume;
        total_stored_volume += stored_volume;
        net_stream_inflow += ws->getAllocatedInflow(id);
        available_treated_flow_rate[i] = utility_owned_wtp_capacities[water_source_to_wtp[ws->id]];
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
    checkErrorsAddWaterSourceOnline(water_source);

    // Add water sources with their IDs matching the water sources vector
    // indexes.
    if (water_source->id > (int) water_sources.size() - 1) {
        water_sources.resize((unsigned int) water_source->id + 1);
    }

    // Add water source
    water_sources[water_source->id] = water_source;

    // Add water source to infrastructure construction manager.
    infrastructure_construction_manager.addWaterSource(water_source);

    // If watersource is online and the utility owns some of its installed
    // treatment capacity, make it online.
    double ws_treat_capacity = 0;
    try {
        if (find(SOURCES_REQUIRING_TREATMENT.begin(),
                 SOURCES_REQUIRING_TREATMENT.end(),
                 water_source->source_type) !=
                SOURCES_REQUIRING_TREATMENT.end()) {
            ws_treat_capacity = utility_owned_wtp_capacities.at(
                    water_source_to_wtp.at(water_source->id)
            );
        }
    } catch (out_of_range &e) {
        char error[256];
        sprintf(error, "Treatment capacity was assigned to utility %s "
                       "for water source %s without being connected to it. If "
                       "you passed an input file to WaterPaths, check [WS TO "
                       "UTILITY MATRIX]. Otherwise, check the connectivity "
                       "matrix.", name.c_str(), water_source->name.c_str());
        throw invalid_argument(error);
    }

    if (water_source->isOnline() && ws_treat_capacity > 0) {
        infrastructure_construction_manager.addWaterSourceToOnlineLists(
                water_source->id, total_storage_capacity,
                total_available_volume,
                total_stored_volume);
    }

    n_sources++;
    max_capacity += water_source->getAllocatedCapacity(id);

    updateTreatmentAndNumberOfStorageSources();
}

void Utility::checkErrorsAddWaterSourceOnline(WaterSource *water_source) {
    for (WaterSource *ws : water_sources) {
        if ((ws != nullptr) && ws->id == water_source->id) {
            cout << "Water source ID: " << water_source->id << endl <<
                 "Utility ID: " << id << endl;
            throw invalid_argument("Attempt to add water source with "
                                   "duplicate ID to utility.");
        }
    }
}

#pragma GCC optimize("O3")
bool Utility::idealDemandSplitUnconstrained(double *split_demands,
                                            const double *available_treated_flow_rate,
                                            double total_demand,
                                            const double *storage,
                                            double total_storage,
                                            int n_storage_sources) {
    bool treatment_capacity_violated = false;
    for (int i = 0; i < n_storage_sources; ++i) {
        split_demands[i] = total_demand * storage[i] / total_storage;
        if (split_demands[i] - 1e-9 > available_treated_flow_rate[i]) {
            treatment_capacity_violated = true;
        }
    }
    return treatment_capacity_violated;
}

#pragma GCC optimize("O3")
bool Utility::idealDemandSplitConstrained(double *split_demands,
                                          bool *over_allocated,
                                          bool *has_spare_capacity,
                                          const double *available_treated_flow_rate,
                                          double total_demand,
                                          const double *storage,
                                          double total_storage,
                                          int n_storage_sources) {
    // Consider only storage of sources that are still not at provision capacity.
    total_storage = 0;
    for (int j = 0; j < n_storage_sources; ++j) {
        if (has_spare_capacity[j]) total_storage += storage[j];
    }

    // Split demands not fulfilled by sources at provision capacity across
    // sources with spare capacity while checking for over allocation.
    bool treatment_capacity_violated = false;
    for (int i = 0; i < n_storage_sources; ++i) {
        if (has_spare_capacity[i]) {
            split_demands[i] = total_demand * storage[i] / total_storage;
        }
        over_allocated[i] =
                split_demands[i] - 1e-9 > available_treated_flow_rate[i];
        has_spare_capacity[i] =
                split_demands[i] + 1e-9 < available_treated_flow_rate[i];
        if (over_allocated[i]) treatment_capacity_violated = true;
    }
    return treatment_capacity_violated;
}

/**
 * Splits demands among sources. Demand is allocated so that river intakes
 * and reuse are first used to their capacity before requesting water from
 * allocations in reservoirs.
 * @param week
 */
#pragma GCC optimize("O3")
void Utility::splitDemands(
        int week, vector<vector<double>> &demands,
        bool apply_demand_buffer) {
    memcpy(utility_owned_wtp_capacities_tmp, utility_owned_wtp_capacities.data(),
            sizeof(double) * n_wtp);
    unrestricted_demand = demand_series_realization[week] +
                          apply_demand_buffer * demand_buffer *
                          weekly_peaking_factor[Utils::weekOfTheYear(week)];
    restricted_demand = unrestricted_demand * demand_multiplier - demand_offset;
    unfulfilled_demand = max(max(restricted_demand - total_available_volume,
                                 restricted_demand - total_treatment_capacity),
                             0.);
    restricted_demand -= unfulfilled_demand;
    double demand_non_priority_sources = restricted_demand;
    double total_serviced_demand = 0;

    // Allocates demand to intakes and reuse based on allocated volume to
    // this utility.
    for (int &ws : priority_draw_water_source) {
        double max_source_output = min(
                water_sources[ws]->getAvailableAllocatedVolume(id),
                utility_owned_wtp_capacities_tmp[water_source_to_wtp[ws]]);
        double source_demand =
                min(demand_non_priority_sources,
                    max_source_output);
        demands[ws][this->id] = source_demand;
        demand_non_priority_sources -= source_demand;
        total_serviced_demand += source_demand;
        utility_owned_wtp_capacities_tmp[water_source_to_wtp[ws]] -= source_demand;
    }

    double storages[n_storage_sources];
    double total_available_flow_rate = 0;
    for (int i = 0; i < n_storage_sources; ++i) {
        auto ws = water_sources[non_priority_draw_water_source[i]];
        storages[i] = ws->getAvailableAllocatedVolume(id);
        available_treated_flow_rate[i] = min(
                storages[i],
                utility_owned_wtp_capacities_tmp[water_source_to_wtp[ws->id]]
        );
        total_available_flow_rate += available_treated_flow_rate[i];
    }

    bool treatment_capacity_violation = false;
    if (demand_non_priority_sources > total_available_flow_rate) {
        // If the utility's demand is greater than the sum of treatment
        // capacities of all water sources, all WTPs will be fully used.
        for (int i = 0; i < n_storage_sources; ++i) {
            demands[non_priority_draw_water_source[i]][id] =
                    available_treated_flow_rate[i];
        }
        treatment_capacity_violation = true;
    } else if (demand_non_priority_sources > 0) {
        // If a given WTP cannot fulfill its ideal demand but there is spare
        // treatment capacity available in other WTPs, use it.

        // Create auxiliary variables and check which sources are over allocated
        // and which have spare capacity.
        bool has_spare_flow_rate[n_storage_sources];
        memset(has_spare_flow_rate, true, 2);
        bool over_allocated[n_storage_sources];
        double split_demands[n_storage_sources];

        treatment_capacity_violation = idealDemandSplitUnconstrained(
                split_demands,
                available_treated_flow_rate,
                demand_non_priority_sources,
                storages,
                total_stored_volume,
                n_storage_sources);

        if (treatment_capacity_violation) {
            // Check which sources are over allocated or have spare capacity.
            for (int i = 0; i < n_storage_sources; ++i) {
                over_allocated[i] = split_demands[i] - 1e-9 >
                                    available_treated_flow_rate[i];
                has_spare_flow_rate[i] =
                        split_demands[i] + 1e-9 <
                        available_treated_flow_rate[i];
            }

            // Redistribute demands across water sources that may still have
            // spare capacity.
            while (treatment_capacity_violation) {
                double remainder_demand = demand_non_priority_sources;
                for (int i = 0; i < n_storage_sources; ++i) {
                    if (over_allocated[i]) {
                        split_demands[i] = available_treated_flow_rate[i];
                    }
                    if (!has_spare_flow_rate[i]) {
                        remainder_demand -= split_demands[i];
                    }
                }
                treatment_capacity_violation = idealDemandSplitConstrained(
                        split_demands,
                        over_allocated,
                        has_spare_flow_rate,
                        available_treated_flow_rate,
                        remainder_demand,
                        storages,
                        total_stored_volume,
                        non_priority_draw_water_source.size());
            }
        }

        for (int j = 0; j < n_storage_sources; ++j) {
            demands[non_priority_draw_water_source[j]][id] = split_demands[j];
        }
    }

    // Update contingency fund
    if (treatment_capacity_violation) {
        unfulfilled_demand = restricted_demand - total_available_flow_rate;
    } else {
        unfulfilled_demand = 0;
    }
    if (used_for_realization) {
        updateContingencyFundAndDebtService(unrestricted_demand,
                                            demand_multiplier,
                                            demand_offset,
                                            unfulfilled_demand,
                                            week);
    }
}

/**
 * Update contingency fund based on regular contribution, restrictions, and
 * transfers. This function works for both sources and receivers of
 * transfers, and the transfer water prices are different than regular prices
 * for both sources and receivers. It also stores the cost of drought
 * mitigation.
 * @param unrestricted_demand
 * @param demand_multiplier
 * @param demand_offset
 * @return contingency fund contribution or draw.
 */
#pragma GCC optimize("O3")
void Utility::updateContingencyFundAndDebtService(
        double unrestricted_demand, double demand_multiplier,
        double demand_offset, double unfulfilled_demand, int week) {
    int week_of_year = Utils::weekOfTheYear(week);
    double unrestricted_price = weekly_average_volumetric_price[week_of_year];
    double current_price;

    // Clear yearly updated data collecting variables.
    if (week_of_year == 0) {
        insurance_purchase = 0.;
    } else if (week_of_year == 1) {
        infra_net_present_cost = 0.;
        current_debt_payment = 0.;
    }

    // Set current water price, contingent on restrictions being enacted.
    if (restricted_price == NON_INITIALIZED)
        current_price = unrestricted_price;
    else
        current_price = restricted_price;

    if (current_price < unrestricted_price)
        throw logic_error("Prices under surcharge cannot be smaller than "
                          "prices w/o restrictions enacted.");

    // calculate fund contributions if there were no shortage.
    double projected_fund_contribution = percent_contingency_fund_contribution *
                                         unrestricted_demand *
                                         unrestricted_price;

    // Calculate actual gross revenue.
    gross_revenue = restricted_demand * current_price;

    // Calculate losses due to restrictions and transfers.
    double lost_demand_vol_sales =
            (unrestricted_demand * (1 - demand_multiplier) +
             unfulfilled_demand);
    double revenue_losses = lost_demand_vol_sales * unrestricted_price;
    double transfer_costs = demand_offset * (offset_rate_per_volume -
                                             unrestricted_price);
    double recouped_loss_price_surcharge =
            restricted_demand * (current_price - unrestricted_price);

    // contingency fund cannot get negative.
    contingency_fund = max(contingency_fund + projected_fund_contribution -
                           revenue_losses - transfer_costs +
                           recouped_loss_price_surcharge,
                           0.0);

    // Update variables for data collection and next iteration.
    drought_mitigation_cost = max(revenue_losses + transfer_costs -
                                  insurance_payout -
                                  recouped_loss_price_surcharge,
                                  0.0);

    fund_contribution =
            projected_fund_contribution - revenue_losses - transfer_costs +
            recouped_loss_price_surcharge;

    resetDroughtMitigationVariables();

    // Calculate current debt payment to be made on that week (if first
    // week of year), if any.
    current_debt_payment = updateCurrent_debt_payment(week);
}

void Utility::resetDroughtMitigationVariables() {
    restricted_price = NON_INITIALIZED;
    offset_rate_per_volume = NONE;
    this->demand_offset = NONE;
}

void Utility::setWaterSourceOnline(unsigned int source_id, int week) {
    infrastructure_construction_manager.setWaterSourceOnline(
            source_id, week, utility_owned_wtp_capacities, water_source_to_wtp,
            total_storage_capacity, total_available_volume,
            total_stored_volume);

    updateTreatmentAndNumberOfStorageSources();
}


/**
 * Calculates total debt payments to be made in a week, if that's the first week
 * of the year.
 * @param week
 * @param debt_payment_streams
 * @return
 */
double Utility::updateCurrent_debt_payment(int week) {
    double current_debt_payment = 0;

    // Checks if it's the first week of the year, when outstanding debt
    // payments should be made.
    for (Bond *bond : issued_bonds) {
        current_debt_payment += bond->getDebtService(week);
    }

    return current_debt_payment;
}

void Utility::issueBond(int new_infra_triggered, int week) {
    if (new_infra_triggered != NON_INITIALIZED) {
        Bond &bond = water_sources.at((unsigned long) new_infra_triggered)
                ->getBond(id);
        if (!bond.isIssued()) {
            double construction_time = water_sources
                    .at((unsigned long) new_infra_triggered)->construction_time;
            bond.issueBond(week, (int) construction_time, bond_term_multiplier,
                           bond_interest_rate_multiplier);
            issued_bonds.push_back(&bond);
            infra_net_present_cost += bond.getNetPresentValueAtIssuance(
                    infra_discount_rate, week);
        }
    }
}

void Utility::forceInfrastructureConstruction(int week,
                                              vector<int> new_infra_triggered) {
    // Build all triggered infrastructure
    infrastructure_construction_manager.forceInfrastructureConstruction(week,
                                                                        new_infra_triggered);

    // Issue bonds for triggered infrastructure
    auto under_construction = infrastructure_construction_manager.getUnder_construction();
    for (int ws : new_infra_triggered) {
        if (under_construction.size() > ws &&
            under_construction.at((unsigned long) ws)) {
            issueBond(ws, week);
        }
    }
}

/**
 * Check if new infrastructure is to be triggered based on long-term risk of failure and, if so, handle
 * the beginning of construction, issue corresponding bonds and update debt.
 * @param long_term_rof
 * @param week
 * @return
 */
int Utility::infrastructureConstructionHandler(double long_term_rof, int week) {
    double past_year_average_demand = 0;
    if (week >= (int) WEEKS_IN_YEAR) {
        //     past_year_average_demand =
        //            std::accumulate(demand_series_realization.begin() + week - (int) WEEKS_IN_YEAR,
        //                            demand_series_realization.begin() + week, 0.0) / WEEKS_IN_YEAR;

        for (int w = week - (int) WEEKS_IN_YEAR; w < week; ++w) {
            past_year_average_demand += demand_series_realization.at(w) /
                    WEEKS_IN_YEAR;
        }
    }

    long_term_risk_of_failure = long_term_rof;

    // Check if new infrastructure is to be triggered and, if so, trigger it.
    int new_infra_triggered = infrastructure_construction_manager.infrastructureConstructionHandler(
            long_term_rof, week,
            past_year_average_demand,
            utility_owned_wtp_capacities,
            water_source_to_wtp,
            total_storage_capacity,
            total_available_volume,
            total_stored_volume);


    // Issue and add bond of triggered water source to list of outstanding bonds, and update total new
    // infrastructure NPV.
    issueBond(new_infra_triggered, week);

    updateTreatmentAndNumberOfStorageSources();

    return new_infra_triggered;
}

void Utility::calculateWastewater_releases(int week, double *discharges) {
    double discharge;
    waste_water_discharge = 0;

    for (int &id : wwtp_discharge_rule.discharge_to_source_ids) {
        discharge = restricted_demand * wwtp_discharge_rule
                .get_dependent_variable(id, Utils::weekOfTheYear(week));
        discharges[id] += discharge;

        waste_water_discharge += discharge;
    }
}

void Utility::addInsurancePayout(double payout_value) {
    contingency_fund += payout_value;
    insurance_payout = payout_value;
}

void Utility::purchaseInsurance(double insurance_price) {
    contingency_fund -= insurance_price;
    insurance_purchase = insurance_price;
}

void
Utility::setDemand_offset(double demand_offset, double offset_rate_per_volume) {
    this->demand_offset = demand_offset;
    this->offset_rate_per_volume = offset_rate_per_volume;
}

/**
 * Get time series corresponding to realization index and eliminate reference to
 * comprehensive demand data set.
 * @param r
 */
void
Utility::setRealization(unsigned long r, const vector<double> &rdm_factors) {
    unsigned long n_weeks = demands_all_realizations.at(r).size();
    demand_series_realization = vector<double>(n_weeks);

    // Apply demand multiplier and copy demands pertaining to current realization.
    double delta_demand =
            demands_all_realizations.at(r)[0] * (1. - rdm_factors.at(0));
    for (unsigned long w = 0; w < n_weeks; ++w) {
        demand_series_realization[w] = demands_all_realizations.at(r)[w] *
                                       rdm_factors.at(0)
                                       + delta_demand;
    }

    try {
        bond_term_multiplier = rdm_factors.at(1);
        bond_interest_rate_multiplier = rdm_factors.at(2);
        infra_discount_rate *= rdm_factors.at(3);

        // Set peaking demand factor.
        weekly_peaking_factor = calculateWeeklyPeakingFactor
                (&demands_all_realizations.at(r));

        price_rdm_multiplier = rdm_factors.at(4);
        for (double &awp : weekly_average_volumetric_price) {
            awp *= price_rdm_multiplier;
        }
    } catch (length_error &e) {
        char error[256];
        sprintf(error, "Utilities RDM file must have five columns but "
                       "has only %lu. The columns are for bond term "
                       "multiplier, bond interest rate multiplier, discount "
                       "rate multiplier, and tariff multiplier.",
                       rdm_factors.size());
        throw invalid_argument(error);
    }
}

vector<double> Utility::calculateWeeklyPeakingFactor(vector<double> *demands) {
    unsigned long n_weeks = (unsigned long) WEEKS_IN_YEAR + 1;
    int n_years = (int) (demands->size() / WEEKS_IN_YEAR - 1);
    vector<double> year_averages(n_weeks,
                                 0.0);

    double year_average_demand;
    for (int y = 0; y < n_years; ++y) {
        year_average_demand = accumulate(
                demands->begin() + y * WEEKS_IN_YEAR,
                demands->begin() + (y + 1) * WEEKS_IN_YEAR,
                0.0) /
                              ((int) ((y + 1) * WEEKS_IN_YEAR) -
                               (int) (y * WEEKS_IN_YEAR));
        for (unsigned long w = 0; w < n_weeks; ++w) {
            year_averages[w] += (*demands)[y * WEEKS_IN_YEAR + w] /
                                year_average_demand / n_years;
        }
    }

    return year_averages;
}

//========================= GETTERS AND SETTERS =============================//

bool Utility::hasTreatmentConnected(int ws) {
    return utility_owned_wtp_capacities[water_source_to_wtp[ws]] > 0.;
}

double Utility::getStorageToCapacityRatio() const {
    return total_stored_volume / total_storage_capacity;
}

double Utility::getTotal_available_volume() const {
    return total_available_volume;
}

double Utility::getTotal_stored_volume() const {
    return total_stored_volume;
}

double Utility::getTotal_storage_capacity() const {
    return total_storage_capacity;
}

double Utility::getRisk_of_failure() const {
    return short_term_risk_of_failure;
}

void Utility::setRisk_of_failure(double risk_of_failure) {
    this->short_term_risk_of_failure = risk_of_failure;
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

double Utility::getUnrestrictedDemand() const {
    return unrestricted_demand;
}

double Utility::getRestrictedDemand() const {
    return restricted_demand;
}

double Utility::getGrossRevenue() const {
    return gross_revenue;
}

double Utility::getDemand_multiplier() const {
    return demand_multiplier;
}

double Utility::getUnrestrictedDemand(int week) const {
    return demand_series_realization[week];
}

double Utility::getInfrastructure_net_present_cost() const {
    return infra_net_present_cost;
}

double Utility::getCurrent_debt_payment() const {
    return current_debt_payment;
}

double Utility::getCurrent_contingency_fund_contribution() const {
    return fund_contribution;
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

const vector<int> &Utility::getRof_infrastructure_construction_order()
const {
    return infrastructure_construction_manager.getRof_infra_construction_order();
}

const vector<int> &Utility::getDemand_infra_construction_order() const {
    return infrastructure_construction_manager.getDemand_infra_construction_order();
}

const vector<int> Utility::getInfrastructure_built() const {
    return infrastructure_construction_manager.getInfra_built_last_week();
}

double Utility::waterPrice(int week) {
    return weekly_average_volumetric_price[week];
}

void Utility::setRestricted_price(double restricted_price) {
    Utility::restricted_price = restricted_price * price_rdm_multiplier;
}

void Utility::setNoFinaicalCalculations() {
    used_for_realization = false;
}

double Utility::getLong_term_risk_of_failure() const {
    return long_term_risk_of_failure;
}

const vector<WaterSource *> &Utility::getWater_sources() const {
    return water_sources;
}

double Utility::getWaste_water_discharge() const {
    return waste_water_discharge;
}

void Utility::resetTotal_storage_capacity() {
    Utility::total_storage_capacity = 0;
}

double Utility::getUnfulfilled_demand() const {
    return unfulfilled_demand;
}

double Utility::getNet_stream_inflow() const {
    return net_stream_inflow;
}

const InfrastructureManager &
Utility::getInfrastructure_construction_manager() const {
    return infrastructure_construction_manager;
}

double Utility::getDemand_offset() const {
    return demand_offset;
}

double Utility::getInfraDiscountRate() const {
    return infra_discount_rate;
}
