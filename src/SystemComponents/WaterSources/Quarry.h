//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_QUARRY_H
#define TRIANGLEMODEL_QUARRY_H


#include "Reservoir.h"

class Quarry : public Reservoir {
private:
    float max_diversion;
    float diverted_flow;

public:

    Quarry(const Quarry &quarry, const float max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, float max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate, float max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            float max_diversion);

    Quarry(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate, float max_diversion);

    Quarry &operator=(const Quarry &quarry);

    ~Quarry();

    void applyContinuity(
            int week, float upstream_source_inflow, float wastewater_inflow,
            vector<float> &demand_outflow) override;

    void setOnline() override;
};


#endif //TRIANGLEMODEL_QUARRY_H
