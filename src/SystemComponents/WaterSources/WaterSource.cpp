//
// Created by bernardo on 1/22/17.
//

#include <iostream>
#include <cmath>
#include "WaterSource.h"

using namespace std;

/**
 * Constructor for when water source is built and operational.
 * @param source_name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param max_treatment_capacity
 * @param source_type
 */
WaterSource::WaterSource(const string &source_name, const int id, const double min_environmental_outflow,
                         const vector<Catchment *> &catchments, const double capacity,
                         const double max_treatment_capacity, const int source_type)
        : name(source_name), capacity(capacity), min_environmental_outflow(min_environmental_outflow),
          total_outflow(min_environmental_outflow), catchments(catchments), online(ONLINE), available_volume(capacity),
          id(id), max_treatment_capacity(max_treatment_capacity), source_type(source_type),
          construction_rof(NON_INITIALIZED), construction_time(NON_INITIALIZED),
          construction_cost_of_capital(NON_INITIALIZED), bond_term(NON_INITIALIZED),
          bond_interest_rate(NON_INITIALIZED) {}

/**
 * Constructor for when water source does not exist in the beginning of the simulation.
 * @param source_name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param max_treatment_capacity
 * @param source_type
 * @param construction_rof
 * @param construction_time_range
 * @param construction_cost_of_capital
 */
WaterSource::WaterSource(const string &source_name, const int id, const double min_environmental_outflow,
                         const vector<Catchment *> &catchments, const double capacity,
                         const double max_treatment_capacity, const int source_type, const double construction_rof,
                         const vector<double> construction_time_range, double construction_cost_of_capital,
                         double bond_term, double bond_interest_rate)
        : name(source_name), capacity(capacity), min_environmental_outflow(min_environmental_outflow),
          total_outflow(min_environmental_outflow), catchments(catchments), online(OFFLINE), available_volume(capacity),
          id(id), max_treatment_capacity(max_treatment_capacity), source_type(source_type),
          construction_rof(construction_rof),
          construction_time(construction_time_range[0] * WEEKS_IN_YEAR + (construction_time_range[1] -
                  construction_time_range[0]) * (rand() % (int) WEEKS_IN_YEAR)),
          construction_cost_of_capital(construction_cost_of_capital), bond_term(bond_term),
          bond_interest_rate(bond_interest_rate) {}

/**
 * Copy constructor.
 * @param water_source
 */
WaterSource::WaterSource(const WaterSource &water_source) : capacity(water_source.capacity),
                                                            name(water_source.name),
                                                            min_environmental_outflow(
                                                                    water_source.min_environmental_outflow),
                                                            id(water_source.id),
                                                            available_volume(water_source.available_volume),
                                                            total_outflow(water_source.total_outflow),
                                                            online(water_source.online),
                                                            source_type(water_source.source_type),
                                                            upstream_min_env_inflow(
                                                                    water_source.upstream_min_env_inflow),
                                                            upstream_catchment_inflow(
                                                                    water_source.upstream_catchment_inflow),
                                                            max_treatment_capacity(
                                                                    water_source.max_treatment_capacity),
                                                            construction_rof(water_source.construction_rof),
                                                            construction_time(
                                                                    water_source.construction_time),
                                                            construction_cost_of_capital(
                                                                    water_source.construction_cost_of_capital),
                                                            bond_term(water_source.bond_term),
                                                            bond_interest_rate(water_source.bond_interest_rate) {
    catchments.clear();
    for (Catchment *c : water_source.catchments) {
        catchments.push_back(new Catchment(*c));
    }
}

/**
 * Destructor.
 */
WaterSource::~WaterSource() {
//    catchments.clear();
}

/**
 * Copy assignment operator.
 * @param water_source
 * @return
 */
WaterSource &WaterSource::operator=(const WaterSource &water_source) {

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
 * Applies continuity to the water source.
 * @param week
 * @param upstream_source_inflow Total inflow released from the upstream water source, excluding water for the
 * catchment between both water sources.
 * @param demand_outflow demand from utility.
 */
void WaterSource::continuityWaterSource(int week, double upstream_source_inflow, double demand_outflow) {
    if (online)
        applyContinuity(week, upstream_source_inflow, demand_outflow);
    else
        bypass(week, upstream_source_inflow);
}

/**
 * Does not apply continuity to the water source, by instead just treats it as non existing,
 * i.e. outflow = inflow + catchment_flow
 * @param week
 * @param upstream_source_inflow Total inflow released from the upstream water source, excluding water for the
 * catchment between both water sources.
 */
void WaterSource::bypass(int week, double upstream_source_inflow) {

    upstream_catchment_inflow = 0;
    for (Catchment *c : catchments) {
        upstream_catchment_inflow += c->getStreamflow((week));
    }

    demand = NONE;
    available_volume = NONE;
    total_outflow = upstream_catchment_inflow + upstream_source_inflow;
    this->upstream_source_inflow = upstream_source_inflow;
}

/**
 * Calculates the net present cost of the infrastructure options (water source) as a bond structured with level debt
 * service payments, issued on a certain week.
 * @param week
 * @param discount_rate
 * @return
 */
double WaterSource::calculateNetPresentCost(int week, double discount_rate) const {
    double rate = bond_interest_rate / BOND_INTEREST_PAYMENTS_PER_YEAR;
    double principal = construction_cost_of_capital;
    double n_payments = bond_term * BOND_INTEREST_PAYMENTS_PER_YEAR;

    /// Level debt service payment value
    double level_debt_service_payment = principal * (rate * pow(1 + rate, n_payments)) /
            (pow(1 + rate, n_payments) - 1);

    /// Net present cost of stream of level debt service payments for the whole bond term, at the time of issuance.
    double net_present_cost_at_issuance = level_debt_service_payment * (1 - pow(1 + discount_rate, -n_payments)) / discount_rate;

    /// Return NPC discounted from the time of issuance to the present.
    return net_present_cost_at_issuance / pow(1 + discount_rate, week / WEEKS_IN_YEAR);
}

double WaterSource::getAvailable_volume() const {
    return available_volume;
}

bool WaterSource::isOnline() const {
    return online;
}

void WaterSource::setAvailable_volume(double available_volume) {
    WaterSource::available_volume = available_volume;
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

double WaterSource::getDemand() const {
    return demand;
}

double WaterSource::getUpstream_source_inflow() const {
    return upstream_source_inflow;
}

double WaterSource::getCatchment_upstream_catchment_inflow() const {
    return upstream_catchment_inflow;
}
