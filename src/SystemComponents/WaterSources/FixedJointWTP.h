//
// Created by dgorelic on 10/28/2019.
//

#ifndef TRIANGLEMODEL_FIXEDJOINTWTP_H
#define TRIANGLEMODEL_FIXEDJOINTWTP_H


#include "JointWTP.h"

class FixedJointWTP : public JointWTP {
private:

    vector<double> *permanent_treatment_allocations;

public:
    FixedJointWTP(const char *name, const int id,
                  const int parent_reservoir_ID,
                  const int expansion_sequence_id,
                  const double total_treatment_capacity,
                  vector<int> connected_sources,
                  vector<int> &agreement_utility_ids,
                  vector<double> &fixed_treatment_capacity_allocations,
                  vector<Bond *> &bonds,
                  const vector<double> &construction_time_range,
                  double permitting_period);

    FixedJointWTP(const FixedJointWTP &fixed_joint_water_treatment_plant);

    ~FixedJointWTP() override;

    FixedJointWTP &operator=(const FixedJointWTP &fixed_joint_water_treatment_plant);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    double implementInitialTreatmentCapacity(int utility_id) override;
};


#endif //TRIANGLEMODEL_FIXEDJOINTWTP_H
