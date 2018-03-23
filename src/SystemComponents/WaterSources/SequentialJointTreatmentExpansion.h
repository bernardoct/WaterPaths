//
// Created by bernardoct on 7/18/17.
//

#ifndef TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
#define TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H


#include "Base/WaterSource.h"

class SequentialJointTreatmentExpansion : public WaterSource {
private:

    double total_treatment_capacity;
    vector<double> &added_treatment_capacities;
    vector<double> &construction_costs;

public:
    const int expansion_sequence_id;

    SequentialJointTreatmentExpansion(const char *name, const int id,
                                          const int parent_reservoir_ID,
                                          const int expansion_sequence_id,
                                          vector<int> connected_sources,
                                          vector<double> &sequential_treatment_capacity,
                                          vector<double> &sequential_cost,
                                          const vector<double> &construction_time_range,
                                          double permitting_period);

    SequentialJointTreatmentExpansion(
            const SequentialJointTreatmentExpansion &joint_water_treatment_plant);

    SequentialJointTreatmentExpansion &operator=(
            const SequentialJointTreatmentExpansion &joint_water_treatment_plant);

    void applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_discharge,
                             vector<double> &demand_outflow) override;

    const unsigned int parent_reservoir_ID;

    double implementTreatmentCapacity(int utility_id);

    double calculateConstructionCost(int utility_id);

    double calculateNetPresentConstructionCost(
            int week, int utility_id, double discount_rate,
            double& level_debt_service_payment, double bond_term,
            double bond_interest_rate) const override;

    ~SequentialJointTreatmentExpansion() override;

    void setSequential_treatment_capacity(vector<vector<double>> *sequential_treatment_capacity);

    void setSequential_cost(vector<vector<double>> *sequential_cost);

    vector<vector<double>> *getSequential_treatment_capacity() const;

    vector<vector<double>> *getSequential_cost() const;


};


#endif //TRIANGLEMODEL_JOINTWATERTREATMENTPLANT_H
