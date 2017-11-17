//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_RESERVOIREXPANSION_H
#define TRIANGLEMODEL_RESERVOIREXPANSION_H


#include "Base/WaterSource.h"

class ReservoirExpansion : public WaterSource {
public:

    const unsigned int parent_reservoir_ID;

    ReservoirExpansion(
            const char *name, const int id,
            const unsigned int parent_reservoir_ID,
            const float capacity, const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate);

    ReservoirExpansion(const ReservoirExpansion &reservoir_expansion);

    ReservoirExpansion &operator=(
            const ReservoirExpansion &reservoir_expansion);

    void applyContinuity(int week, float upstream_source_inflow,
                             float wastewater_discharge,
                             vector<float> &demand_outflow) override;

};


#endif //TRIANGLEMODEL_RESERVOIREXPANSION_H
