//
// Created by bernardo on 2/3/17.
//

#ifndef TRIANGLEMODEL_RESTRICTIONS_H
#define TRIANGLEMODEL_RESTRICTIONS_H


#include "../SystemComponents/Utility.h"
#include "DroughtMitigationPolicy.h"

class Restrictions : public DroughtMitigationPolicy {

private:

    const vector<double> stage_multipliers;
    const vector<double> stage_triggers;
    double current_multiplier = 0;
    Utility *utility;
public:

    Restrictions(const int id, const vector<double> &stage_multipliers,
                 const vector<double> &stage_triggers, const vector<int> utilities_id);

    Restrictions(const Restrictions &reservoir);

    virtual void applyPolicy(int week) override;

    virtual void addUtility(Utility *utility) override;

    double getCurrent_multiplier() const;
};


#endif //TRIANGLEMODEL_RESTRICTIONS_H
