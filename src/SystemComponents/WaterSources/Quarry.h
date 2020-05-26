//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_QUARRY_H
#define TRIANGLEMODEL_QUARRY_H


#include "Reservoir.h"

class Quarry : public Reservoir {
private:
    double max_diversion = NON_INITIALIZED;
    double diverted_flow = NON_INITIALIZED;

public:

    Quarry(
            string name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series,
            DataSeries &storage_area_curve, double max_diversion);

    Quarry(string name, const int id, const vector<Catchment *> &catchments,
           const double capacity,
           const double max_treatment_capacity,
           EvaporationSeries &evaporation_series,
           DataSeries &storage_area_curve,
           vector<int> construction_prerequisites,
           const vector<double> &construction_time_range,
           double permitting_period, Bond &bond, double max_diversion);

    Quarry(
            string name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series, double storage_area,
            double max_diversion);

    Quarry(string name, const int id, const vector<Catchment *> &catchments,
           const double capacity,
           const double max_treatment_capacity,
           EvaporationSeries &evaporation_series, double storage_area,
           vector<int> construction_prerequisites,
           const vector<double> &construction_time_range, double permitting_period, Bond &bond,
           double max_diversion);

    Quarry(const Quarry &quarry, const double max_diversion);

    Quarry &operator=(const Quarry &quarry);

    ~Quarry();

    void applyContinuity(
            int week, double upstream_source_inflow, double wastewater_inflow,
            vector<double> &demand_outflow) override;

    void setOnline() override;
};


#endif //TRIANGLEMODEL_QUARRY_H
