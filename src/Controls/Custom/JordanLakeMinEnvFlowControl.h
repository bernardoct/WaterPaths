//
// Created by bct52 on 7/3/17.
//

#ifndef TRIANGLEMODEL_JORDANLAKEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_JORDANLAKEMINENVFLOWCONTROL_H


#include "../StorageMinEnvFlowControl.h"

class JordanLakeMinEnvFlowControl : public MinEnvironFlowControl {
private:
    Catchment* lillington_gage_catchment;
    double rel02, rel04, rel06_up, min_rel_llt06, min_rel_llt08,
            min_rel_llt08_up, quality_capacity, supply_capacity;

public:
    JordanLakeMinEnvFlowControl(
                int jordan_lake_id, Catchment *lillington_gage_catchment,
                double rel02, double rel04, double rel06_up, double min_rel_llt06,
                double min_rel_llt08, double min_rel_llt08_up,
                double quality_capacity, double supply_capacity);

    JordanLakeMinEnvFlowControl(
            const JordanLakeMinEnvFlowControl &jl_min_flow_control);

    JordanLakeMinEnvFlowControl &operator=
            (const JordanLakeMinEnvFlowControl jl_min_flow_control);

    double getRelease(int week) override;

    void setRealiation(unsigned int r) override;


};


#endif //TRIANGLEMODEL_JORDANLAKEMINENVFLOWCONTROL_H
