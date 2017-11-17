//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_WATERREUSE_H
#define TRIANGLEMODEL_WATERREUSE_H


#include "Base/WaterSource.h"

class WaterReuse : public WaterSource {
private:
    float treated_volume;
public:
    WaterReuse(const char *name, const int id, const float capacity);

    WaterReuse(
            const char *name, const int id, const float capacity,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost_of_capital, float bond_term,
            float bond_interest_rate);

    void applyContinuity(int week, float upstream_source_inflow,
                             float wastewater_discharge,
                             vector<float> &demand_outflow) override;

    WaterReuse &operator=(const WaterReuse &water_reuse);

    float getReused_volume() const;

};

#endif //TRIANGLEMODEL_WATERREUSE_H
