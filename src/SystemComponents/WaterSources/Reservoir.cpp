//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Reservoir.h"
#include "../../Utils/Utils.h"

using namespace std;

/**
 * Constructor for when Reservoir is built and operational.
 * @param name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param max_treatment_capacity
 * @param source_type
 */
Reservoir::Reservoir(const char *name, const int id, const double min_environmental_outflow,
                     const vector<Catchment *> &catchments, const double capacity,
                     const double max_treatment_capacity,
                     vector<vector<double>> *evaporation_time_series_all_realizations) :
        WaterSource(name, id, min_environmental_outflow, catchments, capacity, max_treatment_capacity, RESERVOIR),
        evaporation_time_series_all_realizations(evaporation_time_series_all_realizations) {}

/**
 * Constructor for when Reservoir does not exist in the beginning of the simulation.
 * @param name
 * @param id
 * @param min_environmental_outflow
 * @param catchments
 * @param capacity
 * @param max_treatment_capacity
 * @param source_type
 * @param construction_rof
 * @param construction_time_range
 * @param construction_price
 */
Reservoir::Reservoir(const char *name, const int id, const double min_environmental_outflow,
                     const vector<Catchment *> &catchments, const double capacity,
                     const double max_treatment_capacity, const double construction_rof,
                     const vector<double> &construction_time_range, double construction_cost, double bond_term,
                     double bond_interest_rate, vector<vector<double>> *evaporation_time_series_all_realizations) :
        WaterSource(name, id, min_environmental_outflow, catchments, capacity, max_treatment_capacity, RESERVOIR,
        construction_rof, construction_time_range, construction_cost, bond_term, bond_interest_rate),
        evaporation_time_series_all_realizations(evaporation_time_series_all_realizations) {}

/**
 * Copy constructor.
 * @param reservoir
 */
Reservoir::Reservoir(const Reservoir &reservoir) : WaterSource(reservoir),
                                                   evaporation_time_series_all_realizations(
                                                           reservoir.evaporation_time_series_all_realizations) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
Reservoir &Reservoir::operator=(const Reservoir &reservoir) {
    evaporation_time_series_all_realizations = reservoir.evaporation_time_series_all_realizations;
    WaterSource::operator=(reservoir);
    return *this;
}

/**
 * Destructor.
 */
Reservoir::~Reservoir() {
    catchments.clear();
}

/**
 * Reservoir mass balance. Gets releases from upstream reservoirs, demands from connected utilities, and
 * combines them with its catchments inflows.
 * @param week
 * @param upstream_source_inflow
 * @param demand_outflow
 */
void Reservoir::applyContinuity(int week, double upstream_source_inflow, double demand_outflow) {

    /// Calculate total runoff inflow reaching reservoir from its watershed.
    double catchment_inflow = 0;
    for (Catchment *c : catchments) {
        catchment_inflow += c->getStreamflow((week));
    }

    /// Calculate new stored volume and outflow based on continuity.
    double stored_volume_new = available_volume
                               + upstream_source_inflow + catchment_inflow
                               - demand_outflow - min_environmental_outflow;
    double outflow_new = min_environmental_outflow;

    /// Check if spillage is needed and, if so, correct stored volume and calculate spillage.
    if (online) {
        if (stored_volume_new > capacity) {
            outflow_new += stored_volume_new - capacity;
            stored_volume_new = capacity;
        }
    } else {
        stored_volume_new = available_volume;
        outflow_new = upstream_source_inflow + catchment_inflow;
    }

    /// Update data collection variables.
    demand = demand_outflow;
    available_volume = max(stored_volume_new, 0.0);
    total_outflow = outflow_new;
    this->upstream_source_inflow = upstream_source_inflow;
    upstream_catchment_inflow = catchment_inflow;
}

void Reservoir::setOnline() {
    WaterSource::setOnline();

    /// start empty and gradually fill as inflows start coming in.
    available_volume = NONE;
}

void Reservoir::setRealization(unsigned long r) {
    WaterSource::setRealization(r);

    /// Set evaporation time series and cut off access to series set by setting its pointer to the set to NULL.
    evaporation_time_series = new double[evaporation_time_series_all_realizations->at(r).size()];
    std::copy(evaporation_time_series_all_realizations->at(r).begin(),
              evaporation_time_series_all_realizations->at(r).end(),
              evaporation_time_series);
    evaporation_time_series_all_realizations = NULL;
}
