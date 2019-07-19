//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_WATERREUSE_H
#define TRIANGLEMODEL_WATERREUSE_H


#include "Base/WaterSource.h"

class WaterReuse : public WaterSource {
private:
    double treated_volume = NON_INITIALIZED;
public:
    WaterReuse(const char *name, const int id, const double capacity);

    WaterReuse(const char *name, const int id, const double treatment_capacity,
                   const vector<double> &construction_time_range, double permitting_period,
                   Bond &bond);
    WaterReuse(const WaterReuse &reuse);

    void applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_discharge,
                             vector<double> &demand_outflow) override;

    WaterReuse &operator=(const WaterReuse &water_reuse);

    double getReused_volume() const;

};

#endif //TRIANGLEMODEL_WATERREUSE_H
