//
// Created by bct52 on 7/3/17.
//

#ifndef TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H


#include "../Base/MinEnvironFlowControl.h"

class FallsLakeMinEnvFlowControl : public MinEnvironFlowControl {
public:
    FallsLakeMinEnvFlowControl(int water_source_id,
                               const vector<int> &aux_water_sources_id,
                               const vector<int> &aux_utilities_ids, int type);

    double getRelease(int week) override;

};


#endif //TRIANGLEMODEL_FALLSLAKEMINENVFLOWCONTROL_H
