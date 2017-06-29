//
// Created by bct52 on 6/29/17.
//

#include "InflowMinEnvFlowControl.h"

InflowMinEnvFlowControl::InflowMinEnvFlowControl(const vector<int> &water_sources_ids,
                                                 const vector<double> *inflows, const vector<double> *releases)
        : MinEnvironFlowControl(water_sources_ids, catchments_ids, utilities_ids), inflows(inflows),
          releases(releases) {}

double InflowMinEnvFlowControl::getRelease(int week) {
    double inflow = water_sources[0]->getUpstream_source_inflow() +
            water_sources[0]->getUpstreamCatchmentInflow();

    double release = 0;
    for (int i = 0; i < inflows->size(); ++i) {
        /// Done with ternary operator for improved performance.
        release = (inflow <= (*inflows)[i] ? (*releases)[i] : release);
    }
    return release;
}