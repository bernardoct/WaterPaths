//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_QUARRY_H
#define TRIANGLEMODEL_QUARRY_H


#include "Reservoir.h"

class Quarry : public Reservoir {
private:
    double max_diversion;
    double diverted_flow;

public:

    Quarry(const Quarry &quarry, const double max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, double max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate, double max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            double max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate, double max_diversion);

    Quarry &operator=(const Quarry &quarry);

    ~Quarry();

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> *demand_outflow) override;

    void setOnline() override;
};


#endif //TRIANGLEMODEL_QUARRY_H
