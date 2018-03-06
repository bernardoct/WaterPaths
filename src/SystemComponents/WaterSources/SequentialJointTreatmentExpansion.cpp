//
// Created by bernardoct on 7/18/17.
//

#include <numeric>
#include "SequentialJointTreatmentExpansion.h"

SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(
        const char *name, const int id, const int parent_reservoir_ID,
        double total_treatment_capacity,
        const vector<double>& added_treatment_capacity_fractions,
        const double construction_rof_or_demand,
        const vector<double> &construction_time_range, double permitting_period,
        double construction_cost)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, NEW_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, construction_cost),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          added_treatment_capacity_fractions
                  (added_treatment_capacity_fractions),
          total_treatment_capacity(total_treatment_capacity),
          expansion_sequence_id(NON_INITIALIZED) {}


SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(
        const char *name, const int id, const int parent_reservoir_ID,
        const int expansion_sequence_id,
        double total_treatment_capacity,
        vector<vector<double>>& sequential_treatment_capacity,
        vector<vector<double>>& sequential_cost,
        const double construction_rof_or_demand,
        const vector<double> &construction_time_range, double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, NEW_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, NONE),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          total_treatment_capacity(total_treatment_capacity),
          sequential_treatment_capacity(sequential_treatment_capacity),
          sequential_cost(sequential_cost),
          expansion_sequence_id(expansion_sequence_id) {

    if (sequential_treatment_capacity.empty())
        __throw_invalid_argument("max_sequential_capacity is initialized "
                                         "within "
                                         "SequentialJointTreatmentExpansion "
                                         "and must be an empty vector.");
}

SequentialJointTreatmentExpansion::~SequentialJointTreatmentExpansion() = default;

/**
 * Copy constructor.
 * @param reservoir
 */
SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(
        const SequentialJointTreatmentExpansion &joint_water_treatment_plant) :
        WaterSource(joint_water_treatment_plant),
        parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID),
        total_treatment_capacity(
                joint_water_treatment_plant.total_treatment_capacity),
        expansion_sequence_id(
                joint_water_treatment_plant.expansion_sequence_id),
        sequential_treatment_capacity(
                joint_water_treatment_plant.sequential_treatment_capacity),
        sequential_cost(
                joint_water_treatment_plant.sequential_cost) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
SequentialJointTreatmentExpansion &SequentialJointTreatmentExpansion::operator=(
        const SequentialJointTreatmentExpansion &joint_water_treatment_plant) {
    WaterSource::operator=(joint_water_treatment_plant);
    return *this;
}

void SequentialJointTreatmentExpansion::applyContinuity(int week, double upstream_source_inflow,
                                                        double wastewater_discharge,
                                                        vector<double> &demand_outflow) {
    __throw_logic_error("Reservoir expansion only add storage volume to the "
                                "reservoir they're assigned to.  Continuity "
                                "cannot be called on it, but only on the "
                                "reservoir it's  assigned to expand.");
}

/**
 * Returns the capacity to be installed for a given utility and deducts it
 * from maximum planned expansion.
 * @param utility_id
 * @return
 */
double
SequentialJointTreatmentExpansion::implementTreatmentCapacity(int utility_id) {
    if (!sequential_treatment_capacity.empty()) {
        /// Build capacity of current and previous expansion stages.
        double capacity_to_build = accumulate(sequential_treatment_capacity[utility_id].begin(),
                                              sequential_treatment_capacity[utility_id].begin() +
                                              expansion_sequence_id + 1, 0.);
        /// Set capacity to be built of previous stages to 0 so that they are not built again.
        for (int i = 0; i <= expansion_sequence_id; ++i) {
            sequential_treatment_capacity[utility_id][i] = 0.;
        }

        return capacity_to_build;
    } else {
        return total_treatment_capacity;
    }
}


/**
 * Returns the price to be paid (no drama intended) for a given utility and
 * deducts it from maximum planned expansion.
 * @param utility_id
 * @return
 */
double SequentialJointTreatmentExpansion::calculateConstructionCost(int utility_id) {
    if (!sequential_cost.empty()) {
        /// Pay for capacity of current and previous expansion stages.
        double price_to_pay = accumulate(sequential_cost[utility_id].begin(),
                                         sequential_cost[utility_id].begin() +
                                         expansion_sequence_id + 1, 0.);
        /// Set cost of previous stages to 0 so that they are not built again.
        for (int i = 0; i <= expansion_sequence_id; ++i) {
            sequential_cost[utility_id][i] = 0.;
        }

        construction_cost_of_capital = price_to_pay;
        return price_to_pay;
    } else
        return construction_cost_of_capital;
}

double SequentialJointTreatmentExpansion::calculateNetPresentConstructionCost(
        int week, int utility_id, double discount_rate,
        double& level_debt_service_payment, double bond_term,
        double bond_interest_rate) const {
    double construction_cost_this_expansion = WaterSource::calculateNetPresentConstructionCost
            (week, utility_id, discount_rate, level_debt_service_payment,
             bond_term, bond_interest_rate);

    if (std::isnan(construction_cost_this_expansion))
        __throw_runtime_error("NPV calc error SJTE.");

    /// Return net present cost proportional to treatment allocation.
    return construction_cost_this_expansion;
}

bool SequentialJointTreatmentExpansion::skipConstruction() const {
    return sequential_treatment_capacity[0][expansion_sequence_id] == 0.;
}
