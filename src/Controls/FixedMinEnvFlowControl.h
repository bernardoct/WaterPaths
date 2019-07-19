//
// Created by bernardoct on 6/29/17.
//

#ifndef TRIANGLEMODEL_FIXEDMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_FIXEDMINENVFLOWCONTROL_H


#include "Base/MinEnvFlowControl.h"

class FixedMinEnvFlowControl : public MinEnvFlowControl {
private:
    double release;
public:
    FixedMinEnvFlowControl(int water_source_id, double release);

    FixedMinEnvFlowControl(const FixedMinEnvFlowControl &min_env_control);

    ~FixedMinEnvFlowControl() override;

    double getRelease(int week) override;
};


#endif //TRIANGLEMODEL_FIXEDMINENVFLOWCONTROL_H
