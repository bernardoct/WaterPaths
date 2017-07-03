//
// Created by bct52 on 6/28/17.
//

#include "StorageMinEnvFlowControl.h"

StorageMinEnvFlowControl::StorageMinEnvFlowControl(
        int water_source_id, const vector<int> &aux_water_sources_ids,
        vector<double> *storages, vector<double> *releases)
        : MinEnvironFlowControl(water_source_id, aux_water_sources_ids,
                                vector<int>(), STORAGE),
          storages(storages),
          releases(releases) {
    if (aux_water_sources_ids.size() != 1)
        __throw_invalid_argument("Storage based control rules needs exactly one"
                                         " reservoir/quarry");
}

double StorageMinEnvFlowControl::getRelease(int week) {
    double source_storage = water_sources[water_source_id]
            ->getAvailable_volume();

    double release = 0;
    for (int i = 0; i < storages->size(); ++i) {
        /// Done with ternary operator for improved performance.
        release = (source_storage >= (*storages)[i] ? (*releases)[i] : release);
    }
    return release;
}