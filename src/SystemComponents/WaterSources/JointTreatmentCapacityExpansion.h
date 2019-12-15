//
// Created by Bernardo on 12/14/2019.
//

#ifndef TRIANGLEMODEL_JOINTTREATMENTCAPACITYEXPANSION_H
#define TRIANGLEMODEL_JOINTTREATMENTCAPACITYEXPANSION_H


#include "Base/WaterSource.h"

class JointTreatmentCapacityExpansion : public WaterSource {
private:
    double total_treatment_capacity;

public:
    const map<int, double> &added_treatment_capacities;
    const int parent_source_id;

    JointTreatmentCapacityExpansion(string name, const int id, int parent_source_id,
                                    const map<int, double> &added_treatment_capacities,
                                    vector<Bond *> &bonds,
                                    const vector<double> &construction_time_range,
                                    double permitting_period);

    JointTreatmentCapacityExpansion(
            const JointTreatmentCapacityExpansion &joint_water_treatment_plant);

    ~JointTreatmentCapacityExpansion() override;

    JointTreatmentCapacityExpansion &operator=(
            const JointTreatmentCapacityExpansion &joint_water_treatment_plant);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    double implementTreatmentCapacity(int utility_id);

    void addTreatmentCapacity(const double added_treatment_capacity,
                              int utility_id) override;

    void addCapacity(double capacity) override;
};


#endif //TRIANGLEMODEL_JOINTTREATMENTCAPACITYEXPANSION_H
