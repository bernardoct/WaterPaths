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

private:
    string sourceType = "Reservoir";

public:

    Reservoir(const string &source_name, const int id, const double min_environmental_outflow,
              const Catchment &catchment, bool online, const double capacity);

    virtual void updateAvailableVolume(int week, double upstream_source_inflow, double demand_outflow) override;

};


#endif //TRIANGLEMODEL_RESERVOIR_H
