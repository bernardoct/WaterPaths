//
// Created by bernardoct on 7/25/17.
//

#ifndef TRIANGLEMODEL_RESERVOIRRELLOCATION_H
#define TRIANGLEMODEL_RESERVOIRRELLOCATION_H


#include "Base/WaterSource.h"

class Relocation : public WaterSource {
public:
    const unsigned long parent_reservoir_ID;
    const vector<double> *new_allocated_fractions;
    const vector<int> *utilities_with_allocations;

    Relocation(string name, const int id, unsigned long parent_reservoir_ID,
               vector<double> *allocated_fractions,
               vector<int> *utilities_with_allocations,
               vector<int> construction_prerequisites,
               const vector<double> &construction_time_range, double permitting_period,
               Bond &bond);

    Relocation(const Relocation &relocation);

    void applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_discharge,
                             vector<double> &demand_outflow) override;

    unsigned long getParent_reservoir_ID() const;
};


#endif //TRIANGLEMODEL_RESERVOIRRELLOCATION_H
