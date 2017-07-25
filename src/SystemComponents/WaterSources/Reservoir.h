//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_RESERVOIR_H
#define TRIANGLEMODEL_RESERVOIR_H

#include <string>
#include "../Catchment.h"
#include "Base/WaterSource.h"
#include "../../Utils/DataSeries.h"
#include "../../Utils/EvaporationSeries.h"

using namespace std;


class Reservoir : public WaterSource {
protected:
    EvaporationSeries *evaporation_series;
    DataSeries *storage_area_curve;
    const bool fixed_area;
    double area = NON_INITIALIZED;

public:

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            const double construction_rof_or_demand,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate, int source_type = RESERVOIR);


    Reservoir(const Reservoir &reservoir);

    Reservoir &operator=(const Reservoir &reservoir);

    ~Reservoir() override;

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> *demand_outflow) override;

    void setOnline() override;

    void setRealization(unsigned long r) override;
};


#endif //TRIANGLEMODEL_RESERVOIR_H
