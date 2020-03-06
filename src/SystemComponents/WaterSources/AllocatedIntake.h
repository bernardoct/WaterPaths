//
// Created by dgorelic on 3/2/2020.
//

#ifndef TRIANGLEMODEL_ALLOCATEDINTAKE_H
#define TRIANGLEMODEL_ALLOCATEDINTAKE_H


#include "Base/WaterSource.h"

class AllocatedIntake : public WaterSource {

public:
    double next_upstream_catchment_inflow = 0;
    vector<double> allocated_demands;

    AllocatedIntake(
            const char *name, const int id, const vector<Catchment *> &catchments,
            vector<int> connected_sources, vector<int> &partner_utilities,
            const double total_raw_water_capacity,
            vector<double> &raw_water_capacity_allocation_fractions,
            const double total_treatment_capacity,
            vector<double> &treatment_capacity_allocation_fractions);

    AllocatedIntake(const AllocatedIntake &intake);

    AllocatedIntake &operator=(const AllocatedIntake &intake);

    ~AllocatedIntake() override;

    void applyContinuity(int week, double upstream_source_min_env_flow,
                         double wastewater_inflow, vector<double> &demand) override;

    void setRealization(unsigned long r, vector<double> &rdm_factors) override;

    double getPrioritySourcePotentialVolume(int utility_id) const override;

    vector<double> getAllocatedDemands() const;

    void addCapacity(double capacity, int utility_id);

    void addTreatmentCapacity(const double added_treatment_capacity, int utility_id);
};


#endif //TRIANGLEMODEL_ALLOCATEDINTAKE_H
