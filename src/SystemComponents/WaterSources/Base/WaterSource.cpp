//
// Created by bernardo on 1/22/17.
//

#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include "WaterSource.h"

using namespace std;

/**
 * Constructor for when water source is built and operational.
 * @param name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param treatment_capacity
 * @param source_type
 */
WaterSource::WaterSource(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        double treatment_capacity, const int source_type)
        : name(name), capacity(capacity), catchments(catchments),
          online(ONLINE), available_volume(capacity), id(id),
          total_treatment_capacity(treatment_capacity),
          source_type(source_type), construction_rof_or_demand(NON_INITIALIZED),
          construction_time(NON_INITIALIZED),
          construction_cost_of_capital(NON_INITIALIZED),
          bond_term(NON_INITIALIZED), bond_interest_rate(NON_INITIALIZED),
          permitting_period(NON_INITIALIZED), highest_alloc_id(NOT_ALLOCATED) {}

/**
 * Constructor for when water source does not exist in the beginning of the simulation.
 * @param source_name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param treatment_capacity
 * @param source_type
 * @param construction_rof_or_demand
 * @param construction_time_range
 * @param construction_cost_of_capital
 */
WaterSource::WaterSource(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        double treatment_capacity, const int source_type,
        const double construction_rof_or_demand,
        const vector<double> construction_time_range,
        double permitting_period, double construction_cost_of_capital,
        double bond_term, double bond_interest_rate)
        : name(name), capacity(capacity), catchments(catchments),
          online(OFFLINE), available_volume(capacity), id(id),
          total_treatment_capacity(treatment_capacity),
          source_type(source_type),
          construction_rof_or_demand(construction_rof_or_demand),
          construction_time(
                  construction_time_range[0] * WEEKS_IN_YEAR +
                  (construction_time_range[1] -
                   construction_time_range[0]) *
                  (rand() % (int) WEEKS_IN_YEAR)),
          permitting_period(permitting_period),
          construction_cost_of_capital(construction_cost_of_capital), bond_term(bond_term),
          bond_interest_rate(bond_interest_rate), highest_alloc_id(NOT_ALLOCATED) {}


/**
 * Constructor for when water source is built and operational.
 * @param name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param treatment_capacity
 * @param source_type
 */
WaterSource::WaterSource(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        double treatment_capacity, const int source_type,
        vector<double> *allocated_treatment_fractions,
        vector<double> *allocated_fractions,
        vector<int> *utilities_with_allocations)
        : name(name), capacity(capacity), catchments(catchments),
          online(ONLINE), available_volume(capacity), id(id),
          total_treatment_capacity(treatment_capacity),
          source_type(source_type), construction_rof_or_demand(NON_INITIALIZED),
          construction_time(NON_INITIALIZED),
          construction_cost_of_capital(NON_INITIALIZED),
          bond_term(NON_INITIALIZED), bond_interest_rate(NON_INITIALIZED),
          available_allocated_volumes(nullptr),
          utilities_with_allocations(utilities_with_allocations),
          wq_pool_id(NON_INITIALIZED), permitting_period(NON_INITIALIZED) {
    setAllocations(utilities_with_allocations,
                   allocated_fractions,
                   allocated_treatment_fractions);
}

/**
 * Constructor for when water source does not exist in the beginning of the simulation.
 * @param source_name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param treatment_capacity
 * @param source_type
 * @param construction_rof_or_demand
 * @param construction_time_range
 * @param construction_cost_of_capital
 */
WaterSource::WaterSource(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        double treatment_capacity, const int source_type,
        vector<double> *allocated_treatment_fractions,
        vector<double> *allocated_fractions,
        vector<int> *utilities_with_allocations,
        const double construction_rof_or_demand,
        const vector<double> construction_time_range,
        double permitting_period, double construction_cost_of_capital,
        double bond_term, double bond_interest_rate)
        : name(name), capacity(capacity), catchments(catchments),
          online(OFFLINE), available_volume(capacity), id(id),
          total_treatment_capacity(treatment_capacity),
          source_type(source_type),
          available_allocated_volumes(nullptr),
          utilities_with_allocations(utilities_with_allocations),
          wq_pool_id(NON_INITIALIZED),
          construction_rof_or_demand(construction_rof_or_demand),
          construction_time(construction_time_range[0] * WEEKS_IN_YEAR +
                            (construction_time_range[1] -
                             construction_time_range[0]) *
                            (rand() % (int) WEEKS_IN_YEAR)),
          permitting_period(permitting_period),
          construction_cost_of_capital(construction_cost_of_capital),
          bond_term(bond_term),
          bond_interest_rate(bond_interest_rate) {
    setAllocations(utilities_with_allocations,
                   allocated_fractions,
                   allocated_treatment_fractions);
}

/**
 * Initial set up of allocations with full reservoir in the beginning of the
 * simulations. To be used in constructors only.
 * @param utilities_with_allocations
 * @param allocated_fractions
 */
void WaterSource::setAllocations(
        vector<int> *utilities_with_allocations,
        vector<double> *allocated_fractions,
        vector<double> *allocated_treatment_fractions) {
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
    highest_alloc_id = wq_pool_id;

    this->allocated_fractions = new double[wq_pool_id + 1]();
    this->allocated_treatment_fractions = new double[wq_pool_id + 1]();
    available_allocated_volumes = new double[wq_pool_id + 1]();
    allocated_capacities = new double[wq_pool_id + 1]();
    allocated_treatment_capacities = new double[wq_pool_id + 1]();

    /// Populate vectors.
    for (unsigned long i = 0; i < utilities_with_allocations->size(); ++i) {
        int u = utilities_with_allocations->at(i);

        /// Replace the -1 in the utilities_with_allocations vector with the
        /// ID assigned to the water quality pool.
        u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);

        if (u != wq_pool_id) {
            this->allocated_treatment_fractions[u] =
                    allocated_treatment_fractions->at(u);
            this->allocated_treatment_capacities[u] = total_treatment_capacity *
                    this->allocated_treatment_fractions[u];
        } else
            (*this->utilities_with_allocations)[i] = u;

        this->allocated_fractions[u] = (*allocated_fractions)[i];
        (*this->utilities_with_allocations)[i] = u;
        allocated_capacities[u] = this->capacity * (*allocated_fractions)[i];
        available_allocated_volumes[u] = allocated_capacities[u];
    }
}


/**
 * Copy constructor.
 * @param water_source
 */
WaterSource::WaterSource(const WaterSource &water_source) :
        capacity(water_source.capacity), name(water_source.name),
        min_environmental_outflow(water_source.min_environmental_outflow),
        id(water_source.id),
        available_volume(water_source.available_volume),
        total_outflow(water_source.total_outflow),
        online(water_source.online),
        source_type(water_source.source_type),
        upstream_min_env_inflow(water_source.upstream_min_env_inflow),
        upstream_catchment_inflow(water_source.upstream_catchment_inflow),
        upstream_source_inflow(water_source.upstream_source_inflow),
        total_treatment_capacity(water_source.total_treatment_capacity),
        construction_rof_or_demand(water_source.construction_rof_or_demand),
        construction_time(water_source.construction_time),
        construction_cost_of_capital(water_source.construction_cost_of_capital),
        bond_term(water_source.bond_term),
        bond_interest_rate(water_source.bond_interest_rate),
        utilities_with_allocations(water_source.utilities_with_allocations),
        permitting_period(water_source.permitting_period),
        highest_alloc_id(water_source.highest_alloc_id) {

    if (water_source.wq_pool_id != NON_INITIALIZED) {
        wq_pool_id = water_source.wq_pool_id;

        allocated_fractions = new double[wq_pool_id + 1]();
        allocated_treatment_fractions = new double[wq_pool_id + 1]();
        available_allocated_volumes = new double[wq_pool_id + 1]();
        allocated_capacities = new double[wq_pool_id + 1]();
        allocated_treatment_capacities = new double[wq_pool_id + 1]();

        int length = wq_pool_id + 1;//    *std::max_element
//            (utilities_with_allocations->begin(),
//                                   utilities_with_allocations->end()) + 1;

        std::copy(water_source.allocated_fractions,
                  water_source.allocated_fractions + length,
                  allocated_fractions);

        std::copy(water_source.allocated_treatment_fractions,
                  water_source.allocated_treatment_fractions + length,
                  allocated_treatment_fractions);

        std::copy(water_source.available_allocated_volumes,
                  water_source.available_allocated_volumes + length,
                  available_allocated_volumes);

        std::copy(water_source.allocated_capacities,
                  water_source.allocated_capacities + length,
                  allocated_capacities);

        std::copy(water_source.allocated_treatment_capacities,
                  water_source.allocated_treatment_capacities + length,
                  allocated_treatment_capacities);
    }

    catchments.clear();
    for (Catchment *c : water_source.catchments) {
        catchments.push_back(new Catchment(*c));
    }
}

/**
 * Destructor.
 */
WaterSource::~WaterSource() {
    catchments.clear();
}

/**
 * Copy assignment operator.
 * @param water_source
 * @return
 */
WaterSource &WaterSource::operator=(const WaterSource &water_source) {

    if (wq_pool_id != NON_INITIALIZED) {
        wq_pool_id = water_source.wq_pool_id;

        allocated_fractions = new double[wq_pool_id + 1]();
        allocated_treatment_fractions = new double[wq_pool_id + 1]();
        available_allocated_volumes = new double[wq_pool_id + 1]();
        allocated_capacities = new double[wq_pool_id + 1]();
        allocated_treatment_capacities = new double[wq_pool_id + 1]();

        int length = wq_pool_id + 1;//    *std::max_element
        highest_alloc_id = water_source.highest_alloc_id;
//            (utilities_with_allocations->begin(),
//                                   utilities_with_allocations->end()) + 1;

        std::copy(water_source.allocated_fractions,
                  water_source.allocated_fractions + length,
                  allocated_fractions);

        std::copy(water_source.allocated_treatment_fractions,
                  water_source.allocated_treatment_fractions + length,
                  allocated_treatment_fractions);

        std::copy(water_source.available_allocated_volumes,
                  water_source.available_allocated_volumes + length,
                  available_allocated_volumes);

        std::copy(water_source.allocated_capacities,
                  water_source.allocated_capacities + length,
                  allocated_capacities);

        std::copy(water_source.allocated_treatment_capacities,
                  water_source.allocated_treatment_capacities + length,
                  allocated_treatment_capacities);
    }

    catchments.clear();
    for (Catchment *c : water_source.catchments) {
        catchments.push_back(new Catchment(*c));
    }

    return *this;
}

/**
 * Sorting by id compare operator.
 * @param other
 * @return
 */
bool WaterSource::operator<(const WaterSource *other) {
    return id < other->id;
}

/**
 * Sorting by id compare operator.
 * @param other
 * @return
 */
bool WaterSource::operator>(const WaterSource *other) {
    return id > other->id;
}

/**
 * Sorting by id compare operator.
 * @param other
 * @return
 */
bool WaterSource::operator==(const WaterSource *other) {
    return id == other->id;
}

bool WaterSource::compare(WaterSource *lhs, WaterSource *rhs) {
    return lhs->id < rhs->id;
}

/**
 * Applies continuity to the water source.
 * @param week
 * @param upstream_source_inflow Total inflow released from the upstream
 * water source, excluding water for the catchment between both water sources.
 * @param demand_outflow demand from utility.
 */
void WaterSource::continuityWaterSource(
        int week, double upstream_source_inflow,
        vector<double> &demand_outflow) {
    if (online)
        applyContinuity(week,
                        upstream_source_inflow,
                        demand_outflow);
    else
        bypass(week, upstream_source_inflow);
}

/**
 * Does not apply continuity to the water source, by instead just treats it as
 * non existing, i.e. outflow = inflow + catchment_flow
 * @param week
 * @param upstream_source_inflow Total inflow released from the upstream water
 * source, excluding water for the
 * catchment between both water sources.
 */
void WaterSource::bypass(int week, double upstream_source_inflow) {

    upstream_catchment_inflow = 0;
    for (Catchment *c : catchments) {
        upstream_catchment_inflow += c->getStreamflow((week));
    }

    total_demand = NONE;
    available_volume = NONE;
    total_outflow = upstream_catchment_inflow + upstream_source_inflow;
    this->upstream_source_inflow = upstream_source_inflow;
}

/**
 * Calculates the net present cost of the infrastructure options (water source)
 * as a bond structured with level debt service payments, issued on a given
 * week in the future.
 * @param week
 * @param discount_rate
 * @return Net present cost
 */
double WaterSource::calculateNetPresentConstructionCost(
        int week, int utility_id, double discount_rate,
        double *level_debt_service_payment)
const {
    double rate = bond_interest_rate / BOND_INTEREST_PAYMENTS_PER_YEAR;
    double principal = construction_cost_of_capital *
                       (allocated_fractions ? allocated_fractions[utility_id]
                                            : 1.);
    double n_payments = bond_term * BOND_INTEREST_PAYMENTS_PER_YEAR;

    /// Level debt service payment value
    *level_debt_service_payment = principal * (rate * pow(1 + rate, n_payments)) /
                                  (pow(1 + rate, n_payments) - 1);

    /// Net present cost of stream of level debt service payments for the whole
    /// bond term, at the time of issuance.
    double net_present_cost_at_issuance =
            *level_debt_service_payment *
            (1 - pow(1 + discount_rate,
                     -n_payments)) / discount_rate;

    /// Return NPC discounted from the time of issuance to the present.
    return net_present_cost_at_issuance / pow(1 + discount_rate,
                                              week / WEEKS_IN_YEAR);
}

/**
 * If creating a new water source that can be allocated to different utilities,
 * this function must be overwritten to:
 * available_allocated_volumes[allocation_id] -= volume;
   available_volume -= volume;
   demand += volume;
 * @param utility_id
 * @return
 */
void WaterSource::removeWater(int allocation_id, double volume) {
    available_volume -= volume;
    total_demand += volume;
    policy_added_demand += volume;
}

void WaterSource::addCapacity(double capacity) {
    WaterSource::capacity += capacity;
}

void WaterSource::addTreatmentCapacity(
        const double added_treatment_capacity,
        double allocations_added_treatment_capacity,
        int utility_id) {
    total_treatment_capacity += added_treatment_capacity;
}

void WaterSource::setRealization(unsigned long r) {
    for (Catchment *c : catchments)
        c->setRealization(r);

    /// Update total catchment inflow, demand, and available water volume for
    /// week 0;
    this->upstream_catchment_inflow = 0;
    for (Catchment *c : catchments) {
        this->upstream_catchment_inflow = c->getStreamflow(0);
    }
}

double WaterSource::getAvailable_volume() const {
    return available_volume;
}

/**
 * If creating a new water source that can be allocated to different utilities,
 * this function must be overwritten to:
 * return available_allocated_volumes[utility_id];
 * @param utility_id
 * @return
 */
double WaterSource::getAvailableAllocatedVolume(int utility_id) {
    return getAvailable_volume();
}

bool WaterSource::isOnline() const {
    return online;
}

void WaterSource::setFull() {
    WaterSource::available_volume = capacity;
}

void WaterSource::setOutflow_previous_week(double outflow_previous_week) {
    WaterSource::total_outflow = outflow_previous_week;
}

void WaterSource::setOnline() {
    online = ONLINE;
}

double WaterSource::getTotal_outflow() const {
    return total_outflow;
}

double WaterSource::getCapacity() {
    return capacity;
}

double WaterSource::getAllocatedCapacity(int utility_id) {
    return capacity;
}

double WaterSource::getDemand() const {
    return total_demand;
}

double WaterSource::getUpstream_source_inflow() const {
    return upstream_source_inflow;
}

double WaterSource::getUpstreamCatchmentInflow() const {
    return upstream_catchment_inflow;
}

void WaterSource::setMin_environmental_outflow(
        double min_environmental_outflow) {
    WaterSource::min_environmental_outflow = min_environmental_outflow;
}

double WaterSource::getMin_environmental_outflow() const {
    return min_environmental_outflow;
}

double WaterSource::getAllocatedFraction(int utility_id) {
    return 1.0;
}

double WaterSource::getEvaporated_volume() const {
    return evaporated_volume;
}

double WaterSource::getAllocatedTreatmentCapacity(int utility_id) const {
    return total_treatment_capacity;
}

double WaterSource::getTotal_treatment_capacity(int utility_id) const {
    return total_treatment_capacity;
}

void WaterSource::resetAllocations(
        const vector<double>
        *new_allocated_fractions) {

    /// Populate vectors.
    for (unsigned long i = 0; i < utilities_with_allocations->size(); ++i) {
        int u = utilities_with_allocations->at(i);
        u = (u == WATER_QUALITY_ALLOCATION ? wq_pool_id : u);
        allocated_fractions[u] = (*new_allocated_fractions)[i];

        (*this->utilities_with_allocations)[i] = u;

        allocated_capacities[u] = capacity * (*new_allocated_fractions)[i];

        available_allocated_volumes[u] =
                available_volume * allocated_fractions[u];
    }
}

void WaterSource::setAvailableAllocatedVolumes(
        double *available_allocated_volumes, double available_volume) {

    if (utilities_with_allocations)
        std::copy(available_allocated_volumes,
              available_allocated_volumes + highest_alloc_id + 1,
              this->available_allocated_volumes);
    this->available_volume = available_volume;

    return;
}

double *WaterSource::getAvailable_allocated_volumes() const {
    return available_allocated_volumes;
}

vector<int> *WaterSource::getUtilities_with_allocations() const {
    return utilities_with_allocations;
}


