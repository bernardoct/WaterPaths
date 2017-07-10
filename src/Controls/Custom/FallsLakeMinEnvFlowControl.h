//
// Created by bct52 on 7/3/17.
//

#ifndef TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H


#include "../Base/MinEnvironFlowControl.h"

class FallsLakeMinEnvFlowControl : public MinEnvironFlowControl {
private:
    const int *week_interval;
    const double *base_min_env_flows;
    const double *base_min_gage_flows;
    const int neuse_river_intake_id;
    Catchment crabtree;


public:
    FallsLakeMinEnvFlowControl(
            int water_source_id, int neuse_river_intake_id,
            const int *week_interval, const double *base_min_env_flows,
            const double *base_gage_flows, Catchment crabtree);

    double getRelease(int week) override;

    void setRealization(unsigned int r) override;

};


#endif //TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H
