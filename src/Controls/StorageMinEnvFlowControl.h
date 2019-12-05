//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H


#include "Base/MinEnvFlowControl.h"

class StorageMinEnvFlowControl : public MinEnvFlowControl {
public:
    StorageMinEnvFlowControl(int water_source_id,
            const vector<int> &aux_water_sources_ids,
            vector<double>& storages, vector<double>& releases);

    StorageMinEnvFlowControl(int water_source_id, vector<double> &storages,
                             vector<double> &releases);

    StorageMinEnvFlowControl(const StorageMinEnvFlowControl &min_env_control);

    const vector<double> &storages;
    const vector<double> &releases;

private:
    double getRelease(int week);
};


#endif //TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
