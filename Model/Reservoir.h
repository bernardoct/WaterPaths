//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_RESERVOIR_H
#define TRIANGLEMODEL_RESERVOIR_H

#include <string>

using namespace std;


class Reservoir {
    double stored_volume;

public:

    string const reservoir_name;
    double const capacity;
    double const min_environmental_flow;
    bool online;

    Reservoir(const string reservoir_name, double stored_volume, const double capacity,
              const double min_environmental_flow, bool online);

    double applyContinuity(double inflow, double demand_outflow);

    double getStored_volume() const;

};


#endif //TRIANGLEMODEL_RESERVOIR_H
