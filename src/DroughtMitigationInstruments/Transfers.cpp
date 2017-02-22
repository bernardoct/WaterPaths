//
// Created by bernardo on 2/21/17.
//

#include "Transfers.h"

Transfers::Transfers(const int id, const vector<int> &utilities_id, const int source_utility_id,
                     const double source_treatment_buffer, const vector<double> &buyers_transfer_capacities,
                     const vector<double> &buyers_transfer_triggers)
        : DroughtMitigationPolicy(id, utilities_id), source_utility_id(source_utility_id),
          buyers_transfer_capacities(buyers_transfer_capacities), buyers_transfer_triggers(buyers_transfer_triggers),
          source_treatment_buffer(source_treatment_buffer) {

}

void Transfers::addSourceUtility(Utility *source_utility) {
    this->source_utility = source_utility;
}

/**
 * Adds buying utility.
 * @param utility
 */
void Transfers::addUtility(Utility *utility) {
    buying_utilities.push_back(utility);
}

void Transfers::applyPolicy(int week) {
    /// Calculate total volume available for transfers in source utility.
    double available_transfer_volume = (source_utility->getTotal_treatment_capacity() - source_treatment_buffer) *
                              PEAKING_FACTOR - source_utility->getDemand(week);

    /// Check which utilities need transfers.
    vector<Utility *> utilities_requesting_transfers;
    for (unsigned int i = 0; i < utilities_ids.size(); ++i) {
        if (buying_utilities.at(i)->getRisk_of_failure() > buyers_transfer_triggers.at(i))
            utilities_requesting_transfers.push_back(buying_utilities[i]);
    }


}
