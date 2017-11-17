//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H


#include "Base/MinEnvironFlowControl.h"

class StorageMinEnvFlowControl : public MinEnvironFlowControl {
public:
    StorageMinEnvFlowControl(
            int water_source_id, const vector<int> &aux_water_sources_ids,
            vector<float> *storages, vector<float> *releases);
    const vector<float> *storages;
    const vector<float> *releases;

private:
    float getRelease(int week);
};


#endif //TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
