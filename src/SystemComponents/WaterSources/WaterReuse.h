//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_WATERREUSE_H
#define TRIANGLEMODEL_WATERREUSE_H


#include "Base/WaterSource.h"

class WaterReuse : public WaterSource {
public:
    WaterReuse(const char *name, const int id, const double capacity);

    WaterReuse(
            const char *name, const int id, const double capacity,
            const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost_of_capital, double bond_term,
            double bond_interest_rate);

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> *demand_outflow) override;

    WaterReuse &operator=(const WaterReuse &water_reuse);

};

#endif //TRIANGLEMODEL_WATERREUSE_H
