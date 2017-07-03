//
// Created by bct52 on 7/3/17.
//

#include "FallsLakeMinEnvFlowControl.h"

FallsLakeMinEnvFlowControl::FallsLakeMinEnvFlowControl(int water_source_id,
                                                       const vector<int> &aux_water_sources_id,
                                                       const vector<int> &aux_utilities_ids,
                                                       int type)
        : MinEnvironFlowControl(water_source_id, aux_water_sources_id,
                                aux_utilities_ids, type) {}

double FallsLakeMinEnvFlowControl::getRelease(int week) {
    return 0;
}
