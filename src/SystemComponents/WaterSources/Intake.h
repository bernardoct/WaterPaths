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
            const float max_treatment_capacity);

    Intake(
            const char *name, const int id,
            const vector<Catchment *> &catchments,
            const float raw_water_main_capacity,
            const float construction_rof_or_demand,
            const vector<float> construction_time_range,
            float permitting_period,
            float construction_npv_cost_of_capital, float bond_term,
            float bond_interest_rate);

    Intake(const Intake &intake);

    Intake &operator=(const Intake &intake);

    ~Intake() override;

    void applyContinuity(int week, float upstream_source_min_env_flow,
                             float wastewater_inflow, vector<float> &demand) override;

    void setRealization(unsigned long r) override;

};


#endif //TRIANGLEMODEL_INTAKE_H
