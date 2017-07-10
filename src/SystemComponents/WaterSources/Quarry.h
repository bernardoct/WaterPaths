//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_QUARRY_H
#define TRIANGLEMODEL_QUARRY_H


#include "Reservoir.h"

class Quarry : public WaterSource {
private:
    double max_diversion;
    double diverted_flow;

public:

    Quarry(const Quarry &quarry, const double max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity, double max_diversion_capacity);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double raw_water_main_capacity, double max_diversion,
            const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost_of_capital, double bond_term,
            double bond_interest_rate);

    Quarry &operator=(const Quarry &quarry);

    ~Quarry();

    void applyContinuity(int week, double upstream_source_inflow, double demand_outflow);

    void setOnline() override;
};


#endif //TRIANGLEMODEL_QUARRY_H
