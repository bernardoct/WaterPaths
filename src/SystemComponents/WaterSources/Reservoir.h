//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_RESERVOIR_H
#define TRIANGLEMODEL_RESERVOIR_H

#include <string>
#include "../Catchment.h"
#include "WaterSource.h"

using namespace std;


class Reservoir : public WaterSource {

public:

    Reservoir(const string &source_name, const int id, const double min_environmental_outflow,
              const vector<Catchment *> &catchments, bool online, const double capacity);

    Reservoir(const Reservoir &reservoir);

    Reservoir &operator=(const Reservoir &reservoir);

    ~Reservoir();

    virtual void updateAvailableVolume(int week, double upstream_source_inflow, double demand_outflow) override;

    void setAvailableVolumeAndOutflowPreviousRelease(double available_volume, double outflow_previous_week);


};


#endif //TRIANGLEMODEL_RESERVOIR_H
