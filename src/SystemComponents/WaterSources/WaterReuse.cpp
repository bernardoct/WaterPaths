//
// Created by bernardoct on 5/3/17.
//


#include <numeric>
#include "WaterReuse.h"

WaterReuse::WaterReuse(const char *name, const int id, const double capacity)
        : WaterSource(name, id, vector<Catchment *>(), capacity, vector<int>(), capacity, WATER_REUSE) {
    available_volume = capacity;
}


WaterReuse::WaterReuse(const char *name, const int id, const double treatment_capacity,
                       const vector<double> &construction_time_range, double permitting_period,
                       Bond &bond) : WaterSource(name, id, vector<Catchment *>(), NONE, treatment_capacity,
                                                                          vector<int>(), WATER_REUSE, construction_time_range,
                                                                          permitting_period, bond),
                                                              treated_volume(treatment_capacity) {
    available_volume = treatment_capacity;
}

WaterReuse::WaterReuse(const WaterReuse &reuse) : WaterSource(reuse), treated_volume(reuse.treated_volume) {}

void WaterReuse::applyContinuity(int week, double upstream_source_inflow,
                                 double wastewater_discharge,
                                 vector<double> &demand_outflow) {

    double total_demand = std::accumulate(demand_outflow.begin(),
                                          demand_outflow.end(),
                                          0.);

    treated_volume = min(total_demand, total_treatment_capacity);
}


WaterReuse &WaterReuse::operator=(const WaterReuse &water_reuse) {
    WaterSource::operator=(water_reuse);
    available_volume = water_reuse.available_volume;
    return *this;
}

double WaterReuse::getReused_volume() const {
    return treated_volume;
}
