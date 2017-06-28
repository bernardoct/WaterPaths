//
// Created by bct52 on 6/28/17.
//

#include "SeasonalMinEnvFlowControl.h"

SeasonalMinEnvFlowControl::SeasonalMinEnvFlowControl(const vector<int> &week_thresholds,
                                                     const vector<double> &min_env_flows) : week_thresholds(
        week_thresholds), min_env_flows(min_env_flows) {}

double SeasonalMinEnvFlowControl::getRelease(int week) {
    double releases = 0;
    for (int i = 0; i < week_thresholds.size(); ++i) {
        /// Done with ternary operator for improved performance.
        releases = (week <= week_thresholds[i] ? min_env_flows[i] : releases);
    }
    return 0;
}
