//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_MINENVIRONFLOWCONTROL_H
#define TRIANGLEMODEL_MINENVIRONFLOWCONTROL_H


#include "../../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../../SystemComponents/Utility.h"

class MinEnvFlowControl {
protected:
    vector<WaterSource *> water_sources;
    vector<Catchment *> catchments;
    vector<Utility* > utilities;

public:
    const vector<int> water_sources_ids;
    const vector<int> utilities_ids;
    const int water_source_id;
    const int type;

    MinEnvFlowControl(int water_source_id,
                              const vector<int> &aux_water_sources_id,
                              const vector<int> &aux_utilities_ids, int type);

    MinEnvFlowControl(const MinEnvFlowControl &min_env_control);

    virtual ~MinEnvFlowControl();

    virtual double getRelease(int week) = 0;

    void addComponents(
            vector<WaterSource *> water_sources, vector<Utility *> utilities);

    virtual void setRealization(unsigned long r, vector<double> &rdm_factors);
};


#endif //TRIANGLEMODEL_MINENVIRONFLOWCONTROL_H
