//
// Created by bernardoct on 5/3/17.
//


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


WaterReuse::WaterReuse(const char *name, const int id, const double capacity, const double construction_rof,
                       const vector<double> &construction_time_range, double construction_cost_of_capital,
                       double bond_term,
                       double bond_interest_rate) : WaterSource(name,
                                                                id,
                                                                vector<Catchment *>(),
                                                                capacity,
                                                                capacity,
                                                                WATER_REUSE,
                                                                construction_rof,
                                                                construction_time_range,
                                                                construction_cost_of_capital,
                                                                bond_term,
                                                                bond_interest_rate) {
    available_volume = capacity;
}

void WaterReuse::applyContinuity(
        int week, double upstream_source_inflow,
        vector<double> *demand_outflow) {}


WaterReuse &WaterReuse::operator=(const WaterReuse &water_reuse) {
    WaterSource::operator=(water_reuse);
    available_volume = water_reuse.capacity;
    return *this;
}
