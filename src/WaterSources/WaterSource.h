//
// Created by bernardo on 1/22/17.
//

#ifndef TRIANGLEMODEL_WATERSOURCE_H
#define TRIANGLEMODEL_WATERSOURCE_H

#include <string>
#include "../Catchment.h"

using namespace std;


class WaterSource {
protected:
    Catchment catchment;
    double available_volume;
    double outflow_previous_week;
    bool online;
    int week;
    string source_type;

public:
    const int id;
    string const source_name;
    double const capacity;
    double const min_environmental_outflow;

    WaterSource(const string &source_name, const int id, const double min_environmental_outflow,
                const Catchment &catchment, bool online, const double capacity);

    virtual void updateAvailableVolume(int week, double upstream_source_inflow, double demand_outflow);

    void setOnline(bool online_status);

    double getAvailable_volume() const;

    double getOutflow_previous_week() const;

    bool isOnline() const;

    void toString();
};


#endif //TRIANGLEMODEL_WATERSOURCE_H
