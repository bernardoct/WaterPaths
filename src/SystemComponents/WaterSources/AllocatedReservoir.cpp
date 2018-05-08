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
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED),
          modified_allocations(false), allocation_modifier(nullptr) {

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
          modified_allocations(false), allocation_modifier(nullptr),
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
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED),
          modified_allocations(false), allocation_modifier(nullptr) {

}

AllocatedReservoir::AllocatedReservoir(const char *name, const int id, const vector<Catchment *> &catchments,
                                       const double capacity, const double max_treatment_capacity,
                                       EvaporationSeries &evaporation_series, double storage_area,
                                       vector<int> *utilities_with_allocations, vector<double> *allocated_fractions,
                                       vector<double> *allocated_treatment_fractions,
                                       AllocationModifier *allocation_modifier)
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
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED),
          modified_allocations(true), allocation_modifier(allocation_modifier) {

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
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED),
          modified_allocations(false), allocation_modifier(nullptr) {
}

AllocatedReservoir::AllocatedReservoir(const char *name, const int id, const vector<Catchment *> &catchments,
                                       const double capacity, const double max_treatment_capacity,
                                       EvaporationSeries &evaporation_series, DataSeries *storage_area_curve,
                                       const vector<double> &construction_time_range, double permitting_period,
                                       Bond &bond,
                                       vector<int> *utilities_with_allocations, vector<double> *allocated_fractions,
                                       vector<double> *allocated_treatment_fractions,
                                       AllocationModifier *allocation_modifier)
        : Reservoir(name, id, catchments, capacity, max_treatment_capacity, evaporation_series, storage_area_curve,
                    allocated_treatment_fractions, allocated_fractions, utilities_with_allocations,
                    construction_time_range, permitting_period, bond, ALLOCATED_RESERVOIR),
          has_water_quality_pool(wq_pool_id != NON_INITIALIZED),
          modified_allocations(true), allocation_modifier(allocation_modifier) {}

AllocatedReservoir::AllocatedReservoir(
        const AllocatedReservoir &allocated_reservoir)
        : Reservoir(allocated_reservoir),
          allocation_modifier(allocated_reservoir.allocation_modifier),
          modified_allocations(allocated_reservoir.modified_allocations),
          has_water_quality_pool(allocated_reservoir.has_water_quality_pool),
          member_utility_demand_outflow(allocated_reservoir.member_utility_demand_outflow) {
        }

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

    /// Collect information on split demands for transfer sales
    if (member_utility_demand_outflow.empty())
        for (int i = 0; i < demand_outflow.size(); i++) {
            member_utility_demand_outflow.push_back(demand_outflow[i]);
        }
    else
        for (int i = 0; i < demand_outflow.size(); i++) {
            member_utility_demand_outflow[i] = demand_outflow[i];
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


/**
 * Adds treatment capacity to a source. Specify the fraction of the treatment
 * capacity allocated to a given utility allow for joint treatment plants. To
 * have one utility only building an exclusive plant, the fraction will be 1.
 * @param allocated_fraction_of_total_capacity
 * @param utility_id
 */
void AllocatedReservoir::addAllocatedTreatmentCapacity(double added_allocated_treatment_capacity, int utility_id) {

    if (allocated_treatment_capacities[utility_id] + added_allocated_treatment_capacity < 0) {
        cout << "Error in AllocatedReservoir: treatment capacity re-allocation will result in negative allocated_treatment_capacity" << endl;
        cout << "If this is the first readjustment period after new source comes online, ignore this." << endl;
        added_allocated_treatment_capacity = allocated_treatment_capacities[utility_id];
    }

    /// Add capacity to respective treatment allocation.
    allocated_treatment_capacities[utility_id] += added_allocated_treatment_capacity;

    for (int i = 0; i < (int) utilities_with_allocations->size(); ++i) {
        /// Update treatment allocation fractions based on new allocated amounts
        /// and new total treatment capacity.
        allocated_treatment_fractions[i] = allocated_treatment_capacities[i] / total_treatment_capacity;
    }
}


/**
 * Sets allocated supply for a utility at a source. Specify the fraction of
 * capacity allocated to a given utility allow for joint projects. To
 * have one utility, the fraction will be 1.
 * @param capacity_allocation_fraction
 * @param utility_id
 */
void AllocatedReservoir::setAllocatedSupplyCapacity(double capacity_allocation_fraction, int utility_id) {
    /// Set respective supply allocations.
    /// capacity_allocation_fraction passed as parameter is fraction of water SUPPLY pool, not total capacity
    /// however, allocated_fractions for the utility must be as fraction of entire storage capacity
    allocated_capacities[utility_id] = capacity_allocation_fraction * capacity * (1 - base_wq_pool_fraction);
    allocated_fractions[utility_id]  = allocated_capacities[utility_id] / capacity;

    available_allocated_volumes[utility_id] = available_volume * allocated_fractions[utility_id];

    /// Allocations are normalized again in normalizeAllocatedSupplyCapacity
}


/**
 * If supply allocations have been reset, make sure that the fractions sum to 1
 * by shifting un-allocated supply capacity to the WQ pool. If over-allocated,
 * reduce the water quality pool to account for this.
 */
void AllocatedReservoir::normalizeAllocatedSupplyCapacity() {
    /// Sum the allocated capacity fractions, which should include the water quality pool
    double temp_sum_allocations = accumulate(allocated_fractions.begin(), allocated_fractions.end(), 0.0);

    /// Normalize respective supply allocations for every utility (and water quality pool).
    /// If there is unallocated supply, it goes into the WQ pool for now
//    if (temp_sum_allocations != 1) {
        cout << "Allocations in " << name
             << " do not sum to 1 (or maybe they do, I changed this). Re-allocating now..." << endl;
        if (temp_sum_allocations > 1) {
            cout << "Allocation fractions sum to greater than 1."
                 << " The water quality pool will be reduced if it is above base level. "
                 << "After that, water supply allocations will be reduced." << endl;
            double necessary_reduction = temp_sum_allocations - 1;

            if (allocated_fractions[wq_pool_id] > base_wq_pool_fraction) {
                cout << "Water quality pool fraction (" << allocated_fractions[wq_pool_id]
                     << ") is greater than the base level (" << base_wq_pool_fraction
                     << "). This will be reduced first." << endl;

                if (necessary_reduction > allocated_fractions[wq_pool_id] - base_wq_pool_fraction) {
                    allocated_fractions[wq_pool_id] = base_wq_pool_fraction;
                    necessary_reduction -= allocated_fractions[wq_pool_id] - base_wq_pool_fraction;
                } else {
                    allocated_fractions[wq_pool_id] -= necessary_reduction;
                    necessary_reduction = 0.0;
                }

                cout << "Water quality pool fraction (" << allocated_fractions[wq_pool_id]
                     << ") has been adjusted." << endl;
            }

            temp_sum_allocations = accumulate(allocated_fractions.begin(), allocated_fractions.end(), 0.0);
            double water_supply_allocation_total = temp_sum_allocations - allocated_fractions[wq_pool_id];

            double utility_fraction_of_supply;
            for (int i = 0; i < (int) utilities_with_allocations->size(); ++i) {
                int u = utilities_with_allocations->at(i);
                u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);

                cout << "Utility " << u << " fraction (before): " << allocated_fractions[u] << endl;
                if (u != wq_pool_id) {
                    utility_fraction_of_supply = allocated_fractions[u] / water_supply_allocation_total;
                    allocated_fractions[u] -= utility_fraction_of_supply * necessary_reduction;
                }
                cout << "Utility " << u << " fraction (after): " << allocated_fractions[u] << endl;
            }

            for (int i = 0; i < (int) utilities_with_allocations->size(); ++i) {
                int u = utilities_with_allocations->at(i);
                u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);

                cout << "Utility " << u << " capacity (before): " << allocated_capacities[u] << endl;

                /// Update supply allocation capacitites based on new allocated fractions
                allocated_capacities[u] = allocated_fractions[u] * capacity;
                available_allocated_volumes[u] = available_volume * allocated_fractions[u];

                (*this->utilities_with_allocations)[i] = u;

                cout << "Utility " << u << " capacity (after): " << allocated_capacities[u] << endl;
            }
        } else {
            cout << "Allocation fractions sum to less than 1. Augmenting water quality pool to fix." << endl;
            double necessary_augmentation = 1 - temp_sum_allocations;

            for (int i = 0; i < (int) utilities_with_allocations->size(); ++i) {
                int u = utilities_with_allocations->at(i);
                u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);

                cout << "Utility " << u << " fraction (before): " << allocated_fractions[u] << endl;
                cout << "Utility " << u << " capacity (before): " << allocated_capacities[u] << endl;

                if (u == wq_pool_id)
                    allocated_fractions[u] += necessary_augmentation;

                /// Update capacity allocation fractions based on new allocated amounts
                allocated_capacities[u] = allocated_fractions[u] * capacity;
                available_allocated_volumes[u] = available_volume * allocated_fractions[u];

                (*this->utilities_with_allocations)[i] = u;

                cout << "Utility " << u << " fraction (after): " << allocated_fractions[u] << endl;
                cout << "Utility " << u << " capacity (after): " << allocated_capacities[u] << endl;
            }

        }
//    }

    /// Continuity check
    double temp_sum_allocation_fractions = accumulate(allocated_fractions.begin(), allocated_fractions.end(), 0.0);
    double temp_sum_allocation_capacities = accumulate(allocated_capacities.begin(), allocated_capacities.end(), 0.0);
    if (temp_sum_allocation_fractions != 1 || temp_sum_allocation_capacities != capacity) {
        cout << "Somehow the allocations are still wrong?" << endl;
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

void AllocatedReservoir::addWater(int allocation_id, double volume) {
    if (allocation_id == WATER_QUALITY_ALLOCATION) {
        available_allocated_volumes[wq_pool_id] += volume;
    } else {
        available_allocated_volumes[allocation_id] += volume;
    }
    available_volume += volume;
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
void AllocatedReservoir::updateTreatmentAndCapacityAllocations(int week) {

    if (modified_allocations) {
        cout << "Checking allocations for modification at " << name << " in week "<< week << endl;

        for (unsigned long i = 0; i < allocation_modifier->allocation_adjustment_weeks->size(); ++i) {
            double temp = accumulate((allocation_modifier->new_treatment_capacities->at(i)).begin(),
                                     (allocation_modifier->new_treatment_capacities->at(i)).end(), 0.0);

            cout << "Current treatment capacity: " << total_treatment_capacity
                 << ", Modified capacity: " << temp
                 << ", Current treatment allocations: "
                 << "OWASA " << allocated_treatment_capacities[0]
                 << ", Durham " << allocated_treatment_capacities[1]
                 << ", Cary " << allocated_treatment_capacities[2]
                 << ", Raleigh " << allocated_treatment_capacities[3]
                 << ", WQ Pool " << allocated_treatment_capacities[4] << endl; /// SET UP FOR JORDAN LAKE

            if (allocation_modifier->allocation_adjustment_weeks->at(i) == week &
                    temp < (total_treatment_capacity + 1))
                    /// only change allocations and treatment capacity if it is time and if treatment capacities
                    /// have been expanded already to allow it
            {
                cout << "Allocations to be modified..." << endl;
                double capacity_sums = 0.0;
                for (unsigned long ii = 0; ii < utilities_with_allocations->size(); ++ii) {
                    int u = utilities_with_allocations->at(ii);
                    u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);

                    allocated_fractions[u] = (allocation_modifier->new_capacity_allocations->at(i))[u];
                    allocated_treatment_capacities[u] = (allocation_modifier->new_treatment_capacities->at(i))[u];

                    capacity_sums += (allocation_modifier->new_treatment_capacities->at(i))[u];

                    (*this->utilities_with_allocations)[ii] = u;

                    allocated_capacities[u] = capacity * allocated_fractions[u];
                    allocated_treatment_fractions[u] =  allocated_treatment_capacities[u]/total_treatment_capacity;

                    available_allocated_volumes[u] = available_volume * allocated_fractions[u];
                }

                if (capacity_sums > total_treatment_capacity) {
                    cout << "ERROR in ModifiedAllocations" << endl;
                    __throw_out_of_range("When treatment capacity is adjusted, the changes"
                                                 "cannot exceed total capacity in place");
                }
            }
        }
    }

}

double AllocatedReservoir::getAllocatedTreatmentFraction(int utility_id) const {
    return allocated_treatment_fractions[utility_id];
}

double AllocatedReservoir::getAllocatedDemand(int utility_id) {
    return member_utility_demand_outflow[utility_id];
}