//
// Created by bernardoct on 6/29/17.
//

#ifndef TRIANGLEMODEL_FIXEDMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_FIXEDMINENVFLOWCONTROL_H


#include "Base/MinEnvironFlowControl.h"

class FixedMinEnvFlowcontrol : public MinEnvironFlowControl {
private:
    double release;
public:
    FixedMinEnvFlowcontrol(
            int water_source_id, double release);

    double getRelease(int week) override;
};


#endif //TRIANGLEMODEL_FIXEDMINENVFLOWCONTROL_H
