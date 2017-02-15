//
// Created by bernardo on 1/22/17.
//

#ifndef TRIANGLEMODEL_WATERSOURCE_H
#define TRIANGLEMODEL_WATERSOURCE_H

#include <string>
#include "../Catchment.h"
#include "../../Utils/Constants.h"

using namespace std;
using namespace Constants;


class WaterSource {
protected:
    vector<Catchment *> catchments;
    double available_volume = -1;
    double total_outflow = -1;
    double upstream_source_inflow = -1;
    double upstream_catchment_inflow = -1;
    double demand_previous_week = -1;
    bool online = Constants::ONLINE;
    int week;

public:
    const int id;
    const double capacity = NONE;
    const double min_environmental_outflow;
    const string name;
    const int source_type;

    WaterSource(const string &source_name, const int id, const double min_environmental_outflow,
                const vector<Catchment *> &catchments, bool online, const double capacity,
                const int source_type);

    WaterSource(const WaterSource &water_source);

    virtual ~WaterSource();

    WaterSource &operator=(const WaterSource &water_source);

    virtual void updateAvailableVolume(int week, double upstream_source_inflow, double demand_outflow) = 0;

    void setOnline(bool online_status);

    double getAvailable_volume() const;

    double getTotal_inflow() const;

    bool isOnline() const;

    void setAvailable_volume(double available_volume);

    void setOutflow_previous_week(double outflow_previous_week);

    double getCapacity();

    double getUpstream_source_inflow() const;

    double getDemand() const;

    double getCatchment_upstream_catchment_inflow() const;

    bool isOfType(int type) const;
};


#endif //TRIANGLEMODEL_WATERSOURCE_H
