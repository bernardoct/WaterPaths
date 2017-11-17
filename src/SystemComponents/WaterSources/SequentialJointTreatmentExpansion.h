//
// Created by bernardoct on 7/18/17.
//

#ifndef TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
#define TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H


#include "Base/WaterSource.h"

class SequentialJointTreatmentExpansion : public WaterSource {
private:
    float total_treatment_capacity;
    vector<float> *max_sequential_added_capacity = nullptr;
    vector<float> *max_sequential_added_construction_cost = nullptr;

public:
    const int expansion_sequence_id;

    SequentialJointTreatmentExpansion(
            const char *name, const int id, const int parent_reservoir_ID,
            float total_treatment_capacity,
            const vector<float> *added_treatment_capacity_fractions,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate);

    SequentialJointTreatmentExpansion(
            const char *name, const int id, const int parent_reservoir_ID,
            const int expansion_sequence_id,
            float total_treatment_capacity,
            const vector<float> *added_treatment_capacity_fractions,
            vector<float> *max_sequential_added_capacity,
            vector<float> *max_sequential_added_construction_cost,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate);

    SequentialJointTreatmentExpansion(
            const SequentialJointTreatmentExpansion &joint_water_treatment_plant);

    SequentialJointTreatmentExpansion &operator=(
            const SequentialJointTreatmentExpansion &joint_water_treatment_plant);

    void applyContinuity(int week, float upstream_source_inflow,
                             float wastewater_discharge,
                             vector<float> &demand_outflow) override;

    const unsigned int parent_reservoir_ID;
    const vector<float> *added_treatment_capacity_fractions;

    float implementTreatmentCapacity(int utility_id);

    vector<float> *getMax_sequential_added_capacity() const;

    void setMax_sequential_added_capacity(
            vector<float> *max_sequential_added_capacity);

    vector<float> *getMax_sequential_added_construction_cost() const;

    void setMax_sequential_added_construction_cost(
            vector<float> *max_sequential_added_construction_cost);

    float payConstructionCost(int utility_id);

    float calculateNetPresentConstructionCost(
            int week, int utility_id, float discount_rate,
            float *level_debt_service_payment) const override;
};


#endif //TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
