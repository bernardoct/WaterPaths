//
// Created by bct52 on 6/29/17.
//

#include "InflowMinEnvFlowControl.h"

InflowMinEnvFlowControl::InflowMinEnvFlowControl(int water_source_id,
                                                 const vector<double> inflows,
                                                 const vector<double> &releases)
        : MinEnvFlowControl(water_source_id, vector<int>(1, water_source_id),
                            vector<int>(),
                            INFLOW_CONTROLS),
          inflows(inflows),
          releases(releases),
          n_inflows(inflows.size()) {
    this->inflows.push_back(1e15);
}

InflowMinEnvFlowControl::InflowMinEnvFlowControl(
        const InflowMinEnvFlowControl &min_env_control) :
        MinEnvFlowControl(min_env_control),
        inflows(min_env_control.inflows),
        releases(min_env_control.releases),
        n_inflows(min_env_control.n_inflows) {
}

InflowMinEnvFlowControl::~InflowMinEnvFlowControl() = default;

double InflowMinEnvFlowControl::getRelease(int week) {
    double inflow =
            water_sources[water_source_id]->getUpstream_source_inflow() +
            water_sources[water_source_id]->getUpstreamCatchmentInflow();

    if (inflow > inflows[last_tier]) {
        for (int i = last_tier; i < n_inflows; ++i) {
            if (inflows[i + 1] > inflow) {
            last_tier = i;
                return releases[i];
            }
        }
    } else {
        for (int i = 0; i < last_tier + 1; ++i) {
            if (inflows[i + 1] > inflow) {
            last_tier = i;
                return releases[i];
            }
        }
    }

    throw runtime_error("Release corresponding to inflow not found. Contact"
		    "bct52@cornell.edu (Bernardo Trindade) about this");

}

const vector<double> &InflowMinEnvFlowControl::getInflows() const {
    return inflows;
}

const vector<double> &InflowMinEnvFlowControl::getReleases() const {
    return releases;
}
