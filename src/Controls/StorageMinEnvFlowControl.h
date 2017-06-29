//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H


#include "Base/MinEnvironFlowControl.h"

class StorageMinEnvFlowControl : public MinEnvironFlowControl {
    vector<double> *storages;
    vector<double> *releases;


    double getRelease(int week);
};


#endif //TRIANGLEMODEL_STORAGEMINENVFLOWCONTROL_H
