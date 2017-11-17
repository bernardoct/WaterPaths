//
// Created by bct52 on 7/3/17.
//

#ifndef TRIANGLEMODEL_JORDANLAKEMINENVFLOWCONTROL_H
#define TRIANGLEMODEL_JORDANLAKEMINENVFLOWCONTROL_H


#include "../StorageMinEnvFlowControl.h"

class JordanLakeMinEnvFlowControl : public MinEnvironFlowControl {
private:
    Catchment* lillington_gage_catchment;
    float rel02, rel04, rel06_up, min_rel_llt06, min_rel_llt08,
            min_rel_llt08_up, quality_capacity, supply_capacity;

public:
    JordanLakeMinEnvFlowControl(
                int jordan_lake_id, Catchment *lillington_gage_catchment,
                float rel02, float rel04, float rel06_up, float min_rel_llt06,
                float min_rel_llt08, float min_rel_llt08_up,
                float quality_capacity, float supply_capacity);

    JordanLakeMinEnvFlowControl(
            const JordanLakeMinEnvFlowControl &jl_min_flow_control);

    JordanLakeMinEnvFlowControl &operator=
            (const JordanLakeMinEnvFlowControl jl_min_flow_control);

    float getRelease(int week) override;

    void setRealization(unsigned int r) override;


};


#endif //TRIANGLEMODEL_JORDANLAKEMINENVFLOWCONTROL_H
