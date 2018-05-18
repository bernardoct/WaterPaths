//
// Created by David on 5/7/2018.
//

#include <algorithm>
#include "DirectTreatedWaterTransfer.h"

DirectTreatedWaterTransfer::DirectTreatedWaterTransfer(const int id, const char *name, const int source_reservoir_id,
                                                       const int source_wtp_id, const int recipient_utility_id,
                                                       const double recipient_utility_rof_trigger,
                                                       const double volumetric_rate)
        : DroughtMitigationPolicy(id, DIRECT_TRANSFERS),
          reservoir_id(source_reservoir_id),
          wtp_id(source_wtp_id),
          buyer_id(recipient_utility_id),
          sales_rate(volumetric_rate),
          sales_rof_trigger(recipient_utility_rof_trigger) {

}

void DirectTreatedWaterTransfer::addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources,
                                                     vector<MinEnvFlowControl *> min_env_flow_controls) {
    joint_wtp = dynamic_cast<JointWTP *>(water_sources[wtp_id]);
    parent_allocated_reservoir = dynamic_cast<AllocatedReservoir *>(water_sources[reservoir_id]);
    buying_utility = utilities[buyer_id];

    vector<int> temp_reservoir_utilities = *parent_allocated_reservoir->getUtilities_with_allocations();

    cout << "The JointWTP for Direct Transfers is " << joint_wtp->name << endl;
    cout << "The parent AllocatedReservoir is " << parent_allocated_reservoir->name << endl;
    cout << "The purchasing utility is " << buying_utility->name << endl;

    /// Collect parent reservoir utilities
    for (Utility *u : utilities) {
        auto it = find(temp_reservoir_utilities.begin(), temp_reservoir_utilities.end(), u->id);
        /// Must be utility associated with the reservoir and not the buyer
        if (it != temp_reservoir_utilities.end() && u->id != buyer_id) {
            realization_utilities.push_back(u);
            cout << u->name << " (ID " << u->id << ")" << " is a potential selling utility" << endl;
        }
    }

    quantities_sold = vector<double>(realization_utilities.size(),0.0);
    available_excess_treatment_capacity = vector<double>(realization_utilities.size(),0.0);
    available_excess_supply_capacity = vector<double>(realization_utilities.size(),0.0);
}

void DirectTreatedWaterTransfer::applyPolicy(int week) {
    /// This is a simplified version of treated water transfers from an allocated reservoir
    /// through a jointly-operated water treatment plant to a buying utility.
    /// No piping constraints are accounted for, only wtp and reservoir capacities.

    /// 1. If a buyer's ROF is above trigger level, it will request a transfer from the reservoir.
    ///    The JointWTP through which transfers occur must also be online.
    ///    Quantity requested is the quantity necessary to reduce ROF back to the trigger level.
    double buyer_target_storage_ratio = 0.0;
    double buyer_requested_transfer_volume = 0.0;
    if (buying_utility->getRisk_of_failure() > sales_rof_trigger && joint_wtp->isOnline()) {
        cout << "ROF for " << buying_utility->name
             << " exceeds trigger (" << sales_rof_trigger
             << "), direct treated water transfer initiated in week " << week << endl;

        buyer_target_storage_ratio = getBuyerStorageFromROF(week, storage_to_rof_table_, buyer_id);
        if (buyer_target_storage_ratio - buying_utility->getStorageToCapacityRatio() > 0)
            buyer_requested_transfer_volume = (buyer_target_storage_ratio - buying_utility->getStorageToCapacityRatio())
                                              * buying_utility->getTotal_storage_capacity();

        cout << "The initial requested transfer volume is " << buyer_requested_transfer_volume << endl;
        initial_request = buyer_requested_transfer_volume;

        if (buyer_requested_transfer_volume > 0) {
            /// 2. Any of the utilities with (a) spare treatment capacity and (b) spare reservoir supply storage
            ///    may sell transfers to the buyer.
            vector<int> available_selling_utility_ids;
            vector<double> available_selling_utility_treatment_capacity_in_joint_wtp;
            vector<double> available_selling_utility_spare_reservoir_storage;
            for (Utility *u : realization_utilities) {
                if (joint_wtp->getAllocatedTreatmentCapacity(u->id) > 0
                    && parent_allocated_reservoir->getAllocatedTreatmentCapacity(u->id)
                       - parent_allocated_reservoir->getAllocatedDemand(u->id) > 0
                    && parent_allocated_reservoir->getAvailableAllocatedVolume(u->id)
                       > STORAGE_CAPACITY_RATIO_FAIL * parent_allocated_reservoir->getAllocatedCapacity(u->id)) {

                    available_selling_utility_ids.push_back(u->id);

                    vector<double> temp_utility_treatment_capacity_options =
                            {joint_wtp->getAllocatedTreatmentCapacity(u->id),
                             parent_allocated_reservoir->getAllocatedTreatmentCapacity(u->id)
                             - parent_allocated_reservoir->getAllocatedDemand(u->id)};
                    available_selling_utility_treatment_capacity_in_joint_wtp.
                            push_back(*std::min_element(temp_utility_treatment_capacity_options.begin(),
                                                        temp_utility_treatment_capacity_options.end()));

                    available_selling_utility_spare_reservoir_storage.push_back(
                            parent_allocated_reservoir->getAvailableAllocatedVolume(u->id)
                            - STORAGE_CAPACITY_RATIO_FAIL * parent_allocated_reservoir->getAllocatedCapacity(u->id));

                    cout << u->name << " has available storage and treatment capacity to sell transfers" << endl;
                }
            }

            /// 3. Sales will be allocated based on (a) which utility has most excess at time of request, and
            ///    (b - FUTURE??) which selling party is cheapest

            /// 3a. Sort to get utility ids in order from most excess available to least
            vector<double> temp_sorted_selling_utility_joint_wtp =
                    available_selling_utility_treatment_capacity_in_joint_wtp;
            sort(temp_sorted_selling_utility_joint_wtp.begin(),
                 temp_sorted_selling_utility_joint_wtp.end());
            vector<int> sorted_selling_utility_ids;
            while (temp_sorted_selling_utility_joint_wtp.size() > 0) {
                for (int i = 0; i < available_selling_utility_treatment_capacity_in_joint_wtp.size(); i++) {
                    if (available_selling_utility_treatment_capacity_in_joint_wtp[i]
                        == temp_sorted_selling_utility_joint_wtp[temp_sorted_selling_utility_joint_wtp.size()-1]) {
                        sorted_selling_utility_ids.push_back(available_selling_utility_ids[i]);

                        cout << "Utility " << available_selling_utility_ids[i] << " has "
                             << temp_sorted_selling_utility_joint_wtp[temp_sorted_selling_utility_joint_wtp.size()-1]
                             << " MG of excess treatment capacity available to transfer water" << endl;

                        temp_sorted_selling_utility_joint_wtp.erase(
                                temp_sorted_selling_utility_joint_wtp.begin()
                                + temp_sorted_selling_utility_joint_wtp.size()-1);
                    }
                }
            }

            /// 3b. Cycle through selling utilities and transfer/sell as much water as possible.
            vector<double> water_transferred(available_selling_utility_ids.size(),0.0);
            for (int i = 0; i < sorted_selling_utility_ids.size(); i++) {
                vector<double> temp_possible_water_to_transfer = {buyer_requested_transfer_volume,
                                                                  available_selling_utility_treatment_capacity_in_joint_wtp[
                                                                          sorted_selling_utility_ids[i]],
                                                                  available_selling_utility_spare_reservoir_storage[
                                                                          sorted_selling_utility_ids[i]]};
                water_transferred[i] = *min_element(temp_possible_water_to_transfer.begin(),
                                                    temp_possible_water_to_transfer.end());
                buyer_requested_transfer_volume -= water_transferred[i];

                cout << "Utility " << sorted_selling_utility_ids[i]
                     << " has transferred " << water_transferred[i]
                     << " and " << buyer_requested_transfer_volume
                     << " request remains to be met (if available)" << endl;

                parent_allocated_reservoir->removeWater(sorted_selling_utility_ids[i], water_transferred[i]);
                buying_utility->addDemand_offset(water_transferred[i]);

                buying_utility->purchaseDirectTreatedWaterTransfer(sales_rate, water_transferred[i]);
                realization_utilities[sorted_selling_utility_ids[i]]->
                        sellDirectTreatedWaterTransfer(sales_rate, water_transferred[i]);

                /// Record values for data collector
                quantities_sold[sorted_selling_utility_ids[i]] = water_transferred[i];
                available_excess_treatment_capacity[sorted_selling_utility_ids[i]] =
                        available_selling_utility_treatment_capacity_in_joint_wtp[sorted_selling_utility_ids[i]];
                available_excess_supply_capacity[sorted_selling_utility_ids[i]] =
                        available_selling_utility_spare_reservoir_storage[sorted_selling_utility_ids[i]];

//                if (buyer_requested_transfer_volume <= 0)
//                    break;
            }
        } else {
            cout << "Because buyer did not request any transfer, none was calculated." << endl;
        }
    }
}

void DirectTreatedWaterTransfer::setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                                                vector<double> &water_sources_rdm, vector<double> &policy_rdm) {

}

/**
 * calculates the targeted storage level of the buyer utility according to ROF trigger
 * @param week week of the realization
 * @param storage_to_rof_table table correlating storage capacities to rof
 * @param utility ID
 * @return
 */
const double DirectTreatedWaterTransfer::getBuyerStorageFromROF(
        int week,
        const vector<Matrix2D<double>> storage_to_rof_table,
        const int u_id) {
    /// Determine the storage associated with an ROF level
    // TODO: IS THIS TABLE NOW INVERTED??? check with raw water transfers later
    for (int s = 0; s < NO_OF_INSURANCE_STORAGE_TIERS; ++s) {
        double tier_rof = *storage_to_rof_table.at(u_id).getPointerToElement(week,s);
        double storage_target_level = (((double)s + 1) / (double)NO_OF_INSURANCE_STORAGE_TIERS);
        if (tier_rof <= sales_rof_trigger && storage_target_level >= buying_utility->getStorageToCapacityRatio()) {
            return storage_target_level;
        }
        if (s == NO_OF_INSURANCE_STORAGE_TIERS-1)
            return 1;
    }
}

const vector<double> DirectTreatedWaterTransfer::getSoldQuantities() {
    return quantities_sold;
}

const double DirectTreatedWaterTransfer::getInitialTransferRequest() {
    return initial_request;
}

const vector<double> DirectTreatedWaterTransfer::getAvailableExcessSupplyCapacityToSell() {
    return available_excess_supply_capacity;
}

const vector<double> DirectTreatedWaterTransfer::getAvailableExcessTreatmentCapacityToSell() {
    return available_excess_treatment_capacity;
}

const vector<int> DirectTreatedWaterTransfer::getPotentialSellingUtilities() {
    vector<int> realization_utility_ids;
    for (Utility *u : realization_utilities)
        realization_utility_ids.push_back(u->id);

    return realization_utility_ids;
}

const int DirectTreatedWaterTransfer::getBuyingUtilityID() {
    return buyer_id;
}
