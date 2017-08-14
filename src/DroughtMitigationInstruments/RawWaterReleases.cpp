//
// Created by bct52 on 7/27/17.
//

#include <mshtmlc.h>
#include <algorithm>
#include "RawWaterReleases.h"

RawWaterReleases::RawWaterReleases(const int id,
                                   const int upstream_utility_id,
                                   const int downstream_utility_id,
                                   const int upstream_reservoir_id,
                                   const int downstream_reservoir_id,
                                   const double max_releases,
                                   double *rof_triggers,
                                   const double raw_water_transfer_downstream_allocation_ratio)
        : DroughtMitigationPolicy(id, type),
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
            dynamic_cast<AllocatedReservoir *>(water_sources[upstream_reservoir_id]);
    downstream_reservoir =
            dynamic_cast<AllocatedReservoir *>(water_sources[downstream_reservoir_id]);
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

    /// Raw water transfers are calculated if upstream ROF is above trigger level AND
    /// downstream ROF is below trigger level
    if ((downstream_utility->getRisk_of_failure() > rof_triggers[downstream_utility_id]) &
            (upstream_utility->getRisk_of_failure() < rof_triggers[upstream_utility_id])) {

        /// Determine storage deficit for downstream utility and available capacity for
        /// upstream utility
        getUtilityStorageFromROF(week, storage_to_rof_table_);

        /// Set raw water transfer request after applying physical constraints:
        ///     1. initial downstream request (based on ROF deficit)
        ///     2. initial upstream available volume (based on ROF surplus)
        ///     3. upstream available volume in donor reservoir
        ///     4. downstream available capacity in recipient reservoir
        ///     5. raw water release volume cap (proxy for watercourse flooding)
        std::vector<double> temp_vec = {(downstream_storage_target - downstream_utility->getStorageToCapacityRatio()) *
                                        downstream_utility->getTotal_storage_capacity(),
                                        (upstream_utility->getStorageToCapacityRatio() - upstream_storage_target) *
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
 * calculates the targeted storage levels according to ROF triggers
 * @param week week of the year.
 * @param storage_to_rof_table table correlating storage capacities to rof
 * @return
 */
void RawWaterReleases::getUtilityStorageFromROF(
        int week,
        const Matrix3D<double> *storage_to_rof_table) {

    downstream_storage_target = 0.0;
    upstream_storage_target = 0.0;

    bool notchosen1 = True;
    bool notchosen2 = True;

    /// Determine the storage associated with an ROF level
    for (int s = (NO_OF_INSURANCE_STORAGE_TIERS - 1); s > -1; --s) {

        if ((*storage_to_rof_table)(upstream_utility_id, s, week) > rof_triggers[upstream_utility_id] &
            notchosen1) {
            if (s == (NO_OF_INSURANCE_STORAGE_TIERS - 1)) {
                upstream_storage_target = (*storage_to_rof_table)(upstream_utility_id, s, week);
            } else {
                upstream_storage_target = (*storage_to_rof_table)(upstream_utility_id, s + 1, week);
            }
            notchosen1 = False;
        }

        if ((*storage_to_rof_table)(downstream_utility_id, s, week) > rof_triggers[downstream_utility_id] &
            notchosen2) {
            if (s == (NO_OF_INSURANCE_STORAGE_TIERS - 1)) {
                downstream_storage_target = (*storage_to_rof_table)(downstream_utility_id, s, week);
            } else {
                downstream_storage_target = (*storage_to_rof_table)(downstream_utility_id, s + 1, week);
            }
            notchosen2 = False;
        }

        if (notchosen1 == False & notchosen2 == False) {
            break;
        }
    }

    /// ensure that there is no incorrect assignment - it is possible because of the
    /// size of ROF-to-storage table increments that target and current storage levels
    /// will be the same or incorrect
    if (downstream_storage_target < downstream_utility->getStorageToCapacityRatio()) {
        downstream_storage_target = downstream_utility->getStorageToCapacityRatio();
    }

    if (upstream_storage_target > upstream_utility->getStorageToCapacityRatio()) {
        upstream_storage_target = upstream_utility->getStorageToCapacityRatio();
    }
}

void RawWaterReleases::setRealization(unsigned int realization_id) {

}
