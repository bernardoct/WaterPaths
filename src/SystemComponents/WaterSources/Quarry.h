//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_QUARRY_H
#define TRIANGLEMODEL_QUARRY_H


#include "Reservoir.h"

class Quarry : public WaterSource {
private:
    double max_diversion;

public:

    Quarry(const Quarry &quarry, const double max_diversion);

    Quarry(const string &source_name, const int id, const double min_environmental_outflow,
           const vector<Catchment *> &catchments, const double capacity, const double max_treatment_capacity,
           double max_diversion);

    Quarry(const string &source_name, const int id, const double min_environmental_outflow,
           const vector<Catchment *> &catchments, const double capacity, const double raw_water_main_capacity,
           const double construction_rof, const vector<double> &construction_time_range,
           double construction_cost_of_capital, double bond_term, double bond_interest_rate, double max_diversion);

    Quarry &operator=(const Quarry &quarry);

    ~Quarry();

    void applyContinuity(int week, double upstream_source_inflow, double demand_outflow);

    void setOnline() override;
};


#endif //TRIANGLEMODEL_QUARRY_H
