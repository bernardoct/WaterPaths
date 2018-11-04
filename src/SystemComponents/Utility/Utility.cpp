//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include <numeric>
#include <algorithm>
#include "Utility.h"
#include "../WaterSources/ReservoirExpansion.h"
#include "../WaterSources/SequentialJointTreatmentExpansion.h"
#include "../WaterSources/Relocation.h"
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
        const char *name, int id,
        vector<vector<double>>& demands_all_realizations,
        int number_of_week_demands,
        const double percent_contingency_fund_contribution,
        const vector<vector<double>> *typesMonthlyDemandFraction,
        const vector<vector<double>> *typesMonthlyWaterPrice,
        WwtpDischargeRule wwtp_discharge_rule,
        double demand_buffer) :
        total_storage_capacity(NONE),
        total_available_volume(NONE),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demands_all_realizations(demands_all_realizations),
        infra_discount_rate(NON_INITIALIZED),
        bond_term_multiplier(NON_INITIALIZED),
        bond_interest_rate_multiplier(NON_INITIALIZED),
        id(id),
        number_of_week_demands(number_of_week_demands),
        name(name),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        demand_buffer(demand_buffer) {

    calculateWeeklyAverageWaterPrices(typesMonthlyDemandFraction,
                                      typesMonthlyWaterPrice);


    infrastructure_manager = InfrastructureManager(id, vector<double>(),
                                                   vector<vector<int>>(), infra_discount_rate,
                                                   NON_INITIALIZED, NON_INITIALIZED,
                                                   vector<int>(),
                                                   vector<int>());

    infrastructure_manager.connectWaterSourcesVectorsToUtilitys(water_sources,
                                                                priority_draw_water_source,
                                                                non_priority_draw_water_source);
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
Utility::Utility(const char *name, int id, vector<vector<double>>& demands_all_realizations,
                 int number_of_week_demands, const double percent_contingency_fund_contribution,
                 const vector<vector<double>> *typesMonthlyDemandFraction,
                 const vector<vector<double>> *typesMonthlyWaterPrice,
                 WwtpDischargeRule wwtp_discharge_rule,
                 double demand_buffer, const vector<int> &rof_infra_construction_order,
                 const vector<int> &demand_infra_construction_order,
                 const vector<double> &infra_construction_triggers, double infra_discount_rate,
                 const vector<vector<int>>& infra_if_built_remove, double
                 bond_term, double bond_interest_rate) :
        total_storage_capacity(NONE),
        total_available_volume(NONE),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demands_all_realizations(demands_all_realizations),
        infra_discount_rate(infra_discount_rate),
        bond_term_multiplier(bond_term),
        bond_interest_rate_multiplier(bond_interest_rate),
        id(id),
        number_of_week_demands(number_of_week_demands),
        name(name),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        demand_buffer(demand_buffer) {

    infrastructure_manager =
            InfrastructureManager(id, infra_construction_triggers, infra_if_built_remove,
                                  infra_discount_rate, bond_term, bond_interest_rate,
                                  rof_infra_construction_order, demand_infra_construction_order);

    infrastructure_manager.connectWaterSourcesVectorsToUtilitys(water_sources,
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

    calculateWeeklyAverageWaterPrices(typesMonthlyDemandFraction,
                                      typesMonthlyWaterPrice);
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
 */
Utility::Utility(const char *name, int id, vector<vector<double>>& demands_all_realizations,
                 int number_of_week_demands, const double percent_contingency_fund_contribution,
                 const vector<vector<double>> *typesMonthlyDemandFraction,
                 const vector<vector<double>> *typesMonthlyWaterPrice, WwtpDischargeRule wwtp_discharge_rule,
                 double demand_buffer, const vector<int> &rof_infra_construction_order,
                 const vector<int> &demand_infra_construction_order,
                 const vector<double> &infra_construction_triggers, double infra_discount_rate, double bond_term,
                 double bond_interest_rate) :
        total_storage_capacity(NONE),
        total_available_volume(NONE),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demands_all_realizations(demands_all_realizations),
        infra_discount_rate(infra_discount_rate),
        bond_term_multiplier(bond_term),
        bond_interest_rate_multiplier(bond_interest_rate),
        id(id),
        number_of_week_demands(number_of_week_demands),
        name(name),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        demand_buffer(demand_buffer) {

    infrastructure_manager = InfrastructureManager(id, infra_construction_triggers,
                                                                vector<vector<int>>(), infra_discount_rate,
                                                                bond_term, bond_interest_rate,
                                                                rof_infra_construction_order,
                                                                demand_infra_construction_order);

    infrastructure_manager.connectWaterSourcesVectorsToUtilitys(water_sources,
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

    calculateWeeklyAverageWaterPrices(typesMonthlyDemandFraction,
                                      typesMonthlyWaterPrice);
}

Utility::Utility(Utility &utility) :
        weekly_average_volumetric_price(utility.weekly_average_volumetric_price),
        total_storage_capacity(utility.total_storage_capacity),
        total_available_volume(utility.total_available_volume),
        wwtp_discharge_rule(utility.wwtp_discharge_rule),
        demands_all_realizations(utility.demands_all_realizations),
        demand_series_realization(utility.demand_series_realization),
        infra_discount_rate(utility.infra_discount_rate),
        bond_term_multiplier(utility.bond_term_multiplier),
        bond_interest_rate_multiplier(utility.bond_interest_rate_multiplier),
        id(utility.id),
        number_of_week_demands(utility.number_of_week_demands),
        name(utility.name),
        percent_contingency_fund_contribution(utility.percent_contingency_fund_contribution),
        demand_buffer(utility.demand_buffer),
        infrastructure_manager(utility.infrastructure_manager) {

    infrastructure_manager.connectWaterSourcesVectorsToUtilitys(water_sources,
                                                                             priority_draw_water_source,
                                                                             non_priority_draw_water_source);

    /// Create copies of sources
    water_sources.clear();
}

Utility::~Utility() {
    water_sources.clear();
}

Utility &Utility::operator=(const Utility &utility) {

    demand_series_realization = vector<double>((unsigned long) utility.number_of_week_demands);

    infrastructure_manager.connectWaterSourcesVectorsToUtilitys(water_sources,
                                                                             priority_draw_water_source,
                                                                             non_priority_draw_water_source);

    /// Create copies of sources
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

/**
 * Calculates average water price from consumer types and respective prices.
 * @param typesMonthlyDemandFraction
 * @param typesMonthlyWaterPrice
 */
void Utility::calculateWeeklyAverageWaterPrices(
        const vector<vector<double>> *typesMonthlyDemandFraction,
        const vector<vector<double>> *typesMonthlyWaterPrice) {

    priceCalculationErrorChecking(typesMonthlyDemandFraction,
                                  typesMonthlyWaterPrice);

    weekly_average_volumetric_price = vector<double>((int) WEEKS_IN_YEAR + 1, 0.);
    double monthly_average_price[NUMBER_OF_MONTHS] = {};
    int n_tiers = static_cast<int>(typesMonthlyWaterPrice->at(0).size());

    /// Calculate monthly average prices across consumer types.
    for (int m = 0; m < NUMBER_OF_MONTHS; ++m)
        for (int t = 0; t < n_tiers; ++t)
            monthly_average_price[m] += (*typesMonthlyDemandFraction)[m][t] *
                                        (*typesMonthlyWaterPrice)[m][t];

    /// Create weekly price table from monthly prices.
    for (int w = 0; w < (int) (WEEKS_IN_YEAR + 1); ++w)
        weekly_average_volumetric_price[w] =
                monthly_average_price[(int) (w / WEEKS_IN_MONTH)] / 1e6;
}

/**
 * Checks price calculation input matrices for errors.
 * @param typesMonthlyDemandFraction
 * @param typesMonthlyWaterPrice
 */
void Utility::priceCalculationErrorChecking(
        const vector<vector<double>> *typesMonthlyDemandFraction,
        const vector<vector<double>> *typesMonthlyWaterPrice) {
    if (typesMonthlyDemandFraction->size() != NUMBER_OF_MONTHS)
        __throw_invalid_argument("There must be 12 total_demand fractions per tier.");
    if (typesMonthlyWaterPrice->size() != NUMBER_OF_MONTHS)
        __throw_invalid_argument("There must be 12 water prices per tier.");
    if ((*typesMonthlyWaterPrice)[0].size() !=
        (*typesMonthlyDemandFraction)[0].size())
        __throw_invalid_argument("There must be Demand fractions and water "
                                         "prices for the same number of tiers.");
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

    for (int ws : non_priority_draw_water_source) {
        double stored_volume = max(1.0e-6,
                                   water_sources[ws]->getAvailableAllocatedVolume(id));
        total_available_volume += stored_volume;
        total_stored_volume += stored_volume;
	    net_stream_inflow += water_sources[ws]->getAllocatedInflow(id);
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
    infrastructure_manager.addWaterSource(water_source);

    // If watersource is online and the utility owns some of its installed
    // treatment capacity, make it online.
    if (water_source->isOnline() && water_source->
            getAllocatedTreatmentCapacity(id) > 0) {
        infrastructure_manager.addWaterSourceToOnlineLists(
                water_source->id, total_storage_capacity,
                total_treatment_capacity, total_available_volume,
                total_stored_volume);
    }

    n_sources++;
    max_capacity += water_source->getAllocatedCapacity(id);
}

void Utility::checkErrorsAddWaterSourceOnline(WaterSource *water_source) {
    for (WaterSource *ws : water_sources) {
        if ((ws != nullptr) && ws->id == water_source->id) {
            cout << "Water source ID: " << water_source->id << endl <<
                 "Utility ID: " << id << endl;
            __throw_invalid_argument("Attempt to add water source with "
                                     "duplicate ID to utility.");
        }
    }
}

/**
 * Splits demands among sources. Demand is allocated so that river intakes
 * and reuse are first used to their capacity before requesting water from
 * allocations in reservoirs.
 * @param week
 */
void Utility::splitDemands(
        int week, vector<vector<double>> &demands,
        bool apply_demand_buffer) {
    unrestricted_demand = demand_series_realization[week] +
                          apply_demand_buffer * demand_buffer *
                          weekly_peaking_factor[Utils::weekOfTheYear(week)];
    restricted_demand = unrestricted_demand * demand_multiplier - demand_offset;
    unfulfilled_demand = max(max(restricted_demand - total_available_volume,
                                 restricted_demand - total_treatment_capacity), 0.);
    restricted_demand -= unfulfilled_demand;

    /// Allocates demand to intakes and reuse based on allocated volume to
    /// this utility.
    for (int &ws : priority_draw_water_source) {
        double source_demand =
                min(restricted_demand,
                    water_sources[ws]->getAvailableAllocatedVolume(id));
        demands[ws][this->id] = source_demand;
    }

    /// Allocates remaining demand to reservoirs based on allocated available
    /// volume to this utility.
    unsigned short over_allocated_sources = 0;
    double over_allocated_volume = 0;
    double demand_fraction[water_sources.size()];
    int not_over_allocated_ids[water_sources.size()];
    double sum_not_alloc_demand_fraction = 0;
    unsigned short not_over_allocated_sources = 0;
    for (int &ws : non_priority_draw_water_source) {
        auto source = water_sources[ws];

        /// Calculate allocation based on sources' available volumes.
        demand_fraction[ws] =
                max(1.0e-6,
                    source->getAvailableAllocatedVolume(id) /
                    total_available_volume);

        /// Calculate demand allocated to a given source.
        double source_demand = restricted_demand * demand_fraction[ws];
        demands[ws][id] = source_demand;

        /// Check if allocated demand was greater than treatment capacity.
        double over_allocated_demand_ws =
                source_demand - source->getAllocatedTreatmentCapacity(id);

        /// Set reallocation variables for the sake of reallocating demand.
        if (over_allocated_demand_ws > 0.) {
            over_allocated_sources++;
            over_allocated_volume += over_allocated_demand_ws;
            demands[ws][id] = source_demand - over_allocated_demand_ws;
        } else {
            not_over_allocated_ids[not_over_allocated_sources] = ws;
            sum_not_alloc_demand_fraction += demand_fraction[ws];
            not_over_allocated_sources++;
        }
    }

    /// Do one iteration of demand reallocation among sources whose treatment
    /// capacities have not yet been exceeded if there is an instance of
    /// overallocation.
    if (over_allocated_sources > 0) {		            
        for (int i = 0; i < not_over_allocated_sources; ++i) {
            int ws = not_over_allocated_ids[i];
            demands[ws][id] += over_allocated_volume *
                               demand_fraction[ws] / sum_not_alloc_demand_fraction;
        }
    }

    /// Update contingency fund
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
void Utility::updateContingencyFundAndDebtService(
        double unrestricted_demand, double demand_multiplier,
        double demand_offset, double unfulfilled_demand, int week) {

    int week_of_year = Utils::weekOfTheYear(week);
    double unrestricted_price = weekly_average_volumetric_price[week_of_year];
    double current_price;

    /// Clear yearly updated data collecting variables.
    if (week_of_year == 0) {
        insurance_purchase = 0.;
    } else if (week_of_year == 1) {
        infra_net_present_cost = 0.;
        current_debt_payment = 0.;
    }

    /// Set current water price, contingent on restrictions being enacted.
    if (restricted_price == NON_INITIALIZED)
        current_price = unrestricted_price;
    else
        current_price = restricted_price;

    if (current_price < unrestricted_price)
        __throw_logic_error("Prices under surcharge cannot be smaller than "
                                    "prices w/o restrictions enacted.");

    /// calculate fund contributions if there were no shortage.
    double projected_fund_contribution = percent_contingency_fund_contribution *
                                         unrestricted_demand *
                                         unrestricted_price;

    /// Calculate actual gross revenue.
    gross_revenue = restricted_demand * current_price;

    /// Calculate losses due to restrictions and transfers.
    double lost_demand_vol_sales =
            (unrestricted_demand * (1 - demand_multiplier) +
             unfulfilled_demand);
    double revenue_losses = lost_demand_vol_sales * unrestricted_price;
    double transfer_costs = demand_offset * (offset_rate_per_volume -
                                             unrestricted_price);
    double recouped_loss_price_surcharge =
            restricted_demand * (current_price - unrestricted_price);

    /// contingency fund cannot get negative.
    contingency_fund = max(contingency_fund + projected_fund_contribution -
                           revenue_losses - transfer_costs +
                           recouped_loss_price_surcharge,
                           0.0);

    /// Update variables for data collection and next iteration.
    drought_mitigation_cost = max(revenue_losses + transfer_costs -
                                  insurance_payout -
                                  recouped_loss_price_surcharge,
                                  0.0);

    fund_contribution =
            projected_fund_contribution - revenue_losses - transfer_costs +
            recouped_loss_price_surcharge;

    restricted_price = NON_INITIALIZED;
    offset_rate_per_volume = NONE;
    this->demand_offset = NONE;

    /// Calculate current debt payment to be made on that week (if first
    /// week of year), if any.
    current_debt_payment = updateCurrent_debt_payment(week);
}

void Utility::setWaterSourceOnline(unsigned int source_id, int week) {
    infrastructure_manager.setWaterSourceOnline(source_id, week, total_storage_capacity,
                                                             total_treatment_capacity, total_available_volume,
                                                             total_stored_volume);
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

    /// Checks if it's the first week of the year, when outstanding debt
    /// payments should be made.
    for (Bond *bond : issued_bonds) {
        current_debt_payment += bond->getDebtService(week);
    }

    return current_debt_payment;
}

void Utility::issueBond(int new_infra_triggered, int week) {
    if (new_infra_triggered != NON_INITIALIZED) {
        Bond &bond = water_sources.at((unsigned long) new_infra_triggered)
                ->getBond(id);
        double construction_time = water_sources
                .at((unsigned long) new_infra_triggered)->construction_time;
        bond.issueBond(week, (int) construction_time, bond_term_multiplier,
                       bond_interest_rate_multiplier);
        issued_bonds.push_back(&bond);
        infra_net_present_cost += bond.getNetPresentValueAtIssuance(
                infra_discount_rate, week);
    }
}

void Utility::forceInfrastructureConstruction(int week, vector<int> new_infra_triggered) {
    /// Build all triggered infrastructure
    infrastructure_manager.forceInfrastructureConstruction(week, new_infra_triggered);

    /// Issue bonds for triggered infrastructure
    auto under_construction = infrastructure_manager.getUnder_construction();
    for (int ws : new_infra_triggered) {
        if (under_construction.size() > ws && under_construction.at((unsigned long) ws)) {
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
            past_year_average_demand += demand_series_realization.at(w);
        }
    }

    long_term_risk_of_failure = long_term_rof;

    /// Check if new infrastructure is to be triggered and, if so, trigger it.
    int new_infra_triggered = infrastructure_manager.infrastructureConstructionHandler(long_term_rof, week,
                                                                                                    past_year_average_demand,
                                                                                                    total_storage_capacity,
                                                                                                    total_treatment_capacity,
                                                                                                    total_available_volume,
                                                                                                    total_stored_volume);

    /// Issue and add bond of triggered water source to list of outstanding bonds, and update total new
    /// infrastructure NPV.
    issueBond(new_infra_triggered, week);

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
void Utility::setRealization(unsigned long r, vector<double>& rdm_factors) {
    unsigned long n_weeks = demands_all_realizations.at(r).size();
    demand_series_realization = vector<double>(n_weeks);

    /// Apply demand multiplier and copy demands pertaining to current realization.
    double delta_demand = demands_all_realizations.at(r)[0] * (1. -
            rdm_factors.at(0));
    for (unsigned long w = 0; w < n_weeks; ++w) {
        demand_series_realization[w] = demands_all_realizations.at(r)[w] *
                                               rdm_factors.at(0)
                                       + delta_demand;
    }

    bond_term_multiplier = rdm_factors.at(1);
    bond_interest_rate_multiplier = rdm_factors.at(2);
    infra_discount_rate *= rdm_factors.at(3);

    /// Set peaking demand factor.
    weekly_peaking_factor = calculateWeeklyPeakingFactor
            (&demands_all_realizations.at(r));
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
    return infrastructure_manager.getRof_infra_construction_order();
}

const vector<int> &Utility::getDemand_infra_construction_order() const {
    return infrastructure_manager.getDemand_infra_construction_order();
}

const vector<int> Utility::getInfrastructure_built() const {
    return infrastructure_manager.getInfra_built_last_week();
}

double Utility::waterPrice(int week) {
    return weekly_average_volumetric_price[week];
}

void Utility::setRestricted_price(double restricted_price) {
    Utility::restricted_price = restricted_price;
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

const InfrastructureManager &Utility::getInfrastructure_construction_manager() const {
    return infrastructure_manager;
}
