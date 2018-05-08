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
    double area = NON_INITIALIZED;

public:
    const bool fixed_area;
    EvaporationSeries evaporation_series;

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series,
            DataSeries *storage_area_curve, int source_type = RESERVOIR);

    Reservoir(const char *name, const int id, const vector<Catchment *> &catchments, const double capacity,
              const double max_treatment_capacity, EvaporationSeries &evaporation_series,
              DataSeries *storage_area_curve, const vector<double> &construction_time_range,
              double permitting_period, Bond &bond, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series, double storage_area,
            int source_type = RESERVOIR);

    Reservoir(const char *name, const int id, const vector<Catchment *> &catchments, const double capacity,
              const double max_treatment_capacity, EvaporationSeries &evaporation_series,
              double storage_area, const vector<double> &construction_time_range, double permitting_period,
              Bond &bond, int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series,
            DataSeries *storage_area_curve,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            int source_type = RESERVOIR);

    Reservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series,
            double storage_area,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations,
            int source_type = RESERVOIR);

    Reservoir(const char *name, const int id, const vector<Catchment *> &catchments, const double capacity,
              const double max_treatment_capacity, EvaporationSeries &evaporation_series,
              DataSeries *storage_area_curve, vector<double> *allocated_treatment_fractions,
              vector<double> *allocated_fractions, vector<int> *utilities_with_allocations,
              const vector<double> &construction_time_range, double permitting_period,
              Bond &bond, int source_type = RESERVOIR);

    Reservoir(const char *name, const int id, const vector<Catchment *> &catchments, const double capacity,
              const double max_treatment_capacity, EvaporationSeries &evaporation_series,
              double storage_area, vector<double> *allocated_treatment_fractions,
              vector<double> *allocated_fractions, vector<int> *utilities_with_allocations,
              const vector<double> &construction_time_range, double permitting_period,
              Bond &bond, int source_type = RESERVOIR);


    Reservoir(const Reservoir &reservoir);

    Reservoir &operator=(const Reservoir &reservoir);

    ~Reservoir() override;

    void applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_discharge,
                             vector<double> &demand_outflow) override;

    void setOnline() override;

    void setRealization(unsigned long r, vector<double> &rdm_factors) override;

    double getArea() const;

    void updateTreatmentAndCapacityAllocations(int week) override;
};


#endif //TRIANGLEMODEL_RESERVOIR_H
