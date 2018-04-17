//
// Created by bernardo on 2/10/17.
//

#ifndef TRIANGLEMODEL_INTAKE_H
#define TRIANGLEMODEL_INTAKE_H


#include "Reservoir.h"

using namespace Constants;

class Intake : public WaterSource {

public:

    Intake(
            const char *name, const int id,
            const vector<Catchment *> &catchments,
            const double max_treatment_capacity);

    Intake(const char *name, const int id, const vector<Catchment *> &catchments,
           const double raw_water_main_capacity, const vector<double> construction_time_range,
           double permitting_period, Bond &bond);

    Intake(const Intake &intake);

    Intake &operator=(const Intake &intake);

    ~Intake() override;

    void applyContinuity(int week, double upstream_source_min_env_flow,
                             double wastewater_inflow, vector<double> &demand) override;

    void setRealization(unsigned long r, vector<double> &rdm_factors) override;

};


#endif //TRIANGLEMODEL_INTAKE_H
