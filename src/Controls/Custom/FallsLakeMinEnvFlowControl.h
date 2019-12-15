//
// Created by bct52 on 7/3/17.
//

#ifndef TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H


#include "../Base/MinEnvFlowControl.h"

class FallsLakeMinEnvFlowControl : public MinEnvFlowControl {
private:
    const vector<int> week_interval;
    const vector<double> base_min_env_flows;
    const vector<double> base_min_gage_flows;
    const int neuse_river_intake_id;
    Catchment crabtree;

public:
    FallsLakeMinEnvFlowControl(
            int water_source_id, int neuse_river_intake_id,
            vector<int> week_interval, vector<double> base_min_env_flows,
            vector<double> base_gage_flows, Catchment &crabtree);

    FallsLakeMinEnvFlowControl(const FallsLakeMinEnvFlowControl &min_env_control);

    double getRelease(int week) override;

    void setRealization(unsigned long r, const vector<double> &rdm_factors) override;

    ~FallsLakeMinEnvFlowControl() override;

};


#endif //TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H
