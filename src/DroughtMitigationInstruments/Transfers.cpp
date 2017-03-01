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
            if (id > highest_utility_id) highest_utility_id = (unsigned long) id;
        }

    this->buyers_transfer_capacities.resize(highest_utility_id, 0);
    this->buyers_transfer_triggers.resize(highest_utility_id, 0);

    /// Create vector with ids of all utilities involved in the transfer from source utility.
    int bid;
    utilities_ids.push_back(source_utility_id);
    for (int i = 0; i < buyers_ids.size(); ++i)
        for (int j = 0; j < buyers_ids[i].size(); ++j) {
            bid = buyers_ids[i][j];
            utilities_ids.push_back(bid);
            this->buyers_transfer_capacities.at((unsigned long) bid) = buyers_transfer_capacities[i][j];
            this->buyers_transfer_triggers.at((unsigned long) bid) = buyers_transfer_triggers[i][j];
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
        vector<bool> transfer_request(highest_utility_id, false);

        /// Calculate total volume available for transfers in source utility.
        double available_transfer_volume = (source_utility->getTotal_treatment_capacity() - source_treatment_buffer) *
                                           PEAKING_FACTOR - source_utility->getDemand(week);


        for (int i = 0; i < transfer_volumes.size(); ++i) {
            transfer_volumes[i] = available_transfer_volume * requesting_utilities_rofs[i] / sum_rofs;
            transfer_request[i] = true;
        }

        // IMPLEMENT THE CHECKS TO SEE IF TRANSFER VOLUMES ARE OK GIVEN CAPACITIES AND OTHER REQUIREMENTS.
        // IMPLEMENT THE APPLICATION OF TRANSFERS AND DATA COLLECTION.
        // SETUP A CONSTRAINED ALLOCATION PROBLEM TO BE SOLVED WITH LP. MINIMIZE DEVIATION FROM ROF VALUES.



    }

}

void applyMinTransferConstrain(double available_transfer_volume,
                               vector<double> transfer_volumes_requests, vector<double> requesting_utilities_rofs,
                               vector<bool> requests_transfers) {

    double rofs_partial_sum = 0;
    int n_transfer_requests = 0;
    double fill_in_volume = 0;
    double extra_volume = 0;
    vector<double> transfer_volumes_min_constrained;

    for (auto r : requests_transfers) n_transfer_requests += r;
    double min_transfer_volume_constrain = available_transfer_volume / (n_transfer_requests + 1);

    for (int i = 0; i < transfer_volumes_requests.size(); ++i) {
        extra_volume += transfer_volumes_requests[i] - min_transfer_volume_constrain;
    }


    for (int i = 0; i < transfer_volumes_requests[i]; ++i) {
        if (transfer_volumes_requests[i] < min_transfer_volume_constrain)
            transfer_volumes_requests[i] = min_transfer_volume_constrain;
        else

    }


    if(durhamRequest < transferFloor*caryExtraCapacity*durhamRequestO)
    {
        durhamRequest = transferFloor*caryExtraCapacity*durhamRequestO;

        owasaRequest = ((caryExtraCapacity-durhamRequest)*owasaRisk*owasaRequestO)/(raleighRisk*raleighRequestO+owasaRisk*owasaRequestO+.00001);

        raleighRequest = ((caryExtraCapacity-durhamRequest)*raleighRisk*raleighRequestO)/(raleighRisk*raleighRequestO+owasaRisk*owasaRequestO+.00001);

        if(owasaRequest < transferFloor*caryExtraCapacity*owasaRequestO)
        {
            owasaRequest = transferFloor*caryExtraCapacity*owasaRequestO;

            raleighRequest = (caryExtraCapacity-durhamRequest-owasaRequest)*raleighRequestO;
        }
        if(raleighRequest < transferFloor*caryExtraCapacity*raleighRequestO)
        {
            raleighRequest = transferFloor*caryExtraCapacity*raleighRequestO;

            owasaRequest = (caryExtraCapacity - durhamRequest - raleighRequest)*owasaRequestO;
        }
    }



}

