//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_RESERVOIR_H
#define TRIANGLEMODEL_RESERVOIR_H

#include <string>
#include "../Catchment.h"
#include "WaterSource.h"

using namespace std;


class Reservoir : public WaterSource {
public:

    Reservoir(const char *name, const int id, const double min_environmental_outflow,
              const vector<Catchment *> &catchments, const double capacity, const double max_treatment_capacity);

    Reservoir(const char *name, const int id, const double min_environmental_outflow,
              const vector<Catchment *> &catchments, const double capacity, const double raw_water_main_capacity,
              const double construction_rof, const vector<double> &construction_time_range, double
              construction_cost,
              double bond_term, double bond_interest_rate);

    Reservoir(const Reservoir &reservoir);

    Reservoir &operator=(const Reservoir &reservoir);

    ~Reservoir();

    virtual void applyContinuity(int week, double upstream_source_inflow, double demand_outflow) override;

    void setOnline();
};


#endif //TRIANGLEMODEL_RESERVOIR_H
