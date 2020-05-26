//
// Created by Bernardo on 5/10/2020.
//

#ifndef WATERPATHS_TRANSFERSBILATERAL_H
#define WATERPATHS_TRANSFERSBILATERAL_H


#include "Base/DroughtMitigationPolicy.h"

class TransfersBilateral : public DroughtMitigationPolicy {
    double transfer_overhead;
    const vector<double> &pipe_transfer_capacities;
    double source_treatment_buffer;
    double surcharge_percentage_fee;
    const vector<double> &transfer_triggers;
    vector<double> transfered_volumes;

public:
    TransfersBilateral(int id,
                       const vector<double> &pipe_transfer_capacities,
                       double source_treatment_buffer,
                       double surcharge_percentage_fee,
                       const vector<double> &transfer_triggers,
                       const vector<int> &utilities_ids);

    TransfersBilateral(const TransfersBilateral &transfer_caesb);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities,
                             vector<WaterSource *> water_sources,
                             vector<MinEnvFlowControl *> min_env_flow_controls) override;

    void setRealization(unsigned long realization_id, const vector<double> &utilities_rdm,
                        const vector<double> &water_sources_rdm, const vector<double> &policy_rdm) override;

    double performTransfer(Utility *sender, Utility *receiver,
                         double pumping_capacity,
                         int week) const;

    const vector<double> &getTransferedVolumes() const;
};


#endif //WATERPATHS_TRANSFERSBILATERAL_H
