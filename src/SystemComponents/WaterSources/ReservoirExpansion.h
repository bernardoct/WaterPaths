//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_RESERVOIREXPANSION_H
#define TRIANGLEMODEL_RESERVOIREXPANSION_H


#include "Base/WaterSource.h"

class ReservoirExpansion : public WaterSource {
public:

    const int parent_reservoir_ID;

    ReservoirExpansion(
            const char *name, const int id,
            const int parent_reservoir_ID,
            const double capacity, const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate);

    ReservoirExpansion(const ReservoirExpansion &reservoir_expansion);

    ReservoirExpansion &operator=(
            const ReservoirExpansion &reservoir_expansion);

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> *demand_outflow,
            int n_utilities) override;

};


#endif //TRIANGLEMODEL_RESERVOIREXPANSION_H
