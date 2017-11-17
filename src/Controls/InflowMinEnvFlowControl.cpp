//
// Created by bct52 on 6/29/17.
//

#include "InflowMinEnvFlowControl.h"

InflowMinEnvFlowControl::InflowMinEnvFlowControl(
        int water_source_id, const vector<int> &aux_water_sources_ids,
        const vector<float> *inflows, const vector<float> *releases)
        : MinEnvironFlowControl(water_source_id, aux_water_sources_ids,
                                vector<int>(),
                                INFLOW_CONTROLS),
          inflows(inflows),
          releases(releases) {}

float InflowMinEnvFlowControl::getRelease(int week) {
    float inflow =
            water_sources[water_source_id]->getUpstream_source_inflow() +
            water_sources[water_source_id]->getUpstreamCatchmentInflow();

    float release = 0;
    for (int i = 0; i < inflows->size(); ++i) {
        /// Done with ternary operator for improved performance.
        release = (inflow >= (*inflows)[i] ? (*releases)[i] : release);
    }
    return release;
}