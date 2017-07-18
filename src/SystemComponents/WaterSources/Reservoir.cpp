//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include <numeric>
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
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        const double max_treatment_capacity,
        EvaporationSeries *evaporation_series,
        DataSeries *storage_area_curve, int source_type) :
        WaterSource(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    source_type),
        evaporation_series(evaporation_series),
        storage_area_curve(storage_area_curve), fixed_area(false) {

    if (storage_area_curve && storage_area_curve->getSeries_x().back() != capacity)
        __throw_invalid_argument("Last storage of data series must be equal to reservoir capacity.");
}

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
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        int source_type) :
        WaterSource(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    source_type),
        evaporation_series(evaporation_series),
        storage_area_curve(nullptr), fixed_area(true), area(storage_area) {}

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
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        const double max_treatment_capacity,
        EvaporationSeries *evaporation_series,
        DataSeries *storage_area_curve, const double construction_rof,
        const vector<double> &construction_time_range,
        double construction_cost, double bond_term,
        double bond_interest_rate, int source_type) :
        WaterSource(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    source_type,
                    construction_rof,
                    construction_time_range,
                    construction_cost,
                    bond_term,
                    bond_interest_rate),
        evaporation_series(evaporation_series),
        storage_area_curve(storage_area_curve), fixed_area(false) {

    if (storage_area_curve && storage_area_curve->getSeries_x().back() != capacity)
        __throw_invalid_argument("Last storage of data series must be equal to reservoir capacity.");
}

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
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const double capacity,
        const double max_treatment_capacity,
        EvaporationSeries *evaporation_series, double storage_area,
        const double construction_rof,
        const vector<double> &construction_time_range,
        double construction_cost, double bond_term,
        double bond_interest_rate, int source_type) :
        WaterSource(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    source_type,
                    construction_rof,
                    construction_time_range,
                    construction_cost,
                    bond_term,
                    bond_interest_rate),
        evaporation_series(evaporation_series),
        storage_area_curve(nullptr), fixed_area(true), area(storage_area) {}

/**
 * Copy constructor.
 * @param reservoir
 */
Reservoir::Reservoir(const Reservoir &reservoir) : WaterSource(reservoir),
                                                   evaporation_series(
                                                           reservoir.evaporation_series),
                                                   storage_area_curve(reservoir.storage_area_curve),
                                                   fixed_area(reservoir.fixed_area), area(reservoir.area) {}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
Reservoir &Reservoir::operator=(const Reservoir &reservoir) {
    evaporation_series = reservoir.evaporation_series;
    storage_area_curve = reservoir.storage_area_curve;
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
 * Reservoir mass balance. Gets releases from upstream reservoirs, demands from
 * connected utilities, and combines them with its catchments inflows.
 * @param week
 * @param upstream_source_inflow
 * @param demand_outflow
 */
void Reservoir::applyContinuity(
        int week, double upstream_source_inflow,
        vector<double> *demand_outflow) {

    double total_demand = std::accumulate(demand_outflow->begin(),
                                          demand_outflow->end(),
                                          0.);

    /// Calculate total runoff inflow reaching reservoir from its watershed.
    double catchment_inflow = 0;
    for (Catchment *c : catchments)
        catchment_inflow += c->getStreamflow(week);

    /// Calculates water lost through evaporation.
    evaporated_volume =
            (fixed_area ? area * evaporation_series->getEvaporation(week) :
             storage_area_curve->get_dependent_variable(available_volume) *
             evaporation_series->getEvaporation(week));

    /// Calculate new stored volume and outflow based on continuity.
    double stored_volume_new = available_volume
                               + upstream_source_inflow + catchment_inflow
                               - total_demand - min_environmental_outflow
                               - evaporated_volume;
    double outflow_new = min_environmental_outflow;


    /// Check if spillage is needed and, if so, correct stored volume and
    /// calculate spillage.
    if (stored_volume_new > capacity) {
        outflow_new += stored_volume_new - capacity;
        stored_volume_new = capacity;
    }

    /// Update data collection variables.
    this->total_demand = total_demand;
    available_volume = stored_volume_new;//max(stored_volume_new, 0.0);
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

    /// Set evaporation time series and cut off access to series set by setting
    /// its pointer to the set to NULL.
    if (evaporation_series)
        evaporation_series->setRealization(r);
    else {
        cout << "WARNING: No evaporated_volume time series for Reservoir "
             << name;
        vector<vector<double>> *evaporation =
                new std::vector<vector<double>>(1,
                                                vector<double>(10000,
                                                               0));
        evaporation_series = new EvaporationSeries(evaporation, 10000);
    }
}
