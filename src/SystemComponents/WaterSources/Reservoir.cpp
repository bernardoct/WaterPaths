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
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
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
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series, float storage_area,
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
 * @param construction_rof_or_demand
 * @param construction_time_range
 * @param construction_price
 */
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series,
        DataSeries *storage_area_curve, const float construction_rof_or_demand,
        const vector<float> &construction_time_range, float permitting_period,
        float construction_cost, float bond_term,
        float bond_interest_rate, int source_type) :
        WaterSource(name, id, catchments, capacity, max_treatment_capacity,
                    source_type, construction_time_range, permitting_period,
                    construction_cost, bond_term, bond_interest_rate),
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
 * @param construction_rof_or_demand
 * @param construction_time_range
 * @param construction_price
 */
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series, float storage_area,
        const float construction_rof_or_demand,
        const vector<float> &construction_time_range, float permitting_period,
        float construction_cost, float bond_term,
        float bond_interest_rate, int source_type) :
        WaterSource(name, id, catchments, capacity, max_treatment_capacity,
                    source_type, construction_time_range, permitting_period,
                    construction_cost, bond_term, bond_interest_rate),
        evaporation_series(evaporation_series),
        storage_area_curve(nullptr), fixed_area(true), area(storage_area) {}

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
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series,
        DataSeries *storage_area_curve,
        vector<float> *allocated_treatment_fractions,
        vector<float> *allocated_fractions,
        vector<int> *utilities_with_allocations, int source_type) :
        WaterSource(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    source_type,
                    allocated_treatment_fractions,
                    allocated_fractions,
                    utilities_with_allocations),
        evaporation_series(evaporation_series),
        storage_area_curve(storage_area_curve), fixed_area(false) {

    if (storage_area_curve &&
        storage_area_curve->getSeries_x().back() != capacity)
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
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series, float storage_area,
        vector<float> *allocated_treatment_fractions,
        vector<float> *allocated_fractions,
        vector<int> *utilities_with_allocations, int source_type) :
        WaterSource(name,
                    id,
                    catchments,
                    capacity,
                    max_treatment_capacity,
                    source_type,
                    allocated_treatment_fractions,
                    allocated_fractions,
                    utilities_with_allocations),
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
 * @param construction_rof_or_demand
 * @param construction_time_range
 * @param construction_price
 */
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series,
        DataSeries *storage_area_curve,
        vector<float> *allocated_treatment_fractions,
        vector<float> *allocated_fractions,
        vector<int> *utilities_with_allocations,
        const float construction_rof_or_demand,
        const vector<float> &construction_time_range, float permitting_period,
        float construction_cost, float bond_term,
        float bond_interest_rate, int source_type) :
        WaterSource(name, id, catchments, capacity, max_treatment_capacity,
                    source_type, allocated_treatment_fractions,
                    allocated_fractions, utilities_with_allocations,
                    construction_time_range, permitting_period,
                    construction_cost, bond_term, bond_interest_rate),
        evaporation_series(evaporation_series),
        storage_area_curve(storage_area_curve), fixed_area(false) {

    if (storage_area_curve &&
        storage_area_curve->getSeries_x().back() != capacity)
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
 * @param construction_rof_or_demand
 * @param construction_time_range
 * @param construction_price
 */
Reservoir::Reservoir(
        const char *name, const int id,
        const vector<Catchment *> &catchments, const float capacity,
        const float max_treatment_capacity,
        EvaporationSeries *evaporation_series, float storage_area,
        vector<float> *allocated_treatment_fractions,
        vector<float> *allocated_fractions,
        vector<int> *utilities_with_allocations,
        const float construction_rof_or_demand,
        const vector<float> &construction_time_range, float permitting_period,
        float construction_cost, float bond_term,
        float bond_interest_rate, int source_type) :
        WaterSource(name, id, catchments, capacity, max_treatment_capacity,
                    source_type, allocated_treatment_fractions,
                    allocated_fractions, utilities_with_allocations,
                    construction_time_range, permitting_period,
                    construction_cost, bond_term, bond_interest_rate),
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
                                                   fixed_area(reservoir.fixed_area), area(reservoir.area) {
    evaporation_series = new EvaporationSeries(*evaporation_series);
}

/**
 * Copy assignment operator
 * @param reservoir
 * @return
 */
Reservoir &Reservoir::operator=(const Reservoir &reservoir) {
    evaporation_series = new EvaporationSeries(*evaporation_series);
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
void Reservoir::applyContinuity(int week, float upstream_source_inflow,
                                float wastewater_inflow,
                                vector<float> &demand_outflow) {

    float total_upstream_inflow = upstream_source_inflow +
                                   wastewater_inflow;

//    float total_demand = std::accumulate(demand_outflow.begin(),
//                                          demand_outflow.end(),
//                                          0.);

    total_demand = 0;
    for (int i = 0; i < demand_outflow.size(); ++i) {
        total_demand += demand_outflow[i];
    }

    /// Calculate total runoff inflow reaching reservoir from its watershed.
    float catchment_inflow = 0;
    for (Catchment *c : catchments) {
        /*if (c == nullptr) {
            cout << "not right\n";
        }*/
        catchment_inflow += c->getStreamflow(week);
    }

    /// Calculates water lost through evaporation.
    if (fixed_area)
        evaporated_volume = area * evaporation_series->getEvaporation(week);
    else {
        area = storage_area_curve->get_dependent_variable(available_volume);
        evaporated_volume = area *
                evaporation_series->getEvaporation(week);
    }

    /// Calculate new stored volume and outflow based on continuity.
    float stored_volume_new = available_volume
                               + total_upstream_inflow + catchment_inflow
                               - total_demand - min_environmental_outflow
                               - evaporated_volume;
    float outflow_new = min_environmental_outflow;


    /// Check if spillage is needed and, if so, correct stored volume and
    /// calculate spillage.
    if (stored_volume_new > capacity) {
        outflow_new += stored_volume_new - capacity;
        stored_volume_new = capacity;
    }

    /// Update data collection variables.
    this->total_demand = total_demand + policy_added_demand;
    policy_added_demand = 0;
    available_volume = stored_volume_new;//max(stored_volume_new, 0.0);
    total_outflow = outflow_new;
    upstream_catchment_inflow = catchment_inflow;
    this->upstream_source_inflow = upstream_source_inflow;
    this->wastewater_inflow = wastewater_inflow;
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
    if (evaporation_series != nullptr)
        evaporation_series->setRealization(r);
    else {
        cout << "WARNING: No evaporated_volume time series for Reservoir "
             << name;
        vector<vector<float>> *evaporation =
                new std::vector<vector<float>>(1,
                                                vector<float>(10000,
                                                               0));
        evaporation_series = new EvaporationSeries(evaporation, 10000);
    }
}

float Reservoir::getArea() const {
    return area;
}
