//
// Created by bernardo on 2/6/17.
//

#ifndef TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
#define TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H


#include "../SystemComponents/Utility.h"
#include "../Utils/Constants.h"

class DroughtMitigationPolicy {
protected:
    vector<int> utilities_ids;

public:
    const int id;
    const int type;

    DroughtMitigationPolicy(const int id, const int type);

    virtual void applyPolicy(int week)= 0;

    virtual void addUtility(Utility *utility)= 0;

    const vector<int> &getUtilities_ids() const;

};


#endif //TRIANGLEMODEL_DROUGHTMITIGATIONPOLICY_H
