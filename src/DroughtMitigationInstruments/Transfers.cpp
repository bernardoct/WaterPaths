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

void Transfers::applyPolicy(int week) {
    double available_volume;

}
