//
// Created by bernardoct on 7/18/17.
//

#include "SequentialJointTreatmentExpansion.h"

SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(
        const char *name, const int id, const int parent_reservoir_ID,
        double total_treatment_capacity,
        const vector<double> *added_treatment_capacity_fractions,
        const double construction_rof_or_demand,
        const vector<double> &construction_time_range, double permitting_period,
        double construction_cost)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, NEW_WATER_TREATMENT_PLANT,
                      construction_time_range, parent_reservoir_ID, construction_cost),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          added_treatment_capacity_fractions
                  (added_treatment_capacity_fractions),
          total_treatment_capacity(total_treatment_capacity),
          expansion_sequence_id(NON_INITIALIZED) {}


SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(
        const char *name, const int id, const int parent_reservoir_ID,
        const int expansion_sequence_id,
        double total_treatment_capacity,
        const vector<double> *added_treatment_capacity_fractions,
        vector<double> *max_sequential_added_capacity,
        vector<double> *max_sequential_added_construction_cost,
        const double construction_rof_or_demand,
        const vector<double> &construction_time_range, double permitting_period,
        double construction_cost)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, NEW_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, construction_cost),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID),
          added_treatment_capacity_fractions
                  (added_treatment_capacity_fractions),
          total_treatment_capacity(total_treatment_capacity),
          max_sequential_added_capacity(max_sequential_added_capacity),
          max_sequential_added_construction_cost(max_sequential_added_construction_cost),
          expansion_sequence_id(expansion_sequence_id) {

    if (max_sequential_added_capacity->empty() &&
        !this->max_sequential_added_capacity->empty())
        __throw_invalid_argument("max_sequential_capacity is initialized "
                                         "within "
                                         "SequentialJointTreatmentExpansion "
                                         "and must be an empty vector.");

    int n_allocations = (int) added_treatment_capacity_fractions->size();

    /// Set pointer to vector with capacities to be built on parent reservoir
    /// for each sequential joint treatment expansion with the same parent
    /// reservoir to be the same, so that whenever on utility builds a level
    /// of expansion, the next level add up the difference between levels to
    /// the parent reservoir.
    if (max_sequential_added_capacity->empty()) {
        for (int u = 0; u < n_allocations; ++u) {
            double alloc = (*added_treatment_capacity_fractions)[u];
            max_sequential_added_capacity->push_back(
                    total_treatment_capacity * alloc);
            max_sequential_added_construction_cost->push_back(
                    construction_cost * alloc);

            implemented_treatment_capacity += total_treatment_capacity * alloc;
        }
    } else {
        for (int u = 0; u < n_allocations; ++u) {
            double alloc = (*added_treatment_capacity_fractions)[u];
            (*max_sequential_added_capacity)[u] = max
                    ((*max_sequential_added_capacity)[u],
                     total_treatment_capacity * alloc);
            (*max_sequential_added_construction_cost)[u] = max
                    ((*max_sequential_added_construction_cost)[u],
                     construction_cost * alloc);

            implemented_treatment_capacity += (*max_sequential_added_capacity)[u];
        }
    }

    unallocated_treatment_capacity = total_treatment_capacity - implemented_treatment_capacity;

}

/**
 * Copy constructor.
 * @param reservoir
 */
SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(
        const SequentialJointTreatmentExpansion &joint_water_treatment_plant) :
        WaterSource(joint_water_treatment_plant),
        parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID),
        total_treatment_capacity
                (joint_water_treatment_plant.total_treatment_capacity),
        added_treatment_capacity_fractions
                (joint_water_treatment_plant
                         .added_treatment_capacity_fractions),
        expansion_sequence_id(
                joint_water_treatment_plant.expansion_sequence_id),
        unallocated_treatment_capacity(joint_water_treatment_plant.unallocated_treatment_capacity) {

    if (joint_water_treatment_plant.max_sequential_added_capacity) {
        max_sequential_added_capacity = new vector<double>
                (*joint_water_treatment_plant.max_sequential_added_capacity);

        max_sequential_added_construction_cost = new vector<double>
                (*joint_water_treatment_plant
                        .max_sequential_added_construction_cost);
    }
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

    if (max_sequential_added_capacity) {
        double alloc = (*added_treatment_capacity_fractions)[utility_id];
        double capacity_to_implement = min
                ((*max_sequential_added_capacity)[utility_id],
                 total_treatment_capacity * alloc);
        (*max_sequential_added_capacity)[utility_id] -= capacity_to_implement;
        return capacity_to_implement;
    } else
        return total_treatment_capacity;

}


/**
 * Returns the price to be paid (no drama intended) for a given utility and
 * deducts it from maximum planned expansion.
 * @param utility_id
 * @return
 */
double SequentialJointTreatmentExpansion::payConstructionCost(int utility_id) {
    if (max_sequential_added_construction_cost) {
        double alloc = (*added_treatment_capacity_fractions)[utility_id];
        double price_to_pay = min
                ((*max_sequential_added_construction_cost)[utility_id],
                 construction_cost_of_capital * alloc);
        (*max_sequential_added_construction_cost)[utility_id] -= price_to_pay;
        return price_to_pay;
    } else
        return construction_cost_of_capital;
}

vector<double> *
SequentialJointTreatmentExpansion::getMax_sequential_added_capacity() const {
    return max_sequential_added_capacity;
}

double SequentialJointTreatmentExpansion::getTotal_treatment_capacity(int utility_id) const {
    return total_treatment_capacity;
}

void SequentialJointTreatmentExpansion::setMax_sequential_added_capacity(
        vector<double> *max_sequential_added_capacity) {
    this->max_sequential_added_capacity = max_sequential_added_capacity;
}

vector<double> *
SequentialJointTreatmentExpansion::getMax_sequential_added_construction_cost() const {
    return max_sequential_added_construction_cost;
}

void
SequentialJointTreatmentExpansion::setMax_sequential_added_construction_cost(
        vector<double> *max_sequential_added_construction_cost) {
    this->max_sequential_added_capacity = max_sequential_added_construction_cost;
}

double SequentialJointTreatmentExpansion::calculateNetPresentConstructionCost(int week, int utility_id, double discount_rate,
                                                                              double *level_debt_service_payment, double bond_term,
                                                                              double bond_interest_rate) const {
    double combined_price = WaterSource::calculateNetPresentConstructionCost
            (week, utility_id, discount_rate, level_debt_service_payment, bond_term, bond_interest_rate);
    double alloc = (*added_treatment_capacity_fractions)[utility_id];

    /// Return yearly payments proportional to treatment allocation.
    *level_debt_service_payment *= alloc;

    /// Return net present cost proportional to treatment allocation.
    return combined_price * alloc;
}
