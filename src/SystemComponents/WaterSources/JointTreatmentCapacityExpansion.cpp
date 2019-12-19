//
// Created by Bernardo on 12/14/2019.
//

#include "JointTreatmentCapacityExpansion.h"

JointTreatmentCapacityExpansion::JointTreatmentCapacityExpansion(
        string name, const int id, int parent_source_id,
        const map<int, double> &added_treatment_capacities, vector<int> construction_prerequisites,
        vector<Bond *> &bonds, const vector<double> &construction_time_range,
        double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED,
                      construction_prerequisites, NEW_JOINT_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, bonds),
          added_treatment_capacities(added_treatment_capacities),
          parent_source_id(parent_source_id) {
    total_treatment_capacity = 0;
    for (auto p : added_treatment_capacities) {
        total_treatment_capacity += p.second;
    }
}

JointTreatmentCapacityExpansion::JointTreatmentCapacityExpansion(
        const JointTreatmentCapacityExpansion &joint_water_treatment_plant)
        : WaterSource(joint_water_treatment_plant),
          parent_source_id(joint_water_treatment_plant.parent_source_id),
          added_treatment_capacities(
                  joint_water_treatment_plant.added_treatment_capacities),
          total_treatment_capacity(
                  joint_water_treatment_plant.total_treatment_capacity) {
}

JointTreatmentCapacityExpansion::~JointTreatmentCapacityExpansion() = default;

JointTreatmentCapacityExpansion &JointTreatmentCapacityExpansion::operator=(
        const JointTreatmentCapacityExpansion &joint_water_treatment_plant) {
    return *this;
}

void JointTreatmentCapacityExpansion::applyContinuity(int week,
                                                      double upstream_source_inflow,
                                                      double wastewater_discharge,
                                                      vector<double> &demand_outflow) {
    throw logic_error("Treatment capacity expansion only adds treatment "
                      "capacity to the infrastructure they're assigned to. "
                      "Continuity cannot be called on it, but only on the "
                      "infrastructure it's assigned to.");
}

double
JointTreatmentCapacityExpansion::implementTreatmentCapacity(int utility_id) {
    return added_treatment_capacities.at(utility_id);
}

void JointTreatmentCapacityExpansion::addTreatmentCapacity(
        const double added_treatment_capacity, int utility_id) {
    throw runtime_error(
            "Can't add treatment capacity to JointTreatmentCapacityExpansion");
}

void JointTreatmentCapacityExpansion::addCapacity(double capacity) {
    throw runtime_error(
            "Can't add capacity to JointTreatmentCapacityExpansion");
}
