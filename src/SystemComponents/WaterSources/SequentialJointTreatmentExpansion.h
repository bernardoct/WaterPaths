//
// Created by bernardoct on 7/18/17.
//

#ifndef TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
#define TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H


#include "Base/WaterSource.h"

class SequentialJointTreatmentExpansion : public WaterSource {
private:
    double total_treatment_capacity;
    vector<double> max_sequential_added_capacity;
    vector<double> max_sequential_added_construction_cost;
    double implemented_treatment_capacity = 0.0;

public:
    const int expansion_sequence_id;
    double unallocated_treatment_capacity = 0.0;

    SequentialJointTreatmentExpansion(
            const char *name, const int id, const int parent_reservoir_ID,
            double total_treatment_capacity,
            const vector<double>& added_treatment_capacity_fractions,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double permitting_period,
            double construction_cost);

    SequentialJointTreatmentExpansion(
            const char *name, const int id, const int parent_reservoir_ID,
            const int expansion_sequence_id,
            double total_treatment_capacity,
            const vector<double>& added_treatment_capacity_fractions,
            vector<double>& max_sequential_added_capacity,
            vector<double>& max_sequential_added_construction_cost,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double permitting_period,
            double construction_cost);

    SequentialJointTreatmentExpansion(
            const SequentialJointTreatmentExpansion &joint_water_treatment_plant);

    SequentialJointTreatmentExpansion &operator=(
            const SequentialJointTreatmentExpansion &joint_water_treatment_plant);

    void applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_discharge,
                             vector<double> &demand_outflow) override;

    const unsigned int parent_reservoir_ID;
    const vector<double> added_treatment_capacity_fractions;

    double implementTreatmentCapacity(int utility_id);

    const vector<double>& getMax_sequential_added_capacity() const;

    void setMax_sequential_added_capacity(
            const vector<double> &max_sequential_added_capacity);

    const vector<double>& getMax_sequential_added_construction_cost() const;

    void setMax_sequential_added_construction_cost(
            const vector<double> &max_sequential_added_construction_cost);

    double payConstructionCost(int utility_id);

    double calculateNetPresentConstructionCost(
            int week, int utility_id, double discount_rate,
            double& level_debt_service_payment, double bond_term,
            double bond_interest_rate) const override;

    virtual ~SequentialJointTreatmentExpansion();
};


#endif //TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
