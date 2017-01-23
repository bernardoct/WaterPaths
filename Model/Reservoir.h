//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_RESERVOIR_H
#define TRIANGLEMODEL_RESERVOIR_H

#include <string>
#include "Catchment.h"

using namespace std;


class Reservoir {
private:
    Catchment catchment;
    double stored_volume;
    double release_previous_week;
    bool online;
    int week;

public:
    const int id;
    string const reservoir_name;
    double const capacity;
    double const min_environmental_flow;

    Reservoir(const string &reservoir_name, const int id, const double min_environmental_flow,
              const Catchment &catchment, bool online, const double capacity);

    void applyContinuity(int week, double upstream_reservoir_inflow, double demand_outflow);

    void setOnline(bool online_status);

    double getStored_volume() const;

    double getRelease_previous_week() const;

    bool isOnline() const;

    void toString();
};


#endif //TRIANGLEMODEL_RESERVOIR_H
