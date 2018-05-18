//
// Created by David on 5/7/2018.
//

#ifndef TRIANGLEMODEL_DIRECTTREATEDWATERTRANSFER_H
#define TRIANGLEMODEL_DIRECTTREATEDWATERTRANSFER_H


#include "Base/DroughtMitigationPolicy.h"
#include "../SystemComponents/WaterSources/JointWTP.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"

class DirectTreatedWaterTransfer : public DroughtMitigationPolicy {

    JointWTP *joint_wtp = nullptr;
    AllocatedReservoir *parent_allocated_reservoir = nullptr;

    Utility *buying_utility = nullptr;

    const int reservoir_id;
    const int wtp_id;
    const int buyer_id;

    const double sales_rate;
    const double sales_rof_trigger;

    double initial_request = 0;

    vector<double> quantities_sold;
    vector<double> available_excess_treatment_capacity;
    vector<double> available_excess_supply_capacity;

public:
    DirectTreatedWaterTransfer(const int id,
                               const char *name,
                               const int source_reservoir_id,
                               const int source_wtp_id,
                               const int recipient_utility_id,
                               const double recipient_utility_rof_trigger,
                               const double volumetric_rate);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities,
                             vector<WaterSource *> water_sources,
                             vector<MinEnvFlowControl *> min_env_flow_controls) override;

    void setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                        vector<double> &water_sources_rdm, vector<double> &policy_rdm) override;

    const double getBuyerStorageFromROF(int week, const vector<Matrix2D<double>> storage_to_rof_table, const int u_id);

    const vector<int> getPotentialSellingUtilities();

    const vector<double> getSoldQuantities();

    const vector<double> getAvailableExcessTreatmentCapacityToSell();

    const vector<double> getAvailableExcessSupplyCapacityToSell();

    const double getInitialTransferRequest();

    const int getBuyingUtilityID();

};


#endif //TRIANGLEMODEL_DIRECTTREATEDWATERTRANSFER_H
