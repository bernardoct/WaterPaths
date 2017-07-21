//
// Created by bernardoct on 7/18/17.
//

#ifndef TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
#define TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H


#include "Base/WaterSource.h"

class JointWaterTreatmentPlant : public WaterSource {
public:
    JointWaterTreatmentPlant(
            const char *name, const int id, const int parent_reservoir_ID,
            double total_treatment_capacity,
            const vector<double> *added_treatment_capacity_fractions,
            const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate);

    JointWaterTreatmentPlant(
            const JointWaterTreatmentPlant &joint_water_treatment_plant);

    JointWaterTreatmentPlant &operator=(
            const JointWaterTreatmentPlant &joint_water_treatment_plant);

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> *demand_outflow);

    const unsigned int parent_reservoir_ID;
    const double total_treatment_capacity;
    const vector<double> *added_treatment_capacity_fractions;
};


#endif //TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
