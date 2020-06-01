//
// Created by Bernardo on 5/10/2020.
//

#include "../Utils/Utils.h"
#include "TransfersBilateral.h"

TransfersBilateral::TransfersBilateral(const int id,
                                       const vector<double> &pipe_transfer_capacities,
                                       double source_treatment_buffer,
                                       double surcharge_percentage_fee,
                                       const vector<double> &transfer_triggers,
                                       const vector<int> &utilities_ids)
        : DroughtMitigationPolicy(id, BILATERAL_TRANSFERS),
          source_treatment_buffer(source_treatment_buffer),
          surcharge_percentage_fee(surcharge_percentage_fee),
          pipe_transfer_capacities(pipe_transfer_capacities),
          transfer_overhead(surcharge_percentage_fee),
          transfer_triggers(transfer_triggers) {
    this->utilities_ids = utilities_ids;
}

TransfersBilateral::TransfersBilateral(const TransfersBilateral &transfer_caesb):
        DroughtMitigationPolicy(transfer_caesb.id, BILATERAL_TRANSFERS),
        source_treatment_buffer(transfer_caesb.source_treatment_buffer),
        surcharge_percentage_fee(transfer_caesb.surcharge_percentage_fee),
        pipe_transfer_capacities(transfer_caesb.pipe_transfer_capacities),
        transfer_overhead(transfer_caesb.surcharge_percentage_fee),
        transfer_triggers(transfer_caesb.transfer_triggers) {
    this->utilities_ids = transfer_caesb.utilities_ids;
}

void TransfersBilateral::applyPolicy(int week) {

    double transfer_volume = 0;
    transfer_volume = performTransfer(realization_utilities[0],
                                      realization_utilities[1],
                                      pipe_transfer_capacities[1], week);

    transfered_volumes = {-transfer_volume, transfer_volume};

    if (transfer_volume == 0) {
        transfer_volume = performTransfer(realization_utilities[1],
                                          realization_utilities[0],
                                          pipe_transfer_capacities[0], week);
        transfered_volumes = {transfer_volume, -transfer_volume};
    }
}

double TransfersBilateral::performTransfer(Utility *sender, Utility *receiver,
                                           double pumping_capacity,
                                           int week) const {

    double transfer_volume = 0;
    if (receiver->getRisk_of_failure() > 0. &&
        sender->getRisk_of_failure() == 0.) {
        // Calculate transfer volume
        double available_transfer_volume =
                (sender->getTotal_treatment_capacity()
                 - source_treatment_buffer) * PEAKING_FACTOR
                - sender->getUnrestrictedDemand();
        transfer_volume = max(min(available_transfer_volume,
                              pumping_capacity), 0.);

        // Perform transfer and apply tariffs
        int price_week = Utils::weekOfTheYear(week);
        receiver->setDemand_offset(transfer_volume,
                                   sender->waterPrice(price_week) *
                                   transfer_overhead);
        sender->setDemand_offset(-transfer_volume,
                                 2. * sender->waterPrice(price_week));
    }

    return transfer_volume;
}

void TransfersBilateral::addSystemComponents(vector<Utility *> utilities,
                                             vector<WaterSource *> water_sources,
                                             vector<MinEnvFlowControl *> min_env_flow_controls) {
    realization_utilities = {utilities[0], utilities[1]};
}

void TransfersBilateral::setRealization(
        unsigned long realization_id, const vector<double> &utilities_rdm,
        const vector<double> &water_sources_rdm,
        const vector<double> &policy_rdm) {}

const vector<double> &TransfersBilateral::getTransferedVolumes() const {
    return transfered_volumes;
}
