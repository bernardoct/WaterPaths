//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_SEASONALMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_SEASONALMINENVFLOWCONTROL_H

#include <vector>
#include "Base/MinEnvironFlowControl.h"

using namespace std;

class SeasonalMinEnvFlowControl : public MinEnvironFlowControl {
private:
    const vector<int> *week_thresholds;
    const vector<float> *min_env_flows;

public:
    SeasonalMinEnvFlowControl(
            int water_source_id, const vector<int> *week_thresholds,
            const vector<float> *min_env_flows);

    float getRelease(int week) override;
};


#endif //TRIANGLEMODEL_SEASONALMINENVFLOWCONTROL_H
