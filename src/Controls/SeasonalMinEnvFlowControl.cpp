//
// Created by bct52 on 6/28/17.
//

#include "SeasonalMinEnvFlowControl.h"
#include "../Utils/Utils.h"

SeasonalMinEnvFlowControl::SeasonalMinEnvFlowControl(
        int water_source_id, const vector<int>& week_thresholds,
        const vector<double>& min_env_flows) :
        MinEnvFlowControl(water_source_id, vector<int>(), vector<int>(),
                              SEASONAL_CONTROLS),
        week_thresholds(week_thresholds),
        min_env_flows(min_env_flows) {
    if (week_thresholds.size() != min_env_flows.size() + 1)
        __throw_invalid_argument("Number of week threshold needs to be one "
                                         "more than number of flow rates for "
                                         "seasonal minimum environmental "
                                         "outflow requirements.");
    if (week_thresholds.back() != (int) WEEKS_IN_YEAR + 1 &&
        week_thresholds.at(0) != 0)
        __throw_invalid_argument("Seasonal minimum environmental flow "
                                         "requirements must have first week "
                                         "interval as 0 and last as 53 (for "
                                         "years with 53 weeks.");
}

SeasonalMinEnvFlowControl::SeasonalMinEnvFlowControl(
        const SeasonalMinEnvFlowControl &min_env_control) :
        MinEnvFlowControl(min_env_control),
        week_thresholds(min_env_control.week_thresholds),
        min_env_flows(min_env_control.min_env_flows) {}

double SeasonalMinEnvFlowControl::getRelease(int week) {
    double release = min_env_flows[0];
    for (int i = 0; i < min_env_flows.size(); ++i) {
        /// Done with ternary operator for improved performance.
        release = (Utils::weekOfTheYear(week) >= week_thresholds[i] ?
                   min_env_flows[i] : release);
    }

    return release;
}
