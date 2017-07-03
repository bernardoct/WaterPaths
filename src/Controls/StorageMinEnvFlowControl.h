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
            vector<double> *storages, vector<double> *releases);
    const vector<double> *storages;
    const vector<double> *releases;

private:
    double getRelease(int week);
};


#endif //TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
