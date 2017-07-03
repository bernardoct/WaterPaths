//
// Created by bct52 on 7/3/17.
//

#include "JordanLakeMinEnvFlowControl.h"

/**
 * This set of controls assumes that the Lillington gage is the next thing
 * downstream Jordan Lake.
 * @param jordan_lake_id
 * @param lillington_gage_catchment
 * @param rel02
 * @param rel04
 * @param rel06_up
 * @param min_rel_llt06
 * @param min_rel_llt08
 * @param min_rel_llt08_up
 * @param quality_capacity
 * @param supply_capacity
 */
JordanLakeMinEnvFlowControl::JordanLakeMinEnvFlowControl(
            int jordan_lake_id, Catchment *lillington_gage_catchment,
            double rel02, double rel04, double rel06_up, double min_rel_llt06,
            double min_rel_llt08, double min_rel_llt08_up,
            double quality_capacity, double supply_capacity)
        : MinEnvironFlowControl(jordan_lake_id, vector<int>(1, jordan_lake_id),
                                vector<int>(), JORDAN),
          lillington_gage_catchment(lillington_gage_catchment), rel02(rel02),
          rel04(rel04), rel06_up(rel06_up), min_rel_llt06(min_rel_llt06),
          min_rel_llt08(min_rel_llt08), min_rel_llt08_up(min_rel_llt08_up),
          quality_capacity(quality_capacity),
          supply_capacity(supply_capacity) {}

JordanLakeMinEnvFlowControl::JordanLakeMinEnvFlowControl(
        const JordanLakeMinEnvFlowControl &jl_min_flow_control)
        : MinEnvironFlowControl(jl_min_flow_control),
          lillington_gage_catchment(new Catchment(*jl_min_flow_control
                                             .lillington_gage_catchment)) {
    int i = 1;
}

JordanLakeMinEnvFlowControl &JordanLakeMinEnvFlowControl::operator=
        (const JordanLakeMinEnvFlowControl jl_min_flow_control) {
    lillington_gage_catchment = new Catchment(*jl_min_flow_control
            .lillington_gage_catchment);

    return *this;
}


double JordanLakeMinEnvFlowControl::getRelease(int week) {
    double water_quality_storage =
            max(water_sources[water_source_id]->getAvailable_volume(),
                quality_capacity);
    double lillington_flow_rate =
            water_sources[water_source_id]->getTotal_outflow() +
                    lillington_gage_catchment->getStreamflow(week);

    if (water_quality_storage < 0.2)
        return rel02;
    else if (water_quality_storage < 0.4)
        return rel04;
    else if (lillington_flow_rate < min_rel_llt06 - rel06_up)
        return min(rel06_up, min_rel_llt06 - lillington_flow_rate);
    else if (lillington_flow_rate < min_rel_llt08 - rel06_up)
        return min(rel06_up, min_rel_llt08 - lillington_flow_rate);
    else if (lillington_flow_rate < min_rel_llt08_up - rel06_up)
        return min(rel06_up, min_rel_llt08_up - lillington_flow_rate);
}

void JordanLakeMinEnvFlowControl::setRealiation(unsigned int r) {
    lillington_gage_catchment->setRealization(r);
}
