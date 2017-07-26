//
// Created by bernardo on 1/13/17.
//

#include <iostream>
#include <algorithm>
#include <numeric>
#include "Utility.h"
#include "../Utils/Utils.h"
#include "WaterSources/ReservoirExpansion.h"
#include "WaterSources/SequentialJointTreatmentExpansion.h"
#include "WaterSources/Relocation.h"

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
        vector<vector<double>> *demands_all_realizations,
        int number_of_week_demands,
        const double percent_contingency_fund_contribution,
        const vector<vector<double>> *typesMonthlyDemandFraction,
        const vector<vector<double>> *typesMonthlyWaterPrice,
        WwtpDischargeRule wwtp_discharge_rule,
        double demand_buffer) :
        name(name), id(id), demands_all_realizations(demands_all_realizations),
        number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution(percent_contingency_fund_contribution),
        infra_discount_rate(NON_INITIALIZED),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demand_buffer(demand_buffer),
        total_stored_volume(NONE),
        total_storage_capacity(NONE) {

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
Utility::Utility(
        const char *name, int id,
        vector<vector<double>> *demands_all_realizations,
        int number_of_week_demands,
        const double percent_contingency_fund_contribution,
        const vector<vector<double>> *typesMonthlyDemandFraction,
        const vector<vector<double>> *typesMonthlyWaterPrice,
        WwtpDischargeRule wwtp_discharge_rule,
        double demand_buffer,
        const vector<int> &rof_infra_construction_order,
        const vector<int> &demand_infra_construction_order,
        double infra_discount_rate, const vector<vector<int>>
        *infra_if_built_remove) :
        name(name), id(id), demands_all_realizations(demands_all_realizations),
        number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution
                (percent_contingency_fund_contribution),
        rof_infra_construction_order(rof_infra_construction_order),
        demand_infra_construction_order(demand_infra_construction_order),
        infra_discount_rate(infra_discount_rate),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demand_buffer(demand_buffer),
        total_stored_volume(NONE),
        total_storage_capacity(NONE),
        infra_if_built_remove(infra_if_built_remove) {

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

Utility::Utility(
        const char *name, int id,
        vector<vector<double>> *demands_all_realizations,
        int number_of_week_demands,
        const double percent_contingency_fund_contribution,
        const vector<vector<double>> *typesMonthlyDemandFraction,
        const vector<vector<double>> *typesMonthlyWaterPrice,
        WwtpDischargeRule wwtp_discharge_rule,
        double demand_buffer,
        const vector<int> &rof_infra_construction_order,
        const vector<int> &demand_infra_construction_order,
        double infra_discount_rate) :
        name(name), id(id), demands_all_realizations(demands_all_realizations),
        number_of_week_demands(number_of_week_demands),
        percent_contingency_fund_contribution
                (percent_contingency_fund_contribution),
        rof_infra_construction_order(rof_infra_construction_order),
        demand_infra_construction_order(demand_infra_construction_order),
        infra_discount_rate(infra_discount_rate),
        wwtp_discharge_rule(wwtp_discharge_rule),
        demand_buffer(demand_buffer),
        total_stored_volume(NONE),
        total_storage_capacity(NONE),
        infra_if_built_remove(new vector<vector<int>>()) {

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
        id(utility.id), number_of_week_demands(utility.number_of_week_demands),
        total_storage_capacity(utility.total_storage_capacity),
        total_stored_volume(utility.total_stored_volume),
        demand_series(new double[utility.number_of_week_demands]),
        percent_contingency_fund_contribution(utility.percent_contingency_fund_contribution),
        weekly_average_volumetric_price(utility.weekly_average_volumetric_price),
        rof_infra_construction_order(utility.rof_infra_construction_order),
        demand_infra_construction_order(utility.demand_infra_construction_order),
        infra_discount_rate(utility.infra_discount_rate),
        demands_all_realizations(utility.demands_all_realizations),
        wwtp_discharge_rule(utility.wwtp_discharge_rule),
        demand_buffer(utility.demand_buffer),
        infra_if_built_remove(utility.infra_if_built_remove) {

    /// Create copies of sources
    water_sources.clear();
}

Utility::~Utility() {
    delete[] demand_series;
    delete[] weekly_average_volumetric_price;
}

Utility &Utility::operator=(const Utility &utility) {

    demand_series = new double[utility.number_of_week_demands];

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

    weekly_average_volumetric_price = new double[(int) WEEKS_IN_YEAR + 1]();
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
                monthly_average_price[(int) (w / WEEKS_IN_MONTH)];
}

/**
 * Checks price calculation input matrixes for errors.
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
void Utility::updateTotalStoredVolume() {
    total_stored_volume = 0.0;

    for (int ws : priority_draw_water_source)
        total_stored_volume +=
                max(1.0e-6,
                    water_sources[ws]->getAvailableAllocatedVolume(this->id));

    for (int ws : non_priority_draw_water_source)
        total_stored_volume +=
                max(1.0e-6,
                    water_sources[ws]->getAvailableAllocatedVolume(this->id));
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

    if (water_source->id > (int) water_sources.size() - 1)
        water_sources.resize((unsigned long) (water_source->id + 1));
    water_sources[water_source->id] = water_source;

    if (water_source->isOnline() &&
        water_source->getAllocatedTreatmentCapacity(id) > 0)
        addWaterSourceToOnlineLists(water_source->id);
}

void Utility::addWaterSourceToOnlineLists(int source_id) {

    /// Add capacities to utility.
    total_storage_capacity +=
            water_sources[source_id]->getAllocatedCapacity(id);
    total_treatment_capacity +=
            water_sources[source_id]->getAllocatedTreatmentCapacity(id);
    total_stored_volume = total_storage_capacity;

    /// Add source to the corresponding list of online water sources.
    if ((water_sources[source_id]->source_type == INTAKE ||
         water_sources[source_id]->source_type == WATER_REUSE))
        priority_draw_water_source.push_back(source_id);
    else
        non_priority_draw_water_source.push_back(source_id);
}

/**
 * Splits demands among sources. Demand is allocated so that river intakes
 * and reuse are first used to their capacity before requesting water from
 * allocations in reservoirs.
 * @param week
 */
void Utility::splitDemands(
        int week, vector<vector<double>> *demands,
        bool apply_demand_buffer) {
    unrestricted_demand = demand_series[week] +
                          apply_demand_buffer * demand_buffer *
                          weekly_peaking_factor[Utils::weekOfTheYear(week)];
    restricted_demand = unrestricted_demand * demand_multiplier - demand_offset;
    double unallocated_reservoirs_demand = restricted_demand;
    bool over_allocation_ws[water_sources.size()] = {};

    /// Allocates demand to intakes and reuse based on allocated volume to
    /// this utility.
    for (int ws : priority_draw_water_source) {
        double source_demand =
                min(restricted_demand,
                    water_sources[ws]->getAvailableAllocatedVolume(id));
        (*demands)[ws][this->id] = source_demand;
    }

    /// Allocates remaining demand to reservoirs based on allocated available
    /// volume to this utility.
    bool over_allocation = false;
    double remaining_stored_volume = total_stored_volume;
    for (int ws : non_priority_draw_water_source) {
        /// Calculate allocation based on sources' available volumes.
        double demand_fraction =
                max(1.0e-6,
                    water_sources[ws]->getAvailableAllocatedVolume(id) /
                    total_stored_volume);

        /// Calculate demand allocated to a given source.
        double source_demand = restricted_demand * demand_fraction;
        (*demands)[ws][id] = source_demand;

        /// Check if allocated demand was greater than treatment capacity.
        double over_allocated_demand_ws =
                max(source_demand -
                    water_sources[ws]->getAllocatedTreatmentCapacity(id),
                    0.);

        /// Set reallocation variables for the sake of reallocating demand.
        if (over_allocated_demand_ws > 0.) {
            over_allocation = true;
            over_allocation_ws[ws] = true;
            remaining_stored_volume -=
                    water_sources[ws]->getAvailableAllocatedVolume(id);
            (*demands)[ws][id] = source_demand - over_allocated_demand_ws;
        }

        unallocated_reservoirs_demand -=
                source_demand - over_allocated_demand_ws;
    }

    /// Do one iteration of demand reallocation among sources whose treatment
    /// capacities have not yet been exceeded if there is an instance of
    /// overallocation.
    double unallocated_reservoirs_demand_aux = unallocated_reservoirs_demand;
    if (over_allocation && remaining_stored_volume > 0)
        for (int ws : non_priority_draw_water_source)
            if (!over_allocation_ws[ws]) {
                double demand_fraction = max(1.0e-6,
                            water_sources[ws]->getAvailableAllocatedVolume(id) /
                            remaining_stored_volume);
                double extra_demand_share = demand_fraction *
                                            unallocated_reservoirs_demand_aux;
                (*demands)[ws][id] += extra_demand_share;
                unallocated_reservoirs_demand -= extra_demand_share;
            }

    /// Update contingency fund
    if (used_for_realization)
        updateContingencyFund(unrestricted_demand,
                              demand_multiplier,
                              demand_offset,
                              unallocated_reservoirs_demand,
                              week);

    //FIXME: IMPROVE CONTINUITY ERROR CHECKING HERE TO DETECT POORLY SPLIT DEMANDS WITHOUT CAPTURING UNFULFILLED DEMAND DUE TO LACK OF STORED WATER.
//    if (unallocated_reservoirs_demand > 1e-4)
//        __throw_logic_error("There is demand not being allocated to a water "
//                                    "source when the demand is being split "
//                                    "among water sources.");
}

void Utility::resetDataColletionVariables() {
    /// Reset demand multiplier, offset from transfers and insurance payout and
    /// price.
        demand_multiplier = 1;
        demand_offset = 0;
        insurance_payout = 0;
        insurance_purchase = 0;
        drought_mitigation_cost = 0;
        infra_built_last_week = {};
        fund_contribution = 0;
        gross_revenue = 0;
        current_debt_payment = 0;
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
void Utility::updateContingencyFund(
        double unrestricted_demand, double demand_multiplier,
        double demand_offset, double unfulfilled_demand, int week) {

    int week_of_year = Utils::weekOfTheYear(week);
    double unrestricted_price = weekly_average_volumetric_price[week_of_year];
    double current_price;

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
}

void Utility::setWaterSourceOnline(unsigned int source_id) {

    /// Sets water source online and add its ID to appropriate
    /// priority/non-priority ID vector. If reservoir expansion, add its
    /// capacity to the corresponding existing reservoir.
    if (water_sources.at(source_id)->source_type == NEW_WATER_TREATMENT_PLANT) {
        waterTreatmentPlantConstructionHandler(source_id);
    } else if (water_sources.at(source_id)->source_type ==
               RESERVOIR_EXPANSION) {
        reservoirExpansionConstructionHandler(source_id);
    } else if (water_sources.at(source_id)->source_type == SOURCE_RELOCATION) {
        sourceRelocationConstructionHandler(source_id);
    } else {
        water_sources.at(source_id)->setOnline();
        addWaterSourceToOnlineLists(source_id);
    }
    /// Updates total storage and treatment variables.
    total_storage_capacity = 0;
    total_treatment_capacity = 0;
    total_stored_volume = 0;
    for (WaterSource *ws : water_sources)
        if (ws &&
            (find(priority_draw_water_source.begin(),
                  priority_draw_water_source.end(),
                  ws->id) !=
             priority_draw_water_source.end() ||
             find(non_priority_draw_water_source.begin(),
                  non_priority_draw_water_source.end(),
                  ws->id) !=
             non_priority_draw_water_source.end())) {
            total_storage_capacity += ws->getAllocatedCapacity(id);
            total_treatment_capacity += ws->getAllocatedTreatmentCapacity(id);
            total_stored_volume += ws->getAvailableAllocatedVolume(id);
        }

}

void Utility::waterTreatmentPlantConstructionHandler(unsigned int source_id) {
    auto wtp = dynamic_cast<SequentialJointTreatmentExpansion *>
    (water_sources.at(source_id));

    /// Add treatment capacity to source
    double added_capacity = wtp->implementTreatmentCapacity(id);
    water_sources.at(wtp->parent_reservoir_ID)
            ->addTreatmentCapacity(
                    added_capacity,
                    wtp->added_treatment_capacity_fractions
                            ->at((unsigned long) id),
                    id);

    /// If source is intake or reuse and is not in the list of active
    /// sources, add it to the priority list.
    /// If source is not intake or reuse and is not in the list of active
    /// sources, add it to the non-priority list.
    bool is_priority_source =
            water_sources.at(wtp->parent_reservoir_ID)->source_type == INTAKE ||
            water_sources.at(wtp->parent_reservoir_ID)->source_type ==
            WATER_REUSE;
    bool is_not_in_priority_list =
            find(priority_draw_water_source.begin(),
                 priority_draw_water_source.end(),
                 wtp->parent_reservoir_ID)
            == priority_draw_water_source.end();
    bool is_not_in_non_priority_list =
            find(non_priority_draw_water_source.begin(),
                 non_priority_draw_water_source.end(),
                 wtp->parent_reservoir_ID)
            == non_priority_draw_water_source.end();

    /// Finally, the checking.
    if (is_priority_source && is_not_in_priority_list) {
        priority_draw_water_source.push_back((int) wtp->parent_reservoir_ID);
        total_storage_capacity +=
                water_sources.at(wtp->parent_reservoir_ID)
                        ->getAllocatedCapacity(id);
    } else if (is_not_in_non_priority_list) {
        non_priority_draw_water_source
                .push_back((int) wtp->parent_reservoir_ID);
    }
}

void Utility::reservoirExpansionConstructionHandler(unsigned int source_id) {
    ReservoirExpansion re =
            *dynamic_cast<ReservoirExpansion *>(water_sources.at(source_id));

    water_sources.at(re.parent_reservoir_ID)->
            addCapacity(re.getAllocatedCapacity(id));
}

void Utility::sourceRelocationConstructionHandler(unsigned int source_id) {
    Relocation re =
            *dynamic_cast<Relocation *>(water_sources.at(source_id));

    const vector<double> *new_allocated_fractions = re.new_allocated_fractions;

    water_sources.at(re.parent_reservoir_ID)->
            resetAllocations(new_allocated_fractions);
}

/**
 * Force utility to build a certain infrastructure option, if present in its
 * queue.
 * @param week
 * @param new_infra_triggered
 */
void Utility::forceInfrastructureConstruction(
        int week, vector<int> new_infra_triggered) {
    for (int ws : new_infra_triggered) {
        /// Variable storing the position of an infrastructure option in the
        /// queue of infrastructure to be built.
        auto it = find(rof_infra_construction_order.begin(),
                       rof_infra_construction_order.end(),
                       ws);
        /// If one of the infrastructure options built this year by one of
        /// the other utilities is in this utility's queue, force it to be
        /// triggered.
        if (it != rof_infra_construction_order.end()) {
            /// Bring new_infra_triggered from its current position in the
            /// queue to the first position.
            rof_infra_construction_order.erase(it);
            rof_infra_construction_order.insert(
                    rof_infra_construction_order.begin(),
                    (unsigned int) ws);
            /// Force infrastructure option new_infra_triggered to be built.
            beginConstruction(week,
                              ws);
        }
    }
}

/**
 * Checks if infrastructure should be built, triggers construction, sets it
 * online, and handles accounting/financing of it.
 * @param long_term_rof
 * @param week
 */
int Utility::infrastructureConstructionHandler(double long_term_rof, int week) {

    int new_infra_triggered = NON_INITIALIZED;
    long_term_risk_of_failure = long_term_rof;

    /// Checks whether the long-term ROF has been exceeded for the next
    /// infrastructure option in the list and, if not already under
    /// construction, starts building it.
    if (!rof_infra_construction_order.empty() &&
        !under_construction) { // if there is anything to be built

        /// Checks if ROF threshold for next infrastructure in line has been
        /// reached and if there is already infrastructure being built.
        int next_construction = rof_infra_construction_order[0];
        if (!under_construction &&
            long_term_rof > water_sources.at((unsigned long) next_construction)
                    ->construction_rof_or_demand) {
            new_infra_triggered = next_construction;

            if (infra_if_built_remove)
                for (auto v : *infra_if_built_remove) {
                    if (v[0] == next_construction)
                        for (int i = 0; i < v.size(); ++i) {
                            rof_infra_construction_order.erase(
                                    std::remove(
                                            rof_infra_construction_order
                                                    .begin(),
                                            rof_infra_construction_order.end(),
                                            v[i]),
                                    rof_infra_construction_order.end());
                        }
                }

            /// Begin construction.
            beginConstruction(week,
                              rof_infra_construction_order[0]);
        }
    }

    /// Checks whether the target demand has been exceeded for the next
    /// infrastructure option in the list and, if not already under
    /// construction, starts building it.
    if (!demand_infra_construction_order.empty() &&
        !under_construction &&
        week > WEEKS_IN_YEAR) { // if there is anything to be built

        double average_demand =
                std::accumulate(demand_series + week - (int) WEEKS_IN_YEAR,
                                demand_series + week,
                                0.) / WEEKS_IN_YEAR;

        /// Checks if demand threshold for next infrastructure in line has been
        /// reached and if there is already infrastructure being built.
        if (!under_construction && !demand_infra_construction_order.empty() &&
            average_demand >
            water_sources[demand_infra_construction_order[0]]
                    ->construction_rof_or_demand) {
            new_infra_triggered = demand_infra_construction_order[0];

            /// Begin construction.
            beginConstruction(week,
                              demand_infra_construction_order[0]);
        }
    }

    /// If there's a water source under construction, check if it's ready
    /// and, if so, clear it from the to-build list.
    if (under_construction && week > construction_end_date) {
        setWaterSourceOnline((unsigned int) under_construction_id);

        /// Record ID of and when infrastructure option construction was
        /// completed.
        infra_built_last_week = {id, week, under_construction_id};

        /// Erase infrastructure option from either vector of infrastructure to
        /// be built.
        if (!rof_infra_construction_order.empty()
            && rof_infra_construction_order[0] ==
               under_construction_id)
            rof_infra_construction_order.erase(
                    rof_infra_construction_order.begin());
        else if (!demand_infra_construction_order.empty()
                 && demand_infra_construction_order[0] == under_construction_id)
            demand_infra_construction_order.erase
                    (demand_infra_construction_order.begin());
        else
            __throw_logic_error("Infrastructure option whose construction was"
                                        " complete is not in the demand or "
                                        "rof triggered construction lists.");

        under_construction = false;
        under_construction_id = NON_INITIALIZED;
    }

    /// Calculate current annual debt payment to be made on that week (it first
    /// week of year), if any.
    current_debt_payment = updateCurrent_debt_payment(week);

    return new_infra_triggered;
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
    if (Utils::isFirstWeekOfTheYear(week)) {
        /// Checks if there's outstanding debt.
        if (!debt_payment_streams.empty()) {
            /// Iterate over all outstanding debt.
            for (vector<double> &pmt : debt_payment_streams) {
                if (!pmt.empty()) {
                    /// Adds outstanding debt payment to total current annual
                    /// payment.
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

    /// Add water source construction cost to the books.
    double level_debt_service_payments;
    infra_net_present_cost +=
            water_sources[infra_id]->
                    calculateNetPresentConstructionCost(
                    week,
                    id,
                    infra_discount_rate,
                    &level_debt_service_payments);

    /// Create stream of level debt service payments for water source.
    debt_payment_streams.emplace_back(
            (unsigned long) (water_sources[infra_id]->bond_term),
            level_debt_service_payments);

    /// Make water utility as under construction and determine construction
    /// end date (I wish that was how it worked in real constructions...)
    under_construction = true;
    under_construction_id = infra_id;

    construction_end_date =
            week + (int) water_sources[infra_id]->construction_time;
}

void Utility::calculateWastewater_releases(int week, double *discharges) {

    for (int id : *wwtp_discharge_rule.discharge_to_source_ids) {
        waste_water_discharge = restricted_demand * wwtp_discharge_rule
                .get_dependent_variable(id,
                                        Utils::weekOfTheYear(week));
        discharges[id] = waste_water_discharge;
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
    Utility::demand_offset = demand_offset;
    Utility::offset_rate_per_volume = offset_rate_per_volume;
}

/**
 * Get time series corresponding to realization index and eliminate reference to
 * comprehensive demand data set.
 * @param r
 */
void Utility::setRealization(unsigned long r) {
    int n_weeks = static_cast<int>(demands_all_realizations->at(r).size());
    demand_series = new double[n_weeks];
    std::copy(demands_all_realizations->at(r).begin(),
              demands_all_realizations->at(r).end(),
              demand_series);

    weekly_peaking_factor = calculateWeeklyPeakingFactor
            (&demands_all_realizations->at(r));

//    demands_all_realizations = nullptr;
}

vector<double> Utility::calculateWeeklyPeakingFactor(vector<double> *demands) {
    unsigned long n_weeks = (unsigned long) WEEKS_IN_YEAR + 1;
    int n_years = (int) (demands->size() / WEEKS_IN_YEAR - 1);
    vector<double> year_averages(n_weeks,
                                 0.);

    double year_average_demand;
    for (int y = 0; y < n_years; ++y) {
        year_average_demand = accumulate(
                demands->begin() + y * WEEKS_IN_YEAR,
                demands->begin() + (y + 1) * WEEKS_IN_YEAR,
                0.) /
                              ((int) ((y + 1) * WEEKS_IN_YEAR) -
                               (int) (y * WEEKS_IN_YEAR));
        for (int w = 0; w < n_weeks; ++w) {
            year_averages[w] += (*demands)[y * WEEKS_IN_YEAR + w] /
                                year_average_demand / n_years;
        }
    }

    return year_averages;
}

void Utility::checkErrorsAddWaterSourceOnline(WaterSource *water_source) {
    for (WaterSource *ws : water_sources)
        if ((ws != nullptr) && ws->id == water_source->id) {
            cout << "Water source ID: " << water_source->id << endl <<
                 "Utility ID: " << id << endl;
            __throw_invalid_argument("Attempt to add water source with "
                                             "duplicate ID to utility.");
        }

    /// Catch if user entered a water source as to be built but didn't enter
    /// bond parameters.
    for (int ws : rof_infra_construction_order)
        if (water_source->id == ws)
            if (water_source->construction_cost_of_capital == NON_INITIALIZED) {
                cout << "Utility " << id << " set to build water source " <<
                     ws << ", which has no bond parameters set." << endl;
                __throw_invalid_argument("Water source with no bond parameters "
                                                 "but set as to be built.");
            }
}

//========================= GETTERS AND SETTERS =============================//

double Utility::getStorageToCapacityRatio() const {
    return total_stored_volume / total_storage_capacity;
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
    return demand_series[week];
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
    return rof_infra_construction_order;
}

const vector<int> &Utility::getDemand_infra_construction_order() const {
    return demand_infra_construction_order;
}

const vector<int> Utility::getInfrastructure_built() const {
    return infra_built_last_week;
}

const double Utility::waterPrice(int week) {
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
