//
// Created by bernardoct on 6/29/17.
//

#include "FixedMinEnvFlowControl.h"

FixedMinEnvFlowControl::FixedMinEnvFlowControl(
        int water_source_id, double release)
        : MinEnvFlowControl(water_source_id, vector<int>(), vector<int>(),
                                FIXED_FLOW_CONTROLS), release(release) {}

FixedMinEnvFlowControl::FixedMinEnvFlowControl(
        const FixedMinEnvFlowControl &min_env_control) :
        MinEnvFlowControl(min_env_control), release(min_env_control.release) {}

double FixedMinEnvFlowControl::getRelease(int week) {
    return release;
}

FixedMinEnvFlowControl::~FixedMinEnvFlowControl() = default;
