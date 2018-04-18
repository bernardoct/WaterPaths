//
// Created by David on 12/7/2017.
//

#include "AllocatedReservoirExpansion.h"


AllocatedReservoirExpansion::AllocatedReservoirExpansion(const char *name, const int id,
                                                         const unsigned int parent_reservoir_id,
                                                         const double new_capacity,
                                                         vector<int> *utilities_with_allocations,
                                                         vector<double> *new_allocated_fractions,
                                                         vector<double> *new_allocated_treatment_fractions,
                                                         const double construction_rof_or_demand,
                                                         const vector<double> &construction_time_range,
                                                         double permitting_period, double construction_cost)
        : ReservoirExpansion(name, id, parent_reservoir_id, new_capacity,
                             construction_time_range,
                             permitting_period, construction_cost),
          new_allocated_fractions(new_allocated_fractions),
          new_allocated_treatment_fractions(new_allocated_treatment_fractions) {

    source_type = ALLOCATED_RESERVOIR_EXPANSION;
}

/**
 * Copy constructor.
 * @param reservoir
 */
AllocatedReservoirExpansion::AllocatedReservoirExpansion(const AllocatedReservoirExpansion &allocated_reservoir_expansion) :
        ReservoirExpansion(allocated_reservoir_expansion),
        new_allocated_fractions(allocated_reservoir_expansion.new_allocated_fractions),
        new_allocated_treatment_fractions(allocated_reservoir_expansion.new_allocated_treatment_fractions){}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
AllocatedReservoirExpansion &AllocatedReservoirExpansion::operator=
        (const AllocatedReservoirExpansion &allocated_reservoir_expansion) {
    ReservoirExpansion::operator=(allocated_reservoir_expansion);
    return *this;
}

void AllocatedReservoirExpansion::applyContinuity(int week, double upstream_source_inflow,
                                         double wastewater_discharge,
                                         vector<double> &demand_outflow) {
    __throw_logic_error("Reservoir expansion only add storage volume to the "
                                "reservoir they're assigned to.  Continuity "
                                "cannot be called on it, but only on the "
                                "reservoir it's  assigned to expand.");
}

//void AllocatedReservoirExpansion::addCapacity(double new_capacity) {
//    WaterSource::addCapacity(new_capacity);
//}
//
//void AllocatedReservoirExpansion::setAllocations(vector<int> *utilities_with_allocations,
//                                                   vector<double> *new_allocated_fractions,
//                                                   vector<double> *new_allocated_treatment_fractions) {
//    WaterSource::setAllocations(utilities_with_allocations,
//                                new_allocated_fractions,
//                                new_allocated_treatment_fractions);
//}

//double AllocatedReservoirExpansion::getNewCapacity(int id) {
//    return new_capacity;
//}

vector<double>* AllocatedReservoirExpansion::getNewAllocationFractions(int id) {
    return new_allocated_fractions;
}

vector<double>* AllocatedReservoirExpansion::getNewTreatmentAllocationFractions(int id) {
    return new_allocated_treatment_fractions;
}