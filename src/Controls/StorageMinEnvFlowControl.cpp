//
// Created by bct52 on 6/28/17.
//

#include "StorageMinEnvFlowControl.h"

StorageMinEnvFlowControl::StorageMinEnvFlowControl(
        int water_source_id, const vector<int> &aux_water_sources_ids,
        vector<double>& storages, vector<double>& releases)
        : MinEnvFlowControl(water_source_id, aux_water_sources_ids,
                                vector<int>(),
                                STORAGE_CONTROLS),
          storages(storages),
          releases(releases) {
    if (aux_water_sources_ids.size() != 1)
        throw invalid_argument("Storage based control rules needs exactly one"
                                         " reservoir/quarry");
}

StorageMinEnvFlowControl::StorageMinEnvFlowControl(
        int water_source_id, vector<double>& storages, vector<double>& releases)
        : MinEnvFlowControl(water_source_id, vector<int>(water_source_id),
                            vector<int>(),
                            STORAGE_CONTROLS),
          storages(storages),
          releases(releases) {}

StorageMinEnvFlowControl::StorageMinEnvFlowControl(
        const StorageMinEnvFlowControl &min_env_control) :
        MinEnvFlowControl(min_env_control),
        storages(min_env_control.storages),
        releases(min_env_control.releases) {}


double StorageMinEnvFlowControl::getRelease(int week) {
    double source_storage = water_sources[water_source_id]
            ->getAvailableVolume();

    double release = 0;
    for (unsigned long i = 0; i < storages.size(); ++i) {
        release = (source_storage >= storages[i] ? releases[i] : release);
    }
    return release;
}
