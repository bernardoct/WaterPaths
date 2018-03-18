//
// Created by bernardoct on 7/18/17.
//

#include <numeric>
#include "SequentialJointTreatmentExpansion.h"

/**
 *
 * @param name
 * @param id
 * @param parent_reservoir_ID
 * @param expansion_sequence_id
 *
 * @param sequential_treatment_capacity
 * @param sequential_cost
 * @param construction_time_range
 * @param permitting_period
 */
SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(const char *name, const int id,
                                                                     const int parent_reservoir_ID,
                                                                     const int expansion_sequence_id,
                                                                     vector<int> connected_sources,
                                                                     vector<double> &sequential_treatment_capacity,
                                                                     vector<double> &sequential_cost,
                                                                     const vector<double> &construction_time_range,
                                                                     double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, connected_sources, NEW_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, NONE),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          added_treatment_capacities(sequential_treatment_capacity),
          construction_costs(sequential_cost),
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
        added_treatment_capacities(joint_water_treatment_plant.added_treatment_capacities),
        construction_costs(joint_water_treatment_plant.construction_costs) {
}

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
    return added_treatment_capacities[utility_id];
}


/**
 * Returns the price to be paid (no drama intended) for a given utility and
 * deducts it from maximum planned expansion.
 * @param utility_id
 * @return
 */
double SequentialJointTreatmentExpansion::calculateConstructionCost(int utility_id) {
    construction_cost_of_capital = construction_costs[utility_id];;
    return construction_costs[utility_id];
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
