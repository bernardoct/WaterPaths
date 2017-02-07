//
// Created by bernardo on 2/6/17.
//

#ifndef TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
#define TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H


#include "../SystemComponents/Utility.h"

class DroughtMitigationPolicy {

public:
    const int id;

    DroughtMitigationPolicy(const int id);

    virtual void applyRestriction(int week, Utility *utility)= 0;

};


#endif //TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
