//
// Created by bernardo on 2/10/17.
//

#ifndef TRIANGLEMODEL_INTAKE_H
#define TRIANGLEMODEL_INTAKE_H


#include "Reservoir.h"

using namespace Constants;

class Intake : public WaterSource {
    double water_bank = 0;

public:

    Intake(
            const char *name, const int id,
            const vector<Catchment *> &catchments,
            const double max_treatment_capacity);

    Intake(
            const char *name, const int id,
            const vector<Catchment *> &catchments,
            const double raw_water_main_capacity,
            const double construction_rof,
            const vector<double> construction_time_range,
            double construction_npv_cost_of_capital, double bond_term,
            double bond_interest_rate);

    Intake(const Intake &intake);

    Intake &operator=(const Intake &intake);

    ~Intake() override;

    void applyContinuity(
            int week, double upstream_source_min_env_flow,
            vector<double> *demand) override;

    void setRealization(unsigned long r) override;

};


#endif //TRIANGLEMODEL_INTAKE_H
