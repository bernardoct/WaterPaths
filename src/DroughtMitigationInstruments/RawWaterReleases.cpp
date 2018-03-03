//
// Created by David on 9/18/2017.
//

#include <algorithm>
#include "RawWaterReleases.h"
#include "../Utils/Utils.h"

RawWaterReleases::RawWaterReleases(const int id, const char *name,
                                   const int upstream_utility_id,
                                   const int downstream_utility_id,
                                   const int upstream_reservoir_id,
                                   const int downstream_reservoir_id,
                                   const double max_releases,
                                   const vector<double> rof_triggers,
                                   const double raw_water_transfer_downstream_allocation_ratio,
                                   const double rate_per_volume)
        : DroughtMitigationPolicy(id, RAW_WATER_TRANSFERS),
          upstream_utility_id(upstream_utility_id),
          downstream_utility_id(downstream_utility_id),
          upstream_reservoir_id(upstream_reservoir_id),
          downstream_reservoir_id(downstream_reservoir_id),
          max_releases(max_releases),
          rof_triggers(rof_triggers),
          raw_water_transfer_downstream_allocation_ratio(raw_water_transfer_downstream_allocation_ratio),
          rate_per_volume(rate_per_volume) {

    utilities_ids = vector<int> {upstream_utility_id, downstream_utility_id};
}

void RawWaterReleases::addSystemComponents(vector<Utility *> utilities,
                                           vector<WaterSource *> water_sources,
                                           vector<MinEnvironFlowControl *> min_env_flow_controls) {
    upstream_reservoir =
            dynamic_cast<Reservoir *>(water_sources[upstream_reservoir_id]);
    downstream_reservoir =
            dynamic_cast<Reservoir *>(water_sources[downstream_reservoir_id]);
    upstream_utility = utilities[upstream_utility_id];
    downstream_utility = utilities[downstream_utility_id];

    /// Gets the control rule that acts on reservoir of id reservoir_id.
    for (MinEnvironFlowControl *mefc : min_env_flow_controls) {
        if (mefc->water_source_id == upstream_reservoir_id) {
            upstream_min_env_flow_control = mefc;
        }
    }
}

void RawWaterReleases::applyPolicy(int week) {

    raw_water_transfer_volume = 0.0;
    denied_requests = 0;
    storage_targets = {0.0, 0.0};

    /// Determine storage deficit for downstream utility and available capacity for
    /// upstream utility. Do this in EVERY week so that I can plot ROF guide curves
    storage_targets[UPSTREAM_UTILITY] = getUtilityStorageFromROF(week,
                                                                 storage_to_rof_table_, upstream_utility_id);
    storage_targets[DOWNSTREAM_UTILITY] = getUtilityStorageFromROF(week,
                                                                   storage_to_rof_table_, downstream_utility_id);

    /// Raw water transfers are calculated if upstream storage is above the ROF target
    /// and downstream storage is below ROF target
    if ((downstream_utility->getStorageToCapacityRatio() < storage_targets[DOWNSTREAM_UTILITY]) &
        (upstream_utility->getStorageToCapacityRatio() > storage_targets[UPSTREAM_UTILITY])) {

        /// optional: print detail in each instance of a transfer
        if (LOUD) {
            cout << "Raw water transfer initiated in week " << week << " between " << upstream_utility->name
                 << " and " << downstream_utility->name << endl;
            cout << "Recipient Reservoir Name: " << downstream_reservoir->name << endl;
            cout << "Donor Reservoir Name: " << upstream_reservoir->name << endl;

            cout << "Donor Initial Storage Target: " << storage_targets[UPSTREAM_UTILITY] << endl;
            cout << "Recipient Initial Storage Target: " << storage_targets[DOWNSTREAM_UTILITY] << endl;
        }

        /// ensure that there is no incorrect assignment - it is possible because of the
        /// size of ROF-to-storage table increments that target and current storage levels
        /// will be the same or incorrect
        if (storage_targets[UPSTREAM_UTILITY] > upstream_utility->getStorageToCapacityRatio())
            storage_targets[UPSTREAM_UTILITY] = upstream_utility->getStorageToCapacityRatio();
        if (storage_targets[DOWNSTREAM_UTILITY] < downstream_utility->getStorageToCapacityRatio())
            storage_targets[DOWNSTREAM_UTILITY] = downstream_utility->getStorageToCapacityRatio();

        /// optional: print detail in each instance of a transfer
        if (LOUD) {
            cout << "Donor Adjusted Storage Target: " << storage_targets[UPSTREAM_UTILITY] << endl;
            cout << "Recipient Adjusted Storage Target: " << storage_targets[DOWNSTREAM_UTILITY] << endl;
        }

        /// Set raw water transfer request after applying physical constraints:
        ///     1. initial downstream request (based on ROF deficit)
        ///     2. initial upstream available volume (based on ROF surplus)
        ///     3. upstream available volume in donor reservoir
        ///     4. downstream available capacity in recipient reservoir
        ///     5. raw water release volume cap (proxy for watercourse flooding)
        double initial_request = (storage_targets[DOWNSTREAM_UTILITY] -
                downstream_utility->getStorageToCapacityRatio()) * downstream_utility->getTotal_storage_capacity();
        double initial_available_volume = (upstream_utility->getStorageToCapacityRatio() -
                storage_targets[UPSTREAM_UTILITY]) * upstream_utility->getTotal_storage_capacity();
        double available_source_volume = upstream_reservoir->getAvailableAllocatedVolume(upstream_utility_id);
        double available_recipient_source_volume =
                (downstream_reservoir->getAllocatedCapacity(downstream_utility_id) -
                 downstream_reservoir->getAvailableAllocatedVolume(downstream_utility_id));

        std::vector<double> temp_vec = {initial_request, initial_available_volume, available_source_volume,
                                        available_recipient_source_volume, max_releases};

        raw_water_transfer_volume = *std::min_element(temp_vec.begin(), temp_vec.end());

        /// if a transfer is requested but cannot be, even partially, met, mark it as such.
        if (raw_water_transfer_volume == 0)
            denied_requests = 1;

        /// apply additional constraints:
        ///     1. remove required environmental releases from total
        ///     2. non-negativity
        raw_water_transfer_volume -= upstream_min_env_flow_control->getRelease(week);

        if (raw_water_transfer_volume < 0)
            raw_water_transfer_volume = 0.0;

        /// optional: print detail in each instance of a transfer
        if (LOUD) {
            cout << "RWT Final Volume: " << raw_water_transfer_volume << endl;

            cout << "Donor Initial Storage Ratio: " << upstream_utility->getStorageToCapacityRatio() << endl;
            cout << "Recipient Initial Storage Ratio: " << downstream_utility->getStorageToCapacityRatio() << endl;
        }

        /// adjust supply storage (and water quality storage, when applicable) levels in each reservoir
        downstream_reservoir->addWater(WATER_QUALITY_ALLOCATION,
                                       raw_water_transfer_volume *
                                       (1 - raw_water_transfer_downstream_allocation_ratio));
        downstream_reservoir->addWater(downstream_utility_id,
                                       raw_water_transfer_volume *
                                       raw_water_transfer_downstream_allocation_ratio);
        upstream_reservoir->removeWater(upstream_utility_id, raw_water_transfer_volume);

        /// administer payments between utilities
        /// UNITS OF VOLUME ASSUMED TO BE MG, UNITS OF COST ASSUMED $MM/MG
        upstream_utility->sellRawWaterTransfer(rate_per_volume, raw_water_transfer_volume);
        downstream_utility->purchaseRawWaterTransfer(rate_per_volume, raw_water_transfer_volume);

        /// optional: print detail in each instance of a transfer
        if (LOUD) {
            cout << "Recipient Allocation Final Volume: " << downstream_reservoir->
                    getAvailableAllocatedVolume(downstream_utility_id) << endl;
            cout << "Recipient Allocation Capacity: " << downstream_reservoir->
                    getAllocatedCapacity(downstream_utility_id) << endl;
            cout << "Recipient Reservoir Final Volume: " << downstream_reservoir->getAvailableVolume() << endl;
            cout << "Recipient Reservoir Capacity: " << downstream_reservoir->getSupplyCapacity() << endl;
        }
    }
}

/**
 * calculates the targeted storage level of a utility according to ROF triggers
 * @param week week of the year. NOW WEEK OF ENTIRE REALIZATION
 * @param storage_to_rof_table table correlating storage capacities to rof
 * @param utility ID
 * @return
 */
const double RawWaterReleases::getUtilityStorageFromROF(
        int week,
        const Matrix3D<double> *storage_to_rof_table,
        const int u_id) {

    /// Determine the storage associated with an ROF level
    for (int s = NO_OF_INSURANCE_STORAGE_TIERS - 1; s > -1; --s) {
//        double temp = *storage_to_rof_table->getPointerToElement(week,u_id,s);
        if (*storage_to_rof_table->getPointerToElement(week,u_id,s) >= rof_triggers[u_id]) {
                // ERROR IS HERE, table has changed shape, adjustment made Feb 28,2018
            return (((double)s + 1) / (double)NO_OF_INSURANCE_STORAGE_TIERS);
        }
    }
}

void RawWaterReleases::setRealization(unsigned int realization_id, vector<vector<double>> *utilities_rdm,
                                  vector<vector<double>> *water_sources_rdm, vector<vector<double>> *policy_rdm) {}

const double &RawWaterReleases::getRawWaterTransferVolume() const {
    return raw_water_transfer_volume;
}

const int &RawWaterReleases::getRawWaterTransferDenials() const {
    return denied_requests;
}

const vector<double> &RawWaterReleases::getUtilityTargetStorageLevels() const {
    return storage_targets;
}