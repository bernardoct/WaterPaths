//
// Created by dgorelic on 10/28/2019.
//

#include "JointWTP.h"

JointWTP::JointWTP(const char *name,
                   const int id,
                   const int agreement_type,
                   const int parent_reservoir_ID,
                   const int expansion_sequence_id,
                   const double total_treatment_capacity,
                   vector<int> connected_sources,
                   vector<Bond *> &bonds,
                   const vector<double> &construction_time_range,
                   double permitting_period)
        : WaterSource(name, id, vector<Catchment *>(), NONE, NON_INITIALIZED, connected_sources, NEW_JOINT_WATER_TREATMENT_PLANT,
                      construction_time_range, permitting_period, bonds),
          expansion_sequence_id(expansion_sequence_id),
          total_treatment_capacity(total_treatment_capacity),
          joint_agreement_type(agreement_type),
          parent_reservoir_ID((unsigned int) parent_reservoir_ID) {
}


JointWTP::JointWTP(const JointWTP &joint_water_treatment_plant) :
        WaterSource(joint_water_treatment_plant),
        joint_agreement_type(joint_water_treatment_plant.joint_agreement_type),
        total_treatment_capacity(joint_water_treatment_plant.total_treatment_capacity),
        expansion_sequence_id(joint_water_treatment_plant.expansion_sequence_id),
        parent_reservoir_ID(joint_water_treatment_plant.parent_reservoir_ID) {
}

JointWTP::~JointWTP() = default;

JointWTP &JointWTP::operator=(const JointWTP &joint_water_treatment_plant) {
    WaterSource::operator=(joint_water_treatment_plant);
    return *this;
}

void JointWTP::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                               vector<double> &demand_outflow) {
    throw logic_error("Error in JointWTP: Reservoir expansion only adds storage volume to the "
                      "reservoir assigned.  Continuity "
                      "cannot be called on it, but only on the "
                      "reservoir it's assigned to expand.");
}

double JointWTP::implementInitialTreatmentCapacity(int utility_id) {
    throw logic_error("Error in JointWTP: treatment capacity to be added must be "
                      "designated in a child class of JointWTP through the override of this function.");
    return 0;
}

int JointWTP::getAgreementType() const {
    return joint_agreement_type;
}
