//
// Created by bernardo on 2/6/17.
//

#ifndef TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
#define TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H


#include "../SystemComponents/Utility.h"

class DroughtMitigationPolicy {

public:
    const vector<int> utilities_ids;
    const int id;

    DroughtMitigationPolicy(const int id, vector<int> utilities_id);

    virtual void applyPolicy(int week)= 0;

    virtual void addUtility(Utility *utility) = 0;

};


#endif //TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
