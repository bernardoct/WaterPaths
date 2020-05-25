//
// Created by bct52 on 6/29/17.
//

#ifndef TRIANGLEMODEL_INFLOWMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_INFLOWMINENVFLOWCONTROL_H


#include "Base/MinEnvFlowControl.h"

class InflowMinEnvFlowControl : public MinEnvFlowControl {
private:
    int last_tier = 0;
    vector<double> inflows;
    double getRelease(int week) override;

    unsigned long n_inflows;
public:
    InflowMinEnvFlowControl(int water_source_id, const vector<double> inflows,
                                const vector<double> &releases);

    InflowMinEnvFlowControl(const InflowMinEnvFlowControl &min_env_control);

    ~InflowMinEnvFlowControl() override;

    const vector<double> &getInflows() const;

    const vector<double> &getReleases() const;

    const vector<double>& releases;
};


#endif //TRIANGLEMODEL_INFLOWMINENVFLOWCONTROL_H
