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
private:
    double **storage_vs_area;
    double *evaporation_time_series;
    vector<vector<double>> *evaporation_time_series_all_realizations;

public:

    Reservoir(const char *name, const int id, const double min_environmental_outflow,
              const vector<Catchment *> &catchments, const double capacity,
              const double max_treatment_capacity, vector<vector<double>> *evaporation_time_series_all_realizations);

    Reservoir(const char *name, const int id, const double min_environmental_outflow,
                  const vector<Catchment *> &catchments, const double capacity,
                  const double max_treatment_capacity, const double construction_rof,
                  const vector<double> &construction_time_range, double construction_cost, double bond_term,
                  double bond_interest_rate, vector<vector<double>> *evaporation_series_input_file);

    Reservoir(const Reservoir &reservoir);

    Reservoir &operator=(const Reservoir &reservoir);

    ~Reservoir();

    virtual void applyContinuity(int week, double upstream_source_inflow, double demand_outflow) override;

    void setOnline();

    void setRealization(unsigned long r) override;
};


#endif //TRIANGLEMODEL_RESERVOIR_H
