//
// Created by bernardoct on 7/18/17.
//

#include "JointWaterTreatmentPlant.h"

JointWaterTreatmentPlant::JointWaterTreatmentPlant(
        const char *name, const int id, const int parent_reservoir_ID,
        double total_treatment_capacity,
        const vector<double> *added_treatment_capacity_fractions,
        const double construction_rof,
        const vector<double> &construction_time_range,
        double construction_cost, double bond_term,
        double bond_interest_rate)
        : WaterSource(name,
                      id,
                      vector<Catchment *>(),
                      NONE,
                      NON_INITIALIZED,
                      NEW_WATER_TREATMENT_PLANT,
                      construction_rof,
                      construction_time_range,
                      construction_cost,
                      bond_term,
                      bond_interest_rate),
          parent_reservoir_ID((const unsigned int) parent_reservoir_ID),
          added_treatment_capacity_fractions
                  (added_treatment_capacity_fractions),
          total_treatment_capacity(total_treatment_capacity) {}

/**
 * Copy constructor.
 * @param reservoir
 */
JointWaterTreatmentPlant::JointWaterTreatmentPlant(
        const
        JointWaterTreatmentPlant
        &joint_water_treatment_plant) :
        WaterSource(joint_water_treatment_plant),
        parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID),
        total_treatment_capacity
                (joint_water_treatment_plant.total_treatment_capacity),
        added_treatment_capacity_fractions
                (joint_water_treatment_plant
                         .added_treatment_capacity_fractions) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
JointWaterTreatmentPlant &JointWaterTreatmentPlant::operator=(
        const JointWaterTreatmentPlant &joint_water_treatment_plant) {
    WaterSource::operator=(joint_water_treatment_plant);
    return *this;
}

void JointWaterTreatmentPlant::applyContinuity(
        int week, double upstream_source_inflow,
        vector<double> *demand_outflow) {
    __throw_logic_error("Reservoir expansion only add storage volume to the "
                                "reservoir they're assigned to.  Continuity "
                                "cannot be called on it, but only on the "
                                "reservoir it's  assigned to expand.");
}

