//
// Created by dgorelic on 7/8/2019.
//

#ifndef TRIANGLEMODEL_INTAKEEXPANSION_H
#define TRIANGLEMODEL_INTAKEEXPANSION_H

#include "Base/WaterSource.h"

class IntakeExpansion : public WaterSource {
public:

    const unsigned int parent_intake_ID;

    IntakeExpansion(const char *name, const int id, const unsigned int parent_intake_ID,
                    const double capacity_added, const double treatment_capacity_added,
                    const vector<double> &construction_time_range,
                    double permitting_period, Bond &bond);

    IntakeExpansion(const IntakeExpansion &intake_expansion);

    IntakeExpansion &operator=(
            const IntakeExpansion &intake_expansion);

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;
};


#endif //TRIANGLEMODEL_INTAKEEXPANSION_H
