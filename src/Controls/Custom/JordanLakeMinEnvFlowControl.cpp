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
            int jordan_lake_id, Catchment& lillington_gage_catchment,
            double rel02, double rel04, double rel06_up, double min_rel_llt06,
            double min_rel_llt08, double min_rel_llt08_up,
            double quality_capacity, double supply_capacity)
        : MinEnvFlowControl(jordan_lake_id, vector<int>(1, jordan_lake_id),
                                vector<int>(),
                                JORDAN_CONTROLS),
          lillington_gage_catchment(lillington_gage_catchment), rel02(rel02),
          rel04(rel04), rel06_up(rel06_up), min_rel_llt06(min_rel_llt06),
          min_rel_llt08(min_rel_llt08), min_rel_llt08_up(min_rel_llt08_up),
          quality_capacity(quality_capacity),
          supply_capacity(supply_capacity) {}

JordanLakeMinEnvFlowControl::~JordanLakeMinEnvFlowControl() {
//    delete lillington_gage_catchment;
}

JordanLakeMinEnvFlowControl::JordanLakeMinEnvFlowControl(
        const JordanLakeMinEnvFlowControl &jl_min_flow_control)
        : MinEnvFlowControl(jl_min_flow_control),
          lillington_gage_catchment(Catchment(jl_min_flow_control
                  .lillington_gage_catchment)),
          rel02(jl_min_flow_control.rel02),
          rel04(jl_min_flow_control.rel04),
          rel06_up(jl_min_flow_control.rel06_up),
          min_rel_llt06(jl_min_flow_control.min_rel_llt06),
          min_rel_llt08(jl_min_flow_control.min_rel_llt08),
          min_rel_llt08_up(jl_min_flow_control.min_rel_llt08_up),
          quality_capacity(jl_min_flow_control.quality_capacity),
          supply_capacity(jl_min_flow_control.supply_capacity) {}

JordanLakeMinEnvFlowControl &JordanLakeMinEnvFlowControl::operator=
        (const JordanLakeMinEnvFlowControl& jl_min_flow_control) {
    lillington_gage_catchment = Catchment(jl_min_flow_control
            .lillington_gage_catchment);

    return *this;
}

double JordanLakeMinEnvFlowControl::getRelease(int week) {
    double water_quality_storage =
            max(water_sources[water_source_id]->getAvailableVolume(),
                quality_capacity);
    double lillington_flow_rate =
            water_sources[water_source_id]->getTotal_outflow() +
                    lillington_gage_catchment.getStreamflow(week);

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
    else
        return rel06_up;
}

void JordanLakeMinEnvFlowControl::setRealization(unsigned int r, vector<double> &rdm_factors) {
    lillington_gage_catchment.setRealization(r, rdm_factors);
}
