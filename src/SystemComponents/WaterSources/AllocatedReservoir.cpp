//
// Created by bernardoct on 7/9/17.
//

#include <algorithm>
#include "AllocatedReservoir.h"

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, DataSeries *storage_area_curve,
        const vector<int> *utilities_with_allocations,
        const vector<double> *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area_curve),
          utilities_with_allocations(utilities_with_allocations),
          allocated_fractions(allocated_fractions->data()) {
    int length = *std::max_element(utilities_with_allocations->begin(),
                                   utilities_with_allocations->end());

    available_allocated_volumes = new double[length];
    allocated_capacities = new double[length];

    for (int u : *utilities_with_allocations) {
        allocated_capacities[u] =
                this->capacity * (*allocated_fractions)[u];
        available_allocated_volumes[u] = allocated_capacities[u];
    }
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, DataSeries *storage_area_curve,
        const double construction_rof,
        const vector<double> &construction_time_range, double construction_cost,
        double bond_term, double bond_interest_rate,
        const vector<int> *utilities_with_allocations,
        const vector<double> *allocated_fractions)
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
          allocated_fractions(allocated_fractions->data()) {
    int length = *std::max_element(utilities_with_allocations->begin(),
                                   utilities_with_allocations->end());

    available_allocated_volumes = new double[length];
    allocated_capacities = new double[length];

    for (int u : *utilities_with_allocations) {
        allocated_capacities[u] =
                this->capacity * (*allocated_fractions)[u];
        available_allocated_volumes[u] = 0;
    }
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        const vector<int> *utilities_with_allocations,
        const vector<double> *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area),
          utilities_with_allocations(utilities_with_allocations),
          allocated_fractions(allocated_fractions->data()) {
    int length = *std::max_element(utilities_with_allocations->begin(),
                                   utilities_with_allocations->end());

    available_allocated_volumes = new double[length];
    allocated_capacities = new double[length];

    for (int u : *utilities_with_allocations) {
        allocated_capacities[u] =
                this->capacity * (*allocated_fractions)[u];
        available_allocated_volumes[u] = 0;
    }
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        const double construction_rof,
        const vector<double> &construction_time_range, double construction_cost,
        double bond_term, double bond_interest_rate,
        const vector<int> *utilities_with_allocations,
        const vector<double> *allocated_fractions)
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
          allocated_fractions(allocated_fractions->data()) {
    int length = *std::max_element(utilities_with_allocations->begin(),
                                   utilities_with_allocations->end());

    available_allocated_volumes = new double[length];
    allocated_capacities = new double[length];

    for (int u : *utilities_with_allocations) {
        allocated_capacities[u] =
                this->capacity * (*allocated_fractions)[u];
        available_allocated_volumes[u] = allocated_capacities[u];
    }
}

AllocatedReservoir::AllocatedReservoir(const AllocatedReservoir &reservoir)
        : Reservoir(reservoir),
          utilities_with_allocations(reservoir.utilities_with_allocations),
          allocated_fractions(reservoir.allocated_fractions) {}

/**
 * Copy assignment operator
 * @param allocated_reservoir
 * @return
 */
AllocatedReservoir &
AllocatedReservoir::operator=(
        const AllocatedReservoir &allocated_reservoir) = default;

void AllocatedReservoir::applyContinuity(
        int week, double upstream_source_inflow, double *demand_outflow) {
    Reservoir::applyContinuity(week,
                               upstream_source_inflow,
                               demand_outflow);
    if (total_outflow > min_environmental_outflow)
        for (int u : *utilities_with_allocations)
            available_allocated_volumes[u] =
                    this->capacity * allocated_fractions[u];
    else {
        double upstream_inflow = upstream_catchment_inflow +
                                 upstream_source_inflow;
        for (int u : *utilities_with_allocations) {
            available_allocated_volumes[u] += (upstream_inflow -
                                               evaporated_volume) *
                                              allocated_fractions[u] +
                                              demand_outflow[u];
        }
    }
}

double AllocatedReservoir::getAvailableAllocatedVolume(int utility_id) {
    return available_allocated_volumes[utility_id];
}

const double *AllocatedReservoir::getAvailableAllocatedVolumes() const {
    return available_allocated_volumes;
}
