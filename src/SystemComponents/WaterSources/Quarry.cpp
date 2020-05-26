//
// Created by bernardoct on 5/3/17.
//

#include <numeric>
#include "Quarry.h"

Quarry::Quarry(
        string name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        const double max_treatment_capacity,
        EvaporationSeries &evaporation_series,
        DataSeries &storage_area_curve, double max_diversion)
        : Reservoir(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    evaporation_series,
                    storage_area_curve,
                    QUARRY),
          max_diversion(max_diversion) {}

Quarry::Quarry(string name, const int id, const vector<Catchment *> &catchments,
               const double capacity,
               const double max_treatment_capacity,
               EvaporationSeries &evaporation_series,
               DataSeries &storage_area_curve,
               vector<int> construction_prerequisites,
               const vector<double> &construction_time_range,
               double permitting_period, Bond &bond, double max_diversion)
        : Reservoir(name, id, catchments, capacity, max_treatment_capacity, evaporation_series, storage_area_curve,
                    construction_prerequisites,
                    construction_time_range, permitting_period, bond, QUARRY), max_diversion(max_diversion) {}

Quarry::Quarry(
        string name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        const double max_treatment_capacity,
        EvaporationSeries &evaporation_series, double storage_area,
        double max_diversion) : Reservoir(name,
                                          id,
                                          catchments,
                                          capacity,
                                          max_treatment_capacity,
                                          evaporation_series,
                                          storage_area,
                                          QUARRY),
                                max_diversion(max_diversion) {}

Quarry::Quarry(string name, const int id, const vector<Catchment *> &catchments,
               const double capacity,
               const double max_treatment_capacity,
               EvaporationSeries &evaporation_series, double storage_area,
               vector<int> construction_prerequisites,
               const vector<double> &construction_time_range, double permitting_period, Bond &bond,
               double max_diversion)
        : Reservoir(name, id, catchments, capacity, max_treatment_capacity, evaporation_series, storage_area,
                    construction_prerequisites,
                    construction_time_range, permitting_period, bond, QUARRY), max_diversion(max_diversion) {}

Quarry::Quarry(const Quarry &quarry, const double max_diversion) :
        Reservoir(quarry), max_diversion(max_diversion) {}

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

Quarry::~Quarry() {}

/**
 * Reservoir mass balance. Gets releases from upstream reservoirs, demands from
 * connected utilities, and
 * combines them with its catchments inflows.
 * @param week
 * @param upstream_source_inflow
 * @param demand_outflow
 */
void Quarry::applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_inflow,
                             vector<double> &demand_outflow) {

    double total_upstream_inflow = upstream_source_inflow +
                                   wastewater_inflow;
    this->upstream_source_inflow = upstream_source_inflow;
    this->wastewater_inflow = wastewater_inflow;

    total_demand = 0;
    for (int i = 0; i < (int) demand_outflow.size(); ++i) {
        total_demand += demand_outflow[i];
    }

    double catchment_inflow = 0.0;
    for (Catchment c : catchments) {
        catchment_inflow += c.getStreamflow((week));
    }

    double total_inflow = total_upstream_inflow + catchment_inflow;
    total_outflow = total_demand + min_environmental_outflow;

    diverted_flow = min(max_diversion,
                        total_inflow -
                        min_environmental_outflow);

    double stored_volume_new = available_volume + diverted_flow -
                               total_demand;
    double outflow_new = total_inflow - diverted_flow;

    if (online) {
        if (stored_volume_new > capacity) {
            outflow_new += stored_volume_new - capacity;
            diverted_flow -= stored_volume_new - capacity;
            stored_volume_new = capacity;
        }
    } else {
        stored_volume_new = available_volume;
        outflow_new = upstream_source_inflow + catchment_inflow;
    }

    this->total_demand = total_demand;
    available_volume = max(stored_volume_new, 0.0);
    total_outflow = outflow_new + policy_added_demand;
    policy_added_demand = 0.0;
    upstream_catchment_inflow = catchment_inflow;
    this->upstream_source_inflow = upstream_source_inflow;
    this->wastewater_inflow = wastewater_inflow;
}

void Quarry::setOnline() {
    WaterSource::setOnline();

    /// start empty and gradually fill as inflows start coming in.
    available_volume = NONE;
}
