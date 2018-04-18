//
// Created by David on 9/18/2017.
//

#ifndef TRIANGLEMODEL_RAWWATERRELEASES_H
#define TRIANGLEMODEL_RAWWATERRELEASES_H

#include "../SystemComponents/Utility.h"
#include "Base/DroughtMitigationPolicy.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"

class RawWaterReleases : public DroughtMitigationPolicy {
private:
    Utility *upstream_utility;
    Utility *downstream_utility;
    Reservoir *upstream_reservoir;
    Reservoir *downstream_reservoir;
    MinEnvFlowControl *upstream_min_env_flow_control;

    const int upstream_utility_id;
    const int downstream_utility_id;
    const int upstream_reservoir_id;
    const int downstream_reservoir_id;
    const double max_releases;
    const vector<double> rof_triggers;
    const double raw_water_transfer_downstream_allocation_ratio;
    const double rate_per_volume;

    int denied_requests;
    double raw_water_transfer_volume;
    vector<double> storage_targets;

    const int UPSTREAM_UTILITY = 0;
    const int DOWNSTREAM_UTILITY = 1;

    const int LOUD = 0;

public:
    RawWaterReleases(const int id,
                     const char *name,
                     const int upstream_utility_id,
                     const int downstream_utility_id,
                     const int upstream_reservoir_id,
                     const int downstream_reservoir_id,
                     const double max_releases,
                     const vector<double> rof_triggers,
                     const double raw_water_transfer_downstream_allocation_ratio,
                     const double rate_per_volume);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities,
                             vector<WaterSource *> water_sources,
                             vector<MinEnvFlowControl *> min_env_flow_controls) override;

    void setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                        vector<double> &water_sources_rdm, vector<double> &policy_rdm) override;

    const double getUtilityStorageFromROF(int week, vector<Matrix2D<double>> storage_to_rof_table_, const int u_id);

    const double &getRawWaterTransferVolume() const;

    const vector<double> &getUtilityTargetStorageLevels() const;

    const int &getRawWaterTransferDenials() const;
};

#endif //TRIANGLEMODEL_RAWWATERRELEASES_H
