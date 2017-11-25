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
            const double capacity, const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double permitting_period,
            double construction_cost);

    ReservoirExpansion(const ReservoirExpansion &reservoir_expansion);

    ReservoirExpansion &operator=(
            const ReservoirExpansion &reservoir_expansion);

    void applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_discharge,
                             vector<double> &demand_outflow) override;

};


#endif //TRIANGLEMODEL_RESERVOIREXPANSION_H
