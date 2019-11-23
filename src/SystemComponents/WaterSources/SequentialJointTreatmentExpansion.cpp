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
 * @param bonds
 * @param construction_time_range
 * @param permitting_period
 */
SequentialJointTreatmentExpansion::SequentialJointTreatmentExpansion(string name, const int id,
                                                                     const int parent_reservoir_ID,
                                                                     const int expansion_sequence_id,
                                                                     vector<int> connected_sources,
                                                                     vector<double> &sequential_treatment_capacity,
                                                                     vector<Bond *> &bonds,
                                                                     const vector<double> &construction_time_range,
                                                                     double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, connected_sources, NEW_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, bonds),
          added_treatment_capacities(sequential_treatment_capacity),
          expansion_sequence_id(expansion_sequence_id),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID) {

    if (sequential_treatment_capacity.empty())
        throw invalid_argument("max_sequential_capacity is initialized "
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
        total_treatment_capacity(
                joint_water_treatment_plant.total_treatment_capacity),
        added_treatment_capacities(joint_water_treatment_plant.added_treatment_capacities),
        expansion_sequence_id(
                joint_water_treatment_plant.expansion_sequence_id),
        parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID) {
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
    throw logic_error("Reservoir expansion only add storage volume to the "
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
