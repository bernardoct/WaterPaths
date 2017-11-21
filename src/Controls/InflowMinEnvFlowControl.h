//
// Created by bct52 on 6/29/17.
//

#ifndef TRIANGLEMODEL_INFLOWMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_INFLOWMINENVFLOWCONTROL_H


#include "Base/MinEnvironFlowControl.h"

class InflowMinEnvFlowControl : public MinEnvironFlowControl {
public:
    InflowMinEnvFlowControl(
            int water_source_id, const vector<int> &aux_water_sources_ids,
            const vector<double> *inflows, const vector<double> *releases);

    const vector<double> *inflows;
    const vector<double> *releases;

private:
    double getRelease(int week);
};


#endif //TRIANGLEMODEL_INFLOWMINENVFLOWCONTROL_H
