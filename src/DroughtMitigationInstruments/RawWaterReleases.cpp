//
// Created by bct52 on 7/27/17.
//

#include "RawWaterReleases.h"

RawWaterReleases::RawWaterReleases(const int id,
                                   const int upstream_utility_id,
                                   const int downstream_utility_id,
                                   const int upstream_reservoir_id,
                                   const int downstream_reservoir_id,
                                   const double max_releases,
                                   const double min_releases)
        : DroughtMitigationPolicy(id, type),
          upstream_utility_id(upstream_utility_id),
          downstream_utility_id(downstream_utility_id),
          upstream_reservoir_id(upstream_reservoir_id),
          downstream_reservoir_id(downstream_reservoir_id),
          max_releases(max_releases), min_releases(min_releases) {}

void RawWaterReleases::addSystemComponents(vector<Utility *> utilities,
                                           vector<WaterSource *> water_sources,
                                           vector<MinEnvironFlowControl *> min_env_flow_controls) {
    upstream_reservoir =
            dynamic_cast<Reservoir *>(water_sources[upstream_reservoir_id]);
    downstream_reservoir =
            dynamic_cast<Reservoir *>(water_sources[downstream_reservoir_id]);
    upstream_utility = utilities[upstream_reservoir_id];
    downstream_utility = utilities[downstream_reservoir_id];

    /// Gets the control rule that acts on reservoir of id reservoir_id.
    for (MinEnvironFlowControl *mefc : min_env_flow_controls) {
        if (mefc->water_source_id == upstream_reservoir_id) {
            upstream_min_env_flow_control = mefc;
        }
    }

}

void RawWaterReleases::applyPolicy(int week) {

    if (downstream_reservoir->getAvailable_volume() > 50) {
        upstream_reservoir->removeWater()
    }

}

void RawWaterReleases::setRealization(unsigned int realization_id) {

}
