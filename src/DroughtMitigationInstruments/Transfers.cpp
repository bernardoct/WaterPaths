//
// Created by bernardo on 2/21/17.
//

#include "Transfers.h"

Transfers::Transfers(const int id, const int source_utility_id,
                     const double source_treatment_buffer, const vector<vector<int>> &buyers_ids,
                     const vector<vector<double>> &buyers_transfer_capacities,
                     const vector<vector<double>> &buyers_transfer_triggers) : DroughtMitigationPolicy(id),
                                                                               source_utility_id(source_utility_id),
                                                                               source_treatment_buffer(
                                                                                       source_treatment_buffer),
                                                                               buyers_ids(buyers_ids) {
    for (vector<int> ids : buyers_ids)
        for (int id : ids) {
            if (id > highest_utility_id) highest_utility_id = id;
        }

    this->buyers_transfer_capacities.resize(highest_utility_id, 0);
    this->buyers_transfer_triggers.resize(highest_utility_id, 0);

    /// Create vector with ids of all utilities involved in the transfer from source utility.
    int id;
    utilities_ids.push_back(source_utility_id);
    for (int i = 0; i < buyers_ids.size(); ++i)
        for (int j = 0; j < buyers_ids[i].size(); ++j) {
            id = buyers_ids[i][j];
            utilities_ids.push_back(id);
            this->buyers_transfer_capacities.at((unsigned long) id) = buyers_transfer_capacities[i][j];
            this->buyers_transfer_triggers.at((unsigned long) id) = buyers_transfer_triggers[i][j];
        }
}

/**
 * Adds source and buying utility. The highest utility ID is used to create vectors of appropriate length in other
 * methods.
 * @param utility Utility to be added.
 */
void Transfers::addUtility(Utility *utility) {
    if (utility->id == source_utility_id)
        source_utility = utility;
    else {
        buying_utilities.insert(pair<int, Utility *>(utility->id, utility));
    }
}

void Transfers::applyPolicy(int week) {

    vector<double> requesting_utilities_rofs(highest_utility_id, 0);

    /**
     * Get summation of rofs of utilities needing transfers. This is for splitting the available flow rate and for
     * checking weather any transfers will be needed.
     */
    int id;
    double sum_rofs = 0;
    for (auto u : buying_utilities) {
        id = u.second->id;
        if (u.second->getRisk_of_failure() > buyers_transfer_triggers[id]) {
            sum_rofs += u.second->getRisk_of_failure();
            requesting_utilities_rofs[id] = u.second->getRisk_of_failure();
        }
    }

    /// Check if transfers will be needed and, if so, perform the transfers.
    if (sum_rofs > 0) {

        vector<double> transfer_volumes(highest_utility_id, 0);

        /// Calculate total volume available for transfers in source utility.
        double available_transfer_volume = (source_utility->getTotal_treatment_capacity() - source_treatment_buffer) *
                                           PEAKING_FACTOR - source_utility->getDemand(week);


        for (int i = 0; i < transfer_volumes.size(); ++i) {
            transfer_volumes[i] = available_transfer_volume * requesting_utilities_rofs[i] / sum_rofs;
        }

        // IMPLEMENT THE CHECKS TO SEE IF TRANSFER VOLUMES ARE OK GIVEN CAPACITIES AND OTHER REQUIREMENTS.
        // IMPLEMENT THE APPLICATION OF TRANSFERS AND DATA COLLECTION.
    }

}
