//
// Created by bct52 on 6/28/17.
//

#ifndef TRIANGLEMODEL_MINENVIRONFLOWCONTROL_H
#define TRIANGLEMODEL_MINENVIRONFLOWCONTROL_H


#include "../../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../../SystemComponents/Utility.h"

class MinEnvironFlowControl {
protected:
    vector<WaterSource *> water_sources;
    vector<Catchment *> catchments;
    vector<Utility* > utilities;

public:
    const vector<int> water_sources_ids;
    const vector<int> catchments_ids;
    const vector<int> utilities_ids;

    MinEnvironFlowControl(const vector<int> &water_sources_ids, const vector<int> &catchments_ids,
                          const vector<int> &utilities_ids);

    virtual double getRelease(int week) = 0;

    void addComponents(vector<WaterSource *> water_sources, vector<Utility *> utilities, vector<Catchment *>
                        catchments);
};


#endif //TRIANGLEMODEL_MINENVIRONFLOWCONTROL_H
