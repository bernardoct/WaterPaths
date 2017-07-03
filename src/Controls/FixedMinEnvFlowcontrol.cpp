//
// Created by bernardoct on 6/29/17.
//

#include "FixedMinEnvFlowcontrol.h"

FixedMinEnvFlowcontrol::FixedMinEnvFlowcontrol(
        int water_source_id, double release)
        : MinEnvironFlowControl(water_source_id,
                                vector<int>(),
                                vector<int>()), release(release) {}


double FixedMinEnvFlowcontrol::getRelease(int week) {
    return release;
}
