//
// Created by bernardo on 2/10/17.
//

#ifndef TRIANGLEMODEL_INTAKE_H
#define TRIANGLEMODEL_INTAKE_H


#include "Reservoir.h"

using namespace Constants;

class Intake : public WaterSource {
public:

    Intake(const string &source_name, const int id, const double min_environmental_outflow,
           const vector<Catchment *> &catchments, bool online, double max_treatment_capacity);

    Intake(const Intake &intake);

    Intake &operator=(const Intake &intake);

    ~Intake();

    virtual void applyContinuity(int week, double upstream_source_min_env_flow, double demand) override;

    void setUpstream_min_env_flow(double upstream_min_env_flow);

};


#endif //TRIANGLEMODEL_INTAKE_H
