//
// Created by bernardoct on 5/3/17.
//

#include "Quarry.h"

Quarry::Quarry(const string &source_name, const int id, const double min_environmental_outflow,
               const vector<Catchment *> &catchments, const double capacity, const double max_treatment_capacity,
               double max_diversion) : WaterSource(source_name, id, min_environmental_outflow,
                                                   catchments, capacity, max_treatment_capacity,
                                                   QUARRY), max_diversion(max_diversion) {}

Quarry::Quarry(const string &source_name, const int id, const double min_environmental_outflow,
               const vector<Catchment *> &catchments, const double capacity, const double raw_water_main_capacity,
               const double construction_rof, const vector<double> &construction_time_range,
               double construction_cost_of_capital, double bond_term, double bond_interest_rate, double max_diversion)
        : WaterSource(source_name, id, min_environmental_outflow, catchments, capacity, raw_water_main_capacity,
                      QUARRY, construction_rof, construction_time_range, construction_cost_of_capital, bond_term,
                      bond_interest_rate), max_diversion(max_diversion) {}

Quarry::Quarry(const Quarry &quarry, const double max_diversion) : WaterSource(quarry),
                                                                   max_diversion(max_diversion) {}

/**
 * Copy assignment operator
 * @param quarry
 * @return
 */
Quarry &Quarry::operator=(const Quarry &quarry) {
    WaterSource::operator=(quarry);
    max_diversion = quarry.max_diversion;
    return *this;
}

Quarry::~Quarry() {

}

/**
 * Reservoir mass balance. Gets releases from upstream reservoirs, demands from connected utilities, and
 * combines them with its catchments inflows.
 * @param week
 * @param upstream_source_inflow
 * @param demand_outflow
 */
void Quarry::applyContinuity(int week, double upstream_source_inflow, double demand_outflow) {

    double catchment_inflow = 0;
    for (Catchment *c : catchments) {
        catchment_inflow += c->getStreamflow((week));
    }


    double total_flow_in = upstream_source_inflow + catchment_inflow;
    double total_flow_out = demand_outflow - min_environmental_outflow;

    double diverted_flow = max(max_diversion, total_flow_in - total_flow_out);

    double stored_volume_new = available_volume + diverted_flow;
    double outflow_new = min_environmental_outflow - (total_flow_in - total_flow_out) - diverted_flow;

    if (online) {
        if (stored_volume_new > capacity) {
            outflow_new += stored_volume_new - capacity;
            stored_volume_new = capacity;
        }
    } else {
        stored_volume_new = available_volume;
        outflow_new = upstream_source_inflow + catchment_inflow;
    }

    demand = demand_outflow;
    available_volume = max(stored_volume_new, 0.0);
    total_outflow = outflow_new;
    this->upstream_source_inflow = upstream_source_inflow;
    upstream_catchment_inflow = catchment_inflow;
}

void Quarry::setOnline() {
    WaterSource::setOnline();

    /// start empty and gradually fill as inflows start coming in.
    available_volume = NONE;
}
