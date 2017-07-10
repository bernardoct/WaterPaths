//
// Created by bct52 on 7/3/17.
//

#include "FallsLakeMinEnvFlowControl.h"


FallsLakeMinEnvFlowControl::FallsLakeMinEnvFlowControl(
        int water_source_id, int neuse_river_intake_id,
        const int *week_interval, const double *base_min_env_flows,
        const double *base_gage_flows, Catchment crabtree)
        : MinEnvironFlowControl(water_source_id,
                                vector<int>(1,
                                            neuse_river_intake_id),
                                vector<int>(),
                                FALLS_CONTROLS),
          neuse_river_intake_id(neuse_river_intake_id),
          week_interval(week_interval),
          base_min_env_flows(base_min_env_flows),
          base_min_gage_flows(base_gage_flows), crabtree(crabtree) {}


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
               min_gage_flow - neuse_outflow -
               crabtree.getStreamflow(week));
}

void FallsLakeMinEnvFlowControl::setRealization(unsigned int r) {
    crabtree.setRealization(r);
}
