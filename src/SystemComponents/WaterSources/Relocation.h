//
// Created by bernardoct on 7/25/17.
//

#ifndef TRIANGLEMODEL_RESERVOIRRELLOCATION_H
#define TRIANGLEMODEL_RESERVOIRRELLOCATION_H


#include "Base/WaterSource.h"

class Relocation : public WaterSource {
public:
    Relocation(const Relocation &relocation);

    const unsigned long parent_reservoir_ID;
    const vector<float> *new_allocated_fractions;
    const vector<int> *utilities_with_allocations;

    Relocation(
            const char *name, const int id, unsigned long parent_reservoir_ID,
            vector<float> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost_of_capital, float bond_term,
            float bond_interest_rate);

    void applyContinuity(int week, float upstream_source_inflow,
                             float wastewater_discharge,
                             vector<float> &demand_outflow) override;

    unsigned long getParent_reservoir_ID() const;
};


#endif //TRIANGLEMODEL_RESERVOIRRELLOCATION_H
