//
// Created by bernardoct on 7/9/17.
//

#include <algorithm>
#include <iostream>
#include <numeric>
#include "AllocatedReservoir.h"


AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries &evaporation_series, DataSeries *storage_area_curve,
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions, vector<double> *allocated_treatment_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area_curve,
                    allocated_treatment_fractions,
                    allocated_fractions,
                    utilities_with_allocations,
                    ALLOCATED_RESERVOIR),
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED) {
}

AllocatedReservoir::AllocatedReservoir(const char *name, const int id, const vector<Catchment *> &catchments,
                                       const double capacity, const double max_treatment_capacity,
                                       EvaporationSeries &evaporation_series, DataSeries *storage_area_curve,
                                       const vector<double> &construction_time_range, double permitting_period,
                                       Bond &bond, vector<int> *utilities_with_allocations,
                                       vector<double> *allocated_fractions,
                                       vector<double> *allocated_treatment_fractions)
        : Reservoir(name, id, catchments, capacity, max_treatment_capacity, evaporation_series, storage_area_curve,
                    allocated_treatment_fractions, allocated_fractions, utilities_with_allocations,
                    construction_time_range, permitting_period, bond, ALLOCATED_RESERVOIR),
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED) {
}

AllocatedReservoir::AllocatedReservoir(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double capacity, const double max_treatment_capacity,
        EvaporationSeries &evaporation_series, double storage_area,
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions, vector<double>
        *allocated_treatment_fractions)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area,
                    allocated_treatment_fractions,
                    allocated_fractions,
                    utilities_with_allocations,
                    ALLOCATED_RESERVOIR),
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED) {
}

AllocatedReservoir::AllocatedReservoir(const char *name, const int id, const vector<Catchment *> &catchments,
                                       const double capacity, const double max_treatment_capacity,
                                       EvaporationSeries &evaporation_series, double storage_area,
                                       const vector<double> &construction_time_range, double permitting_period,
                                       Bond &bond, vector<int> *utilities_with_allocations,
                                       vector<double> *allocated_fractions,
                                       vector<double> *allocated_treatment_fractions)
        : Reservoir(name, id, catchments, capacity, max_treatment_capacity, evaporation_series, storage_area,
                    allocated_treatment_fractions, allocated_fractions, utilities_with_allocations,
                    construction_time_range, permitting_period, bond, ALLOCATED_RESERVOIR),
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED) {
}

AllocatedReservoir::AllocatedReservoir(
        const AllocatedReservoir &allocated_reservoir)
        : Reservoir(allocated_reservoir), has_water_quality_pool(allocated_reservoir.has_water_quality_pool) {}

/**
 * Copy assignment operator
 * @param allocated_reservoir
 * @return
 */
AllocatedReservoir &AllocatedReservoir::operator=(
        const AllocatedReservoir &allocated_reservoir) {
    return *this;
};

AllocatedReservoir::~AllocatedReservoir() = default;


void AllocatedReservoir::applyContinuity(int week, double upstream_source_inflow,
                                         double wastewater_inflow, vector<double> &demand_outflow) {

    double total_upstream_inflow;

    total_upstream_inflow = upstream_source_inflow +
            wastewater_inflow;

    this->upstream_source_inflow = upstream_source_inflow;
    this->wastewater_inflow = wastewater_inflow;

    double available_volume_old = available_volume;
    
    total_demand = 0.0;
    for (double i : demand_outflow) {
        total_demand += i;
    }

    double direct_demand = total_demand;


    /// Calculate total runoff inflow reaching reservoir from its watershed.
    upstream_catchment_inflow = 0;
    for (Catchment &c : catchments)
        upstream_catchment_inflow += c.getStreamflow(week);

    /// Calculates water lost through evaporation.
    evaporated_volume =
            (fixed_area ? area * evaporation_series.getEvaporation(week) :
             storage_area_curve->get_dependent_variable(available_volume) *
             evaporation_series.getEvaporation(week));

    /// Calculate new stored volume and outflow based on continuity.
    double available_volume_new = available_volume
                                  + total_upstream_inflow
                                  + upstream_catchment_inflow
                                  - total_demand
                                  - min_environmental_outflow
                                  - evaporated_volume;
    double outflow_new = min_environmental_outflow;
    total_outflow = outflow_new;

    /// Check if spillage is needed and, if so, correct stored volume and
    /// calculate spillage and set all allocations to full. Otherwise,
    /// distributed inflows and outflows among respective allocations.
    if (available_volume_new > capacity) {
        for (int u : *utilities_with_allocations)
            available_allocated_volumes[u] = this->capacity *
                                             allocated_fractions[u];
        total_outflow = outflow_new + available_volume_new - capacity;
        available_volume = capacity;
    } else {
            available_volume = available_volume_new;
            /// Water exceeding allocated capacities.
            double excess_allocated_water = 0.f;
            double fraction_needing_water = 0.f;

            /// Volume of water that entered the reservoir and stayed until being
            /// used or released.
            double net_inflow = upstream_catchment_inflow +
                                total_upstream_inflow - evaporated_volume;

            bool overallocation;
            if (has_water_quality_pool) {
                overallocation = mass_balance_with_wq_pool(net_inflow,
                                                           demand_outflow);
            } else {
                overallocation = mass_balance_without_wq_pool(net_inflow,
                                                              demand_outflow);
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
                                     / fraction_needing_water);

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

    total_demand += policy_added_demand;

    /// Sanity checking from now on.
    double sum_allocations = accumulate(available_allocated_volumes.begin(),
                                        available_allocated_volumes.end(),
                                        0.f);

    double cont_error = abs(available_volume_old - direct_demand + total_upstream_inflow +
                            upstream_catchment_inflow - evaporated_volume -
                            total_outflow - available_volume);

    if ((int) abs(sum_allocations - available_volume) > 1) {
        char error[4000];
        sprintf(error, "Sum of allocated volumes in a reservoir must \n"
                        "total current storage minus unallocated \n"
                        "volume.Please report this error to \n"
                        "bct52@cornell.edu.\n\n"
                        "week: %d\nsum_allocations: %f\n"
                        "available_volume_old: %f\navailable_volume %f\n"
                        "total_upstream_inflow: %f\n"
                        "upstream_catchment_inflow: %f\nevaporation: %f\n"
                        "total_demand: %f\npolicy_added_demand: %f\n"
                        "total_outflow: %f\ncontinuity error: %f\n"
                        "demands_0: %f\n"
                        "demands_1: %f\n"
                        "demands_2: %f\n"
                        "demands_3: %f\n"
                        "alloc_vols_0: %f\n"
                        "alloc_vols_1: %f\n"
                        "alloc_vols_2: %f\n"
                        "alloc_vols_3: %f\n"
                        "alloc_vols_4: %f\n",
                week, sum_allocations, available_volume_old, available_volume,
                total_upstream_inflow, upstream_catchment_inflow,
                evaporated_volume, total_demand, policy_added_demand,
                total_outflow, cont_error,
                demand_outflow[0],
                demand_outflow[1],
                demand_outflow[2],
                demand_outflow[3],
                available_allocated_volumes[0],
                available_allocated_volumes[1],
                available_allocated_volumes[2],
                available_allocated_volumes[3],
                available_allocated_volumes[4]);

	__throw_runtime_error(error);
//        throw_with_nested(runtime_error(error));
    }

    if (abs(cont_error) > 1.f || available_volume < 0 || sum_allocations < 0) {
        char error[4000];
        sprintf(error, "Continuity error in %s\n\n"
                        "week: %d\nsum_allocations: %f\n"
                        "available_volume_old: %f\navailable_volume %f\n"
                        "total_upstream_inflow: %f\n"
                        "upstream_catchment_inflow: %f\nevaporation: %f\n"
                        "total_demand: %f\npolicy_added_demand: %f\n"
                        "total_outflow: %f\ncontinuity error: %f\n"
                        "demands_0: %f\n"
                        "demands_1: %f\n"
                        "demands_2: %f\n"
                        "demands_3: %f\n"
                        "alloc_vols_0: %f\n"
                        "alloc_vols_1: %f\n"
                        "alloc_vols_2: %f\n"
                        "alloc_vols_3: %f\n"
                        "alloc_vols_4: %f\n",
                name, week, sum_allocations, available_volume_old, available_volume,
                total_upstream_inflow, upstream_catchment_inflow,
                evaporated_volume, total_demand, policy_added_demand,
                total_outflow, cont_error,
                demand_outflow[0],
                demand_outflow[1],
                demand_outflow[2],
                demand_outflow[3],
                available_allocated_volumes[0],
                available_allocated_volumes[1],
                available_allocated_volumes[2],
                available_allocated_volumes[3],
                available_allocated_volumes[4]);

	__throw_runtime_error(error);
//        throw_with_nested(runtime_error(error));
    }

    policy_added_demand = 0;
}

void AllocatedReservoir::addCapacity(double capacity) {
    WaterSource::addCapacity(capacity);

    for (int i : *utilities_with_allocations)
        allocated_capacities[i] += capacity * allocated_fractions[i];
}

/**
 * Addes treatment capacity to a source. The specification of both the total
 * treatment capacity of the new plant and the fraction of the treatment
 * capacity allocated to a given utility allow for joint treatment plants. To
 * have one utility only building an exclusive plant, the fraction will be 1.
 * @param added_plant_treatment_capacity
 * @param allocated_fraction_of_total_capacity
 * @param utility_id
 */
void AllocatedReservoir::addTreatmentCapacity(const double added_plant_treatment_capacity, int utility_id) {
    WaterSource::addTreatmentCapacity(added_plant_treatment_capacity, utility_id);

    /// Add capacity to respective treatment allocation.
    allocated_treatment_capacities[utility_id] +=
            added_plant_treatment_capacity;

    /// Update treatment allocation fractions based on new allocated amounts
    /// and new total treatment capacity.
    for (int i = 0; i < (int) utilities_with_allocations->size(); ++i) {
        allocated_treatment_fractions[i] = allocated_treatment_capacities[i]
                                           / total_treatment_capacity;
    }
}

bool AllocatedReservoir::mass_balance_with_wq_pool(double net_inflow,
                                                     vector<double>
                                                     &demand_outflow) {
    bool overallocation = false;
    int u;
    for (int i = 0; i < (int) utilities_with_allocations->size() - 1; ++i) {
        u = (*utilities_with_allocations)[i];
        /// Split inflows and evaporation among allocations and
        /// subtract demands.
        available_allocated_volumes[u] +=
                net_inflow * allocated_fractions[u] -
                demand_outflow[u];

        /// Flag the occurrence of an allocation exceeding its capacity
        if (!overallocation)
            overallocation = available_allocated_volumes[u] >
                             allocated_capacities[u];
    }

    /// the water quality pool has no demand and provided the
    /// minimum environmental flows.
    u = utilities_with_allocations->back();
    available_allocated_volumes[u] +=
            net_inflow * allocated_fractions[u] -
            min_environmental_outflow;

    /// Flag the occurrence of an allocation exceeding its capacity
    if (!overallocation)
        overallocation = available_allocated_volumes[u] >
                         allocated_capacities[u];

    return overallocation;
}

bool AllocatedReservoir::mass_balance_without_wq_pool(double net_inflow,
                                                   vector<double>
                                                   &demand_outflow) {
    bool overallocation = false;
    net_inflow -= min_environmental_outflow;
    for (int u : *utilities_with_allocations) {
        /// Split inflows, min environmental outflows and evaporation among
        /// allocations and subtract demands.
        available_allocated_volumes[u] +=
                net_inflow * allocated_fractions[u] -
                demand_outflow[u];

        /// Flag the occurrence of an allocation exceeding its capacity
        if (!overallocation)
            overallocation = available_allocated_volumes[u] >
                             allocated_capacities[u];
    }

    return overallocation;
}

double AllocatedReservoir::getAvailableAllocatedVolume(int utility_id) {
    return available_allocated_volumes[utility_id];
}

void AllocatedReservoir::removeWater(int allocation_id, double volume) {
    available_allocated_volumes[allocation_id] -= volume;
    available_volume -= volume;
    total_demand += volume;
    policy_added_demand += volume;
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

double AllocatedReservoir::getAllocatedTreatmentCapacity(int utility_id) const {
    if (utility_id == WATER_QUALITY_ALLOCATION)
        __throw_invalid_argument("Water quality pool does not have allocated treatment "
                                     "capacity.");
    return allocated_treatment_capacities[utility_id];
}

double AllocatedReservoir::getSupplyAllocatedFraction(int utility_id) {
    if (utility_id == WATER_QUALITY_ALLOCATION)
        __throw_invalid_argument("Water quality pool allocation fraction cannot "
                                         "be used for supply.");
    return supply_allocated_fractions[utility_id];
}
