//
// Created by bct52 on 7/27/17.
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
    MinEnvironFlowControl *upstream_min_env_flow_control;

    const int upstream_utility_id;
    const int downstream_utility_id;
    const int upstream_reservoir_id;
    const int downstream_reservoir_id;
    const double max_releases;
    const vector<double> rof_triggers;
    const double raw_water_transfer_downstream_allocation_ratio;

    double raw_water_transfer_volume;
    vector<double> storage_targets;

public:
    RawWaterReleases(const int id,
                     const int upstream_utility_id,
                     const int downstream_utility_id,
                     const int upstream_reservoir_id,
                     const int downstream_reservoir_id,
                     const double max_releases,
                     const vector<double> rof_triggers,
                     const double raw_water_transfer_downstream_allocation_ratio);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities,
                             vector<WaterSource *> water_sources,
                             vector<MinEnvironFlowControl *> min_env_flow_controls) override;

    void setRealization(unsigned int realization_id) override;

    void getUtilityStorageFromROF(int week, const Matrix3D<double> *storage_to_rof_table);
    const double getUtilityStorageFromROF(int week, const Matrix3D<double> *storage_to_rof_table,
                                          const int u_id);

    const double &getRawWaterTransferVolume() const;

    const vector<double> &getUtilityTargetStorageLevels() const;
};


#endif //TRIANGLEMODEL_RAWWATERRELEASES_H
