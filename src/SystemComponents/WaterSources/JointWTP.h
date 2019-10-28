//
// Created by dgorelic on 10/28/2019.
//

#ifndef TRIANGLEMODEL_JOINTWTP_H
#define TRIANGLEMODEL_JOINTWTP_H


#include "Base/WaterSource.h"

class JointWTP : public WaterSource {
protected:

    double total_treatment_capacity;

public:
    const int expansion_sequence_id;
    const unsigned int parent_reservoir_ID;
    const int joint_agreement_type;

    JointWTP(const char *name, const int id, const int agreement_type,
             const int parent_reservoir_ID,
             const int expansion_sequence_id,
             const double total_treatment_capacity,
             vector<int> connected_sources,
             vector<Bond *> &bonds,
             const vector<double> &construction_time_range,
             double permitting_period);

    JointWTP(const JointWTP &joint_water_treatment_plant);

    ~JointWTP() override;

    JointWTP &operator=(const JointWTP &joint_water_treatment_plant);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    virtual double implementInitialTreatmentCapacity(int utility_id);

    int getAgreementType() const override;
};


#endif //TRIANGLEMODEL_JOINTWTP_H
