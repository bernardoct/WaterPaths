//
// Created by bct52 on 6/28/17.
//

#include "StorageMinEnvFlowControl.h"

StorageMinEnvFlowControl::StorageMinEnvFlowControl(const vector<int> &water_sources_ids, vector<double> *storages,
                                                   vector<double> *releases)
        : MinEnvironFlowControl(water_sources_ids, vector<int>(), vector<int>()), storages(storages),
          releases(releases) {
    if (water_sources_ids.size() != 1) __throw_invalid_argument("Storage based control rules needs exactly one "
                                                                        "reservoir/quarry");
}

double StorageMinEnvFlowControl::getRelease(int week) {
    double source_storage = water_sources[0]->getAvailable_volume();

    double release = 0;
    for (int i = 0; i < storages->size(); ++i) {
        /// Done with ternary operator for improved performance.
        release = (source_storage <= (*storages)[i] ? (*releases)[i] : release);
    }
    return release;
}