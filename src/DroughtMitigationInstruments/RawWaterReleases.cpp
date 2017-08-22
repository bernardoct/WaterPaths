//
// Created by bct52 on 7/27/17.
//

#include <mshtmlc.h>
#include <algorithm>
#include "RawWaterReleases.h"
#include "../Utils/Utils.h"

RawWaterReleases::RawWaterReleases(const int id,
                                   const int upstream_utility_id,
                                   const int downstream_utility_id,
                                   const int upstream_reservoir_id,
                                   const int downstream_reservoir_id,
                                   const double max_releases,
                                   const vector<double> rof_triggers,
                                   const double raw_water_transfer_downstream_allocation_ratio)
        : DroughtMitigationPolicy(id, RAW_WATER_TRANSFERS),
          upstream_utility_id(upstream_utility_id),
          downstream_utility_id(downstream_utility_id),
          upstream_reservoir_id(upstream_reservoir_id),
          downstream_reservoir_id(downstream_reservoir_id),
          max_releases(max_releases),
          rof_triggers(rof_triggers),
          raw_water_transfer_downstream_allocation_ratio(raw_water_transfer_downstream_allocation_ratio) {}

void RawWaterReleases::addSystemComponents(vector<Utility *> utilities,
                                           vector<WaterSource *> water_sources,
                                           vector<MinEnvironFlowControl *> min_env_flow_controls) {
    upstream_reservoir =
            dynamic_cast<Reservoir *>(water_sources[upstream_reservoir_id]);
    downstream_reservoir =
            dynamic_cast<Reservoir *>(water_sources[downstream_reservoir_id]);
    upstream_utility = utilities[upstream_reservoir_id];
    downstream_utility = utilities[downstream_reservoir_id];

    /// Gets the control rule that acts on reservoir of id reservoir_id.
    for (MinEnvironFlowControl *mefc : min_env_flow_controls) {
        if (mefc->water_source_id == upstream_reservoir_id) {
            upstream_min_env_flow_control = mefc;
        }
    }
}

void RawWaterReleases::applyPolicy(int week) {

    raw_water_transfer_volume = 0.0;
    storage_targets = {0.0, 0.0};

    /// Raw water transfers are calculated if upstream ROF is above trigger level AND
    /// downstream ROF is below trigger level
    if ((downstream_utility->getRisk_of_failure() > rof_triggers[downstream_utility_id]) &
            (upstream_utility->getRisk_of_failure() < rof_triggers[upstream_utility_id])) {

        /// Determine storage deficit for downstream utility and available capacity for
        /// upstream utility
        storage_targets[0] = getUtilityStorageFromROF(Utils::weekOfTheYear(week), storage_to_rof_table_,
                                                      upstream_utility_id);
        storage_targets[1] = getUtilityStorageFromROF(Utils::weekOfTheYear(week), storage_to_rof_table_,
                                                      downstream_utility_id);

        /// ensure that there is no incorrect assignment - it is possible because of the
        /// size of ROF-to-storage table increments that target and current storage levels
        /// will be the same or incorrect
        if (storage_targets[0] > upstream_utility->getStorageToCapacityRatio())
            storage_targets[0] = upstream_utility->getStorageToCapacityRatio();
        if (storage_targets[1] < downstream_utility->getStorageToCapacityRatio())
            storage_targets[1] = downstream_utility->getStorageToCapacityRatio();

        /// Set raw water transfer request after applying physical constraints:
        ///     1. initial downstream request (based on ROF deficit)
        ///     2. initial upstream available volume (based on ROF surplus)
        ///     3. upstream available volume in donor reservoir
        ///     4. downstream available capacity in recipient reservoir
        ///     5. raw water release volume cap (proxy for watercourse flooding)
        std::vector<double> temp_vec = {(storage_targets[1] - downstream_utility->getStorageToCapacityRatio()) *
                                        downstream_utility->getTotal_storage_capacity(),
                                        (upstream_utility->getStorageToCapacityRatio() - storage_targets[0]) *
                                        upstream_utility->getTotal_storage_capacity(),
                                        upstream_reservoir->getAvailableAllocatedVolume(upstream_utility_id),
                                        (downstream_reservoir->getAllocatedCapacity(downstream_utility_id) -
                                         downstream_reservoir->getAvailableAllocatedVolume(downstream_utility_id)),
                                        max_releases};

        raw_water_transfer_volume = *std::min_element(temp_vec.begin(), temp_vec.end());

        /*if (raw_water_transfer_volume > upstream_reservoir->getAvailable_volume()) {
            raw_water_transfer_volume = upstream_reservoir->getAvailable_volume();
        }

        if (raw_water_transfer_volume >
                (downstream_reservoir->getCapacity() - downstream_reservoir->getAvailable_volume())) {
            raw_water_transfer_volume =
                    (downstream_reservoir->getCapacity() - downstream_reservoir->getAvailable_volume());
        }

        if (raw_water_transfer_volume > max_releases) {
            raw_water_transfer_volume = max_releases;
        }*/

        /// apply additional constraints:
        ///     1. remove required environmental releases from total
        ///     2. non-negativity
        raw_water_transfer_volume -= upstream_min_env_flow_control->getRelease(week);

        if (raw_water_transfer_volume < 0) {
            raw_water_transfer_volume = 0.0;
        }

        /*if (downstream_utility->getStorageToCapacityRatio() < 0.2) {
            raw_water_transfer_downstream_allocation_ratio = 1.0;
        }*/

        /// adjust storage (and water quality, when applicable) levels in each reservoir
        downstream_reservoir->addWater(WATER_QUALITY_ALLOCATION,
                                       raw_water_transfer_volume *
                                               (1 - raw_water_transfer_downstream_allocation_ratio));
        downstream_reservoir->addWater(downstream_utility_id,
                                       raw_water_transfer_volume *
                                               raw_water_transfer_downstream_allocation_ratio);
        upstream_reservoir->removeWater(upstream_utility_id, raw_water_transfer_volume);
    }
}

/**
 * calculates the targeted storage level of a utility according to ROF triggers
 * @param week week of the year.
 * @param storage_to_rof_table table correlating storage capacities to rof
 * @param utility ID
 * @return
 */
const double RawWaterReleases::getUtilityStorageFromROF(
        int week,
        const Matrix3D<double> *storage_to_rof_table,
        const int u_id) {

    /// Determine the storage associated with an ROF level
    for (int s = (NO_OF_RAW_WATER_TRANSFER_STORAGE_TIERS - 1); s > -1; --s) {
        if ((*storage_to_rof_table)(u_id, s, week) > rof_triggers[u_id]) {
            if (s == (NO_OF_RAW_WATER_TRANSFER_STORAGE_TIERS - 1)) {
                return ((double)s / (double)NO_OF_RAW_WATER_TRANSFER_STORAGE_TIERS);
            } else {
                return (((double)s + 1) / (double)NO_OF_RAW_WATER_TRANSFER_STORAGE_TIERS);
            }
        }
    }
}

void RawWaterReleases::setRealization(unsigned int realization_id) {}

const double &RawWaterReleases::getRawWaterTransferVolume() const {
    return raw_water_transfer_volume;
}

const vector<double> &RawWaterReleases::getUtilityTargetStorageLevels() const {
    return storage_targets;
}