//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_SEASONALMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_SEASONALMINENVFLOWCONTROL_H

#include <vector>
#include "Base/MinEnvFlowControl.h"

using namespace std;

class SeasonalMinEnvFlowControl : public MinEnvFlowControl {
private:
    int week_next_tier;
    int last_tier = 0;
    vector<int> week_thresholds;
    const vector<double>& min_env_flows;
    const unsigned long n_week_thresholds;

public:
    SeasonalMinEnvFlowControl(
            int water_source_id, const vector<int> week_thresholds,
            const vector<double>& min_env_flows);

    SeasonalMinEnvFlowControl(const SeasonalMinEnvFlowControl &min_env_control);

    double getRelease(int week) override;

    const vector<int> &getWeekThresholds() const;

    const vector<double> &getMinEnvFlows() const;
};


#endif //TRIANGLEMODEL_SEASONALMINENVFLOWCONTROL_H
