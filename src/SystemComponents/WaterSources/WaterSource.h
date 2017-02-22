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
    double demand = -1;
    double upstream_min_env_inflow;
    bool online = Constants::ONLINE;
    int week;

public:
    const int id;
    const double capacity = NONE;
    const double min_environmental_outflow;
    const string name;
    const int source_type;
    const double max_treatment_capacity;

    WaterSource(const string &source_name, const int id, const double min_environmental_outflow,
                const vector<Catchment *> &catchments, bool online, const double capacity,
                const double max_treatment_capacity, const int source_type);

    WaterSource(const WaterSource &water_source);

    virtual ~WaterSource();

    WaterSource &operator=(const WaterSource &water_source);

    bool operator<(const WaterSource *other);

    void continuityWaterSource(int week, double upstream_source_inflow, double demand_outflow);

    virtual void applyContinuity(int week, double upstream_source_inflow, double demand_outflow) = 0;

    virtual void setOnline();

    double getAvailable_volume() const;

    double getTotal_outflow() const;

    bool isOnline() const;

    void setAvailable_volume(double available_volume);

    void setOutflow_previous_week(double outflow_previous_week);

    double getCapacity();

    double getUpstream_source_inflow() const;

    double getDemand() const;

    double getCatchment_upstream_catchment_inflow() const;

    void bypass(int week, double upstream_source_inflow);
};


#endif //TRIANGLEMODEL_WATERSOURCE_H
