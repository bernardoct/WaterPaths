//
// Created by David on 12/7/2017.
//

#ifndef TRIANGLEMODEL_ALLOCATEDRESERVOIREXPANSION_H
#define TRIANGLEMODEL_ALLOCATEDRESERVOIREXPANSION_H


#include "ReservoirExpansion.h"

class AllocatedReservoirExpansion : public ReservoirExpansion {
private:
    vector<double> *new_allocated_fractions;
    vector<double> *new_allocated_treatment_fractions;

public:
    AllocatedReservoirExpansion(
            const char *name, const int id,
            const unsigned int parent_reservoir_id,
            const double new_capacity,
            vector<int> *utilities_with_allocations,
            vector<double> *new_allocated_fractions,
            vector<double> *new_allocated_treatment_fractions,
            const double construction_rof_or_demand, const vector<double> &construction_time_range,
            double permitting_period, double construction_cost);

    AllocatedReservoirExpansion(const AllocatedReservoirExpansion &allocated_reservoir_expansion);

    AllocatedReservoirExpansion &operator=(
            const AllocatedReservoirExpansion &allocated_reservoir_expansion);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

//    void addCapacity(double capacity) override;
//
//    void setAllocations(vector<int> *utilities_with_allocations,
//                        vector<double> *new_allocated_fractions,
//                        vector<double> *new_allocated_treatment_fractions) override;

//    double getNewCapacity(int id);

    vector<double>* getNewAllocationFractions(int id);

    vector<double> *getNewTreatmentAllocationFractions(int id);
};


#endif //TRIANGLEMODEL_ALLOCATEDRESERVOIREXPANSION_H
