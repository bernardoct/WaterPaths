//
// Created by bernardoct on 7/9/17.
//

#include <algorithm>
#include "AllocatedReservoir.h"


AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, DataSeries *storage_area_curve,
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area_curve,
                    ALLOCATED_RESERVOIR),
          utilities_with_allocations(utilities_with_allocations) {
    setAllocations(utilities_with_allocations,
                   allocated_fractions);
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, DataSeries *storage_area_curve,
        const double construction_rof,
        const vector<double> &construction_time_range, double construction_cost,
        double bond_term, double bond_interest_rate,
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions)
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
                    bond_interest_rate,
                    ALLOCATED_RESERVOIR),
          utilities_with_allocations(utilities_with_allocations) {
    setAllocations(utilities_with_allocations,
                   allocated_fractions);
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area,
                    ALLOCATED_RESERVOIR),
          utilities_with_allocations(utilities_with_allocations) {
    setAllocations(utilities_with_allocations,
                   allocated_fractions);
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        const double construction_rof,
        const vector<double> &construction_time_range, double construction_cost,
        double bond_term, double bond_interest_rate,
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions)
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
                    bond_interest_rate,
                    ALLOCATED_RESERVOIR),
          utilities_with_allocations(utilities_with_allocations) {
    setAllocations(utilities_with_allocations,
                   allocated_fractions);
}

/**
 * Initial set up of allocations with full reservoir in the beginning of the
 * simulations. To be used in constructors only.
 * @param utilities_with_allocations
 * @param allocated_fractions
 */
void AllocatedReservoir::setAllocations(
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions) {
    if (utilities_with_allocations->size() != allocated_fractions->size())
        __throw_invalid_argument("There must be one allocation fraction in "
                                         "utilities_with_allocations for "
                                         "each utility id in "
                                         "allocated_fractions.");

    total_allocated_fraction = accumulate(allocated_fractions->begin(),
                                          allocated_fractions->end(),
                                          0.);

    if (total_allocated_fraction < 1.0)
        for (int i = 0; i < utilities_with_allocations->size(); ++i)
            if ((*utilities_with_allocations)[i] == WATER_QUALITY_ALLOCATION)
                (*allocated_fractions)[i] += 1. - total_allocated_fraction;

    /// Have water quality pool as a reservoir with ID next to highest ID
    /// allocation.
    wq_pool_id = *std::max_element(utilities_with_allocations->begin(),
                                   utilities_with_allocations->end()) + 1;

    this->allocated_fractions = new double[wq_pool_id + 1]();
    available_allocated_volumes = new double[wq_pool_id + 1]();
    allocated_capacities = new double[wq_pool_id + 1]();
    unallocated_volume = capacity;

    /// Populate vectors.
    for (unsigned long i = 0; i < utilities_with_allocations->size(); ++i) {
        int u = utilities_with_allocations->at(i);
        u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);
        this->allocated_fractions[u] = (*allocated_fractions)[i];
        (*this->utilities_with_allocations)[i] = u;
        allocated_capacities[u] = this->capacity * (*allocated_fractions)[i];
        available_allocated_volumes[u] = allocated_capacities[u];
        unallocated_volume -= allocated_capacities[u];
    }
}

AllocatedReservoir::AllocatedReservoir(
        const AllocatedReservoir &allocated_reservoir)
        : Reservoir(allocated_reservoir),
          utilities_with_allocations(allocated_reservoir
                                             .utilities_with_allocations),
          allocated_fractions(allocated_reservoir.allocated_fractions) {

    wq_pool_id = allocated_reservoir.wq_pool_id;
    unallocated_volume = allocated_reservoir.unallocated_volume;

    available_allocated_volumes = new double[wq_pool_id + 1];
    allocated_capacities = new double[wq_pool_id + 1];

    std::copy(allocated_reservoir.available_allocated_volumes,
              allocated_reservoir.available_allocated_volumes + wq_pool_id + 1,
              available_allocated_volumes);

    std::copy(allocated_reservoir.allocated_capacities,
              allocated_reservoir.allocated_capacities + wq_pool_id + 1,
              allocated_capacities);
}

/**
 * Copy assignment operator
 * @param allocated_reservoir
 * @return
 */
AllocatedReservoir &
AllocatedReservoir::operator=(
        const AllocatedReservoir &allocated_reservoir) {

    int length = *std::max_element(utilities_with_allocations->begin(),
                                   utilities_with_allocations->end()) + 1;

    available_allocated_volumes = new double[length];
    allocated_capacities = new double[length];

    std::copy(available_allocated_volumes,
              available_allocated_volumes + length,
              allocated_reservoir.available_allocated_volumes);

    std::copy(allocated_capacities,
              allocated_capacities + length,
              allocated_reservoir.allocated_capacities);

    return *this;
};

AllocatedReservoir::~AllocatedReservoir() {}

void AllocatedReservoir::applyContinuity(
        int week, double upstream_source_inflow, vector<double> *demand_outflow,
        int n_utilities) {

    total_demand = std::accumulate(demand_outflow->begin(),
                                   demand_outflow->end(),
                                   0.);

    /// Calculate total runoff inflow reaching reservoir from its watershed.
    upstream_catchment_inflow = 0;
    for (Catchment *c : catchments)
        upstream_catchment_inflow += c->getStreamflow(week);

    /// Calculates water lost through evaporation.
    evaporated_volume =
            (fixed_area ? area * evaporation_series->getEvaporation(week) :
             storage_area_curve->get_dependent_variable(available_volume) *
             evaporation_series->getEvaporation(week));

    /// Calculate new stored volume and outflow based on continuity.
    double available_volume_new = available_volume
                                  + upstream_source_inflow +
                                  upstream_catchment_inflow
                                  - total_demand - min_environmental_outflow
                                  - evaporated_volume;
    double outflow_new = min_environmental_outflow;


    /// Check if spillage is needed and, if so, correct stored volume and
    /// calculate spillage and set all allocations to full. Otherwise,
    /// distributed inflows and outflows among respective allocations.
    if (available_volume_new > capacity) {
        for (int u : *utilities_with_allocations)
            available_allocated_volumes[u] = this->capacity *
                                             allocated_fractions[u];
        total_outflow = outflow_new + available_volume_new - capacity;
        available_volume = capacity;
        this->upstream_source_inflow = upstream_source_inflow;
    } else {

        available_volume = available_volume_new;
        /// Water exceeding allocated capacities.
        double excess_allocated_water = 0.;
        double fraction_needing_water = 0.;
        bool overallocation = false;

        /// Volume of water that entered the reservoir and stayed until being
        /// used or released.
        double net_inflow = upstream_catchment_inflow +
                            upstream_source_inflow - evaporated_volume;

        for (int u : *utilities_with_allocations) {
            if (u != wq_pool_id) {
                /// Split inflows and evaporation among allocations and
                /// subtract demands.
                available_allocated_volumes[u] +=
                        net_inflow * allocated_fractions[u] -
                        (*demand_outflow)[u];
            } else {
                /// the water quality pool has no demand and provided the
                /// minimum environmental flows.
                available_allocated_volumes[u] +=
                        net_inflow * allocated_fractions[u] -
                        min_environmental_outflow;
            }

            /// Flag the occurrence of an allocation exceeding its capacity
            if (!overallocation)
                overallocation = available_allocated_volumes[u] >
                                 allocated_capacities[u];
        }

        /// Split among utilities any allocated volume exceeding a giver
        /// utility's maximum allocated capacity.
        if (overallocation) {
            /// Calculate combined excess allocation and combined allocation
            /// fraction.
            for (int u : *utilities_with_allocations) {
                /// Calculation of amount exceeding all individual allocations
                if (available_allocated_volumes[u] >=
                    allocated_capacities[u]) {
                    excess_allocated_water += available_allocated_volumes[u]
                                              - allocated_capacities[u];
                    available_allocated_volumes[u] = allocated_capacities[u];
                } else
                    fraction_needing_water += allocated_fractions[u];
            }

            /// Redistribute combined excess among utilities based on their
            /// allocation fractions. If one is exceeded, roll "second order"
            /// excess down to the next utility.
            double rellocation_excess = 0;
            while (excess_allocated_water > 0 &&
                   fraction_needing_water > 0) {
                for (int u : *utilities_with_allocations) {
                    /// Redistribute excess based on allocations fractions.
                    if (available_allocated_volumes[u] <
                        allocated_capacities[u]) {
                        available_allocated_volumes[u] +=
                                excess_allocated_water *
                                (allocated_fractions[u]
                                 /
                                 fraction_needing_water);

                        /// Check if redistribution of excesses didn't make an
                        /// allocation exceed its capacity.
                        if (available_allocated_volumes[u] >
                            allocated_capacities[u]) {
                            rellocation_excess += (
                                    available_allocated_volumes[u]
                                    - allocated_capacities[u]);
                            available_allocated_volumes[u] = allocated_capacities[u];
                        }
                    }
                }

                /// If there was too much water added to an allocation,
                /// redistribute it.
                excess_allocated_water = rellocation_excess;
                rellocation_excess = 0;
                fraction_needing_water = 0;
                for (int u : *utilities_with_allocations) {
                    if (available_allocated_volumes[u] <
                        allocated_capacities[u])
                        fraction_needing_water += allocated_fractions[u];
                }
            }

            /// If there's still an excess but all allocations are at capacity,
            /// release the rest as environmental flows.
            for (int u : *utilities_with_allocations) {
                if (available_allocated_volumes[u] >
                    allocated_capacities[u]) {
                    total_outflow += available_allocated_volumes[u] -
                                     allocated_capacities[u];
                    available_allocated_volumes[u] = allocated_capacities[u];
                }
            }
        }
    }

    for (int u : *utilities_with_allocations)
        if (isnan(available_allocated_volumes[u]) ||
            available_allocated_volumes[u] > allocated_capacities[u])
            __throw_logic_error("Available allocated volume for utility is "
                                        "nan or greater than capacity. Please "
                                        "report this error to  bct52@cornell"
                                        ".edu.");
    double sum_allocations = accumulate(available_allocated_volumes,
                                        available_allocated_volumes +
                                        wq_pool_id + 1,
                                        0.);
    if ((int) std::round(sum_allocations) !=
        (int) std::round(available_volume - unallocated_volume))
        __throw_logic_error("Sum of allocated volumes in a reservoir must "
                                    "total current storage minus unallocated "
                                    "volume");
}

double AllocatedReservoir::getAvailableAllocatedVolume(int utility_id) {
    return available_allocated_volumes[utility_id];
}

void AllocatedReservoir::removeWater(int allocation_id, double volume) {
    available_allocated_volumes[allocation_id] -= volume;
    available_volume -= volume;
    total_demand += volume;
}

double AllocatedReservoir::getAllocatedCapacity(int utility_id) {
    return allocated_capacities[utility_id];
}

void AllocatedReservoir::setFull() {
    WaterSource::setFull();
    for (int u : *utilities_with_allocations)
        available_allocated_volumes[u] = allocated_capacities[u];
}

double AllocatedReservoir::getAllocatedFraction(int utility_id) {
    return allocated_fractions[utility_id];
}


