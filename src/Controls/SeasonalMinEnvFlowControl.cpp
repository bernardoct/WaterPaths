//
// Created by bct52 on 6/28/17.
//

#include "SeasonalMinEnvFlowControl.h"
#include "../Utils/Utils.h"

SeasonalMinEnvFlowControl::SeasonalMinEnvFlowControl(
        int water_source_id, const vector<int> week_thresholds,
        const vector<double>& min_env_flows) :
        MinEnvFlowControl(water_source_id, vector<int>(), vector<int>(),
                              SEASONAL_CONTROLS),
        week_thresholds(week_thresholds),
        min_env_flows(min_env_flows),
        n_week_thresholds(week_thresholds.size() - 1) {
    this->week_thresholds.push_back(54);
    week_next_tier = week_thresholds.at(1);

    if (week_thresholds.size() != min_env_flows.size() + 1)
        throw invalid_argument("Number of week threshold needs to be one "
                                         "more than number of flow rates for "
                                         "seasonal minimum environmental "
                                         "outflow requirements.");
    if (week_thresholds.back() != (int) WEEKS_IN_YEAR + 1 &&
        week_thresholds.at(0) != 0)
        throw invalid_argument("Seasonal minimum environmental flow "
                                         "requirements must have first week "
                                         "interval as 0 and last as 53 (for "
                                         "years with 53 weeks.");
}

SeasonalMinEnvFlowControl::SeasonalMinEnvFlowControl(
        const SeasonalMinEnvFlowControl &min_env_control) :
        MinEnvFlowControl(min_env_control),
        week_thresholds(min_env_control.week_thresholds),
        min_env_flows(min_env_control.min_env_flows),
        n_week_thresholds(min_env_control.n_week_thresholds),
        week_next_tier(min_env_control.week_next_tier) {}

double SeasonalMinEnvFlowControl::getRelease(int week) {
    if (Utils::weekOfTheYear(week) == 0) {
        last_tier = 0;
        week_next_tier = week_thresholds[1];
        return min_env_flows[0];
    } if (Utils::weekOfTheYear(week) < week_next_tier) {
        return min_env_flows[last_tier];
    } else {
        last_tier++;
        if (last_tier == min_env_flows.size()) {
            return *min_env_flows.end();
        } else {
            week_next_tier = week_thresholds[last_tier + 1];
            return min_env_flows[last_tier];
        }
    }
}

const vector<int> &SeasonalMinEnvFlowControl::getWeekThresholds() const {
    return week_thresholds;
}

const vector<double> &SeasonalMinEnvFlowControl::getMinEnvFlows() const {
    return min_env_flows;
}
