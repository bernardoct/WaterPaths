//
// Created by bernardoct on 5/3/17.
//


#include <numeric>
#include "WaterReuse.h"

WaterReuse::WaterReuse(const char *name, const int id, const double capacity)
        : WaterSource(name,
                      id,
                      vector<Catchment *>(),
                      capacity,
                      capacity,
                      WATER_REUSE) {
    available_volume = capacity;
}


WaterReuse::WaterReuse(
        const char *name, const int id, const double capacity,
        const double construction_rof_or_demand,
        const vector<double> &construction_time_range, double permitting_period,
        double construction_cost_of_capital,
        double bond_term,
        double bond_interest_rate) : WaterSource(name,
                                                 id,
                                                 vector<Catchment *>(),
                                                 capacity,
                                                 capacity,
                                                 WATER_REUSE,
                                                 construction_rof_or_demand,
                                                 construction_time_range,
                                                 permitting_period,
                                                 construction_cost_of_capital,
                                                 bond_term,
                                                 bond_interest_rate) {
    available_volume = capacity;
}

void WaterReuse::applyContinuity(
        int week, double upstream_source_inflow,
        vector<double> &demand_outflow) {

    double total_demand = std::accumulate(demand_outflow.begin(),
                                          demand_outflow.end(),
                                          0.);

    treated_volume = min(total_demand,
                         capacity);
}


WaterReuse &WaterReuse::operator=(const WaterReuse &water_reuse) {
    WaterSource::operator=(water_reuse);
    available_volume = water_reuse.capacity;
    return *this;
}

double WaterReuse::getReused_volume() const {
    return treated_volume;
}
