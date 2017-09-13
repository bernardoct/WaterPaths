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
    const vector<double> *new_allocated_fractions;
    const vector<int> *utilities_with_allocations;

    Relocation(
            const char *name, const int id, unsigned long parent_reservoir_ID,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double permitting_period,
            double construction_cost_of_capital, double bond_term,
            double bond_interest_rate);

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> &demand_outflow) override;

    unsigned long getParent_reservoir_ID() const;
};


#endif //TRIANGLEMODEL_RESERVOIRRELLOCATION_H
