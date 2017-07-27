//
// Created by bct52 on 7/27/17.
//

#ifndef TRIANGLEMODEL_RAWWATERRELEASES_H
#define TRIANGLEMODEL_RAWWATERRELEASES_H


#include "../SystemComponents/Utility.h"
#include "Base/DroughtMitigationPolicy.h"

class RawWaterReleases : DroughtMitigationPolicy {
private:
    Utility *upstream_utility;
    Utility *downstream_utility;
    Reservoir *upstream_reservoir;
    Reservoir *downstream_reservoir;
    MinEnvironFlowControl *upstream_min_env_flow_control;

    const int upstream_utility_id;
    const int downstream_utility_id;
    const int upstream_reservoir_id;
    const int downstream_reservoir_id;
    const double max_releases;
    const double min_releases;


public:
    RawWaterReleases(const int id,
                         const int upstream_utility_id,
                         const int downstream_utility_id,
                         const int upstream_reservoir_id,
                         const int downstream_reservoir_id,
                         const double max_releases,
                         const double min_releases);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities,
                             vector<WaterSource *> water_sources,
                             vector<MinEnvironFlowControl *> min_env_flow_controls) override;

    void setRealization(unsigned int realization_id) override;


public:

};


#endif //TRIANGLEMODEL_RAWWATERRELEASES_H
