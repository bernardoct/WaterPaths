//
// Created by dgorelic on 3/2/2020.
//

#ifndef TRIANGLEMODEL_ALLOCATEDINTAKEEXPANSION_H
#define TRIANGLEMODEL_ALLOCATEDINTAKEEXPANSION_H


#include "Base/WaterSource.h"

class AllocatedIntakeExpansion : public WaterSource {

public:

    const unsigned int parent_intake_ID;
    const vector<double> supply_capacity_added;
    const vector<double> treatment_capacity_added;

    AllocatedIntakeExpansion(const char *name, const int id, const unsigned int parent_intake_ID,
                    const double total_supply_capacity_added, double total_treatment_capacity_added,
                    vector<int> &partner_utility_ids,
                    vector<double> &supply_capacity_allocations_added,
                    vector<double> &treatment_capacity_allocations_added,
                    const vector<double> &construction_time_range, vector<int> connected_sources,
                    double permitting_period, vector<Bond *> &bonds);

    AllocatedIntakeExpansion(const AllocatedIntakeExpansion &intake_expansion);

    AllocatedIntakeExpansion &operator=(
            const AllocatedIntakeExpansion &intake_expansion);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    double getAllocatedCapacity(int utility_id) override;

    double getAllocatedTreatmentCapacity(int utility_id) const override;

    Bond &getBond(int utility_id) override;
};


#endif //TRIANGLEMODEL_ALLOCATEDINTAKEEXPANSION_H
