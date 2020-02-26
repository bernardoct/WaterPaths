//
// Created by dgorelic on 10/29/2019.
//

#ifndef TRIANGLEMODEL_VARIABLEJOINTWTP_H
#define TRIANGLEMODEL_VARIABLEJOINTWTP_H


#include "JointWTP.h"

class VariableJointWTP : public JointWTP {
private:

    vector<double> initial_treatment_allocations;

public:
    VariableJointWTP(const char *name, const int id,
                     const int parent_reservoir_ID,
                     const int expansion_sequence_id,
                     const double total_treatment_capacity,
                     vector<int> connected_sources,
                     vector<int> &agreement_utility_ids,
                     vector<double> &initial_treatment_capacity_allocations,
                     vector<Bond *> &bonds,
                     const vector<double> &construction_time_range,
                     double permitting_period);

    VariableJointWTP(const VariableJointWTP &variable_joint_water_treatment_plant);

    ~VariableJointWTP() override;

    VariableJointWTP &operator=(const VariableJointWTP &variable_joint_water_treatment_plant);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    double implementInitialTreatmentCapacity(int utility_id) override;

    void resetAllocations(const vector<double> *demand_ratios) override; // for storage capacity in WaterSource, for treatment capacity here
};


#endif //TRIANGLEMODEL_VARIABLEJOINTWTP_H
