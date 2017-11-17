//
// Created by bernardoct on 6/29/17.
//

#include "FixedMinEnvFlowControl.h"

FixedMinEnvFlowControl::FixedMinEnvFlowControl(
        int water_source_id, float release)
        : MinEnvironFlowControl(water_source_id, vector<int>(), vector<int>(),
                                FIXED_FLOW_CONTROLS), release(release) {}


float FixedMinEnvFlowControl::getRelease(int week) {
    return release;
}
