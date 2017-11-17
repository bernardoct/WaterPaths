//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_RESERVOIR_H
#define TRIANGLEMODEL_RESERVOIR_H

#include <string>
#include "../Catchment.h"
#include "Base/WaterSource.h"
#include "../../Utils/DataSeries.h"
#include "../../Controls/EvaporationSeries.h"

using namespace std;


class Reservoir : public WaterSource {
protected:
    DataSeries *storage_area_curve;
    float area = NON_INITIALIZED;

public:
    const bool fixed_area;
    EvaporationSeries *evaporation_series;

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            vector<float> *allocated_treatment_fractions,
            vector<float> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            vector<float> *allocated_treatment_fractions,
            vector<float> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            vector<float> *allocated_treatment_fractions,
            vector<float> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            vector<float> *allocated_treatment_fractions,
            vector<float> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float permitting_period,
            float construction_cost, float bond_term,
            float bond_interest_rate, int source_type = RESERVOIR);


    Reservoir(const Reservoir &reservoir);

    Reservoir &operator=(const Reservoir &reservoir);

    ~Reservoir() override;

    void applyContinuity(int week, float upstream_source_inflow,
                             float wastewater_discharge,
                             vector<float> &demand_outflow) override;

    void setOnline() override;

    void setRealization(unsigned long r) override;

    float getArea() const;
};


#endif //TRIANGLEMODEL_RESERVOIR_H
