//
// Created by bernardoct on 7/9/17.
//

#include "AllocatedReservoir.h"

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, DataSeries *storage_area_curve,
        const int *utilities_with_allocations,
        const double *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area_curve),
          utilities_with_allocations(utilities_with_allocations),
          allocated_fractions(allocated_fractions) {}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, DataSeries *storage_area_curve,
        const double construction_rof,
        const vector<double> &construction_time_range, double construction_cost,
        double bond_term, double bond_interest_rate,
        const int *utilities_with_allocations,
        const double *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area_curve,
                    construction_rof,
                    construction_time_range,
                    construction_cost,
                    bond_term,
                    bond_interest_rate),
          utilities_with_allocations(utilities_with_allocations),
          allocated_fractions(allocated_fractions) {}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        const int *utilities_with_allocations,
        const double *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area),
          utilities_with_allocations(utilities_with_allocations),
          allocated_fractions(allocated_fractions) {}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        const double construction_rof,
        const vector<double> &construction_time_range, double construction_cost,
        double bond_term, double bond_interest_rate,
        const int *utilities_with_allocations,
        const double *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area,
                    construction_rof,
                    construction_time_range,
                    construction_cost,
                    bond_term,
                    bond_interest_rate),
          utilities_with_allocations(utilities_with_allocations),
          allocated_fractions(allocated_fractions) {}

AllocatedReservoir::AllocatedReservoir(
        const AllocatedReservoir &reservoir)
        : Reservoir(reservoir),
          utilities_with_allocations(reservoir.utilities_with_allocations),
          allocated_fractions(reservoir.allocated_fractions) {}

/**
 * Copy assignment operator
 * @param allocated_reservoir
 * @return
 */
AllocatedReservoir &
AllocatedReservoir::operator=(const AllocatedReservoir &allocated_reservoir) {
    utilities_with_allocations = allocated_reservoir.utilities_with_allocations;
    allocated_fractions = allocated_reservoir.allocated_fractions;
    Reservoir::operator=(allocated_reservoir);
    return *this;
}

void AllocatedReservoir::applyContinuity(
        int week, double upstream_source_inflow, double demand_outflow) {
    Reservoir::applyContinuity(week,
                               upstream_source_inflow,
                               demand_outflow);

}
