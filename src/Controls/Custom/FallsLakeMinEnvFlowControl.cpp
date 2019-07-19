//
// Created by bct52 on 7/3/17.
//

#include "FallsLakeMinEnvFlowControl.h"


FallsLakeMinEnvFlowControl::FallsLakeMinEnvFlowControl(
        int water_source_id, int neuse_river_intake_id,
        const vector<int> week_interval, const vector<double> base_min_env_flows,
        const vector<double> base_gage_flows, Catchment &crabtree)
        : MinEnvFlowControl(water_source_id,
                                vector<int>(1, neuse_river_intake_id),
                                vector<int>(),
                                FALLS_CONTROLS),
          week_interval(week_interval),
          base_min_env_flows(base_min_env_flows),
          base_min_gage_flows(base_gage_flows), 
          neuse_river_intake_id(neuse_river_intake_id),
          crabtree(crabtree) {}

FallsLakeMinEnvFlowControl::~FallsLakeMinEnvFlowControl() = default;

FallsLakeMinEnvFlowControl::FallsLakeMinEnvFlowControl(
        const FallsLakeMinEnvFlowControl &min_env_control) :
        MinEnvFlowControl(min_env_control),
        week_interval(min_env_control.week_interval) ,
        base_min_env_flows(min_env_control.base_min_env_flows),
        base_min_gage_flows(min_env_control.base_min_gage_flows),
        neuse_river_intake_id(min_env_control.neuse_river_intake_id),
        crabtree(Catchment(min_env_control.crabtree)) {}

double FallsLakeMinEnvFlowControl::getRelease(int week) {

    double base_release;
    double min_gage_flow;

    if (week < week_interval[0] && week > week_interval[1]) {
        base_release = base_min_env_flows[0];
        min_gage_flow = base_min_gage_flows[0];
    } else {
        base_release = base_min_env_flows[1];
        min_gage_flow = base_min_gage_flows[1];
    }

    double neuse_outflow =
            water_sources[neuse_river_intake_id]->getTotal_outflow();

    return max(base_release,
               min_gage_flow - max(0., neuse_outflow) -
               crabtree.getStreamflow(week));
}

void FallsLakeMinEnvFlowControl::setRealization(unsigned long r, vector<double> &rdm_factors) {
    crabtree.setRealization(r, rdm_factors);
}
