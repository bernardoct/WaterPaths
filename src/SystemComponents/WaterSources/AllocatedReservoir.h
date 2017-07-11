//
// Created by bernardoct on 7/9/17.
//

#ifndef TRIANGLEMODEL_ALLOCATEDRESERVOIR_H
#define TRIANGLEMODEL_ALLOCATEDRESERVOIR_H


#include "Reservoir.h"

class AllocatedReservoir : public Reservoir {
private:
    double *allocated_capacities;

public:
    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            const vector<int> *utilities_with_allocations,
            const vector<double> *allocated_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate,
            const vector<int> *utilities_with_allocations,
            const vector<double> *allocated_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            const vector<int> *utilities_with_allocations,
            const vector<double> *allocated_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate,
            const vector<int> *utilities_with_allocations,
            const vector<double> *allocated_fractions);

    AllocatedReservoir &operator=(
            const AllocatedReservoir
            &allocated_reservoir);

    AllocatedReservoir(const AllocatedReservoir &reservoir);

    const vector<int> *utilities_with_allocations;
    const double *allocated_fractions;

    void applyContinuity(
            int week, double upstream_source_inflow,
            double *demand_outflow) override;

    double getAvailableAllocatedVolume(int utility_id) override;

    const double *getAvailableAllocatedVolumes() const;
};


#endif //TRIANGLEMODEL_ALLOCATEDRESERVOIR_H
