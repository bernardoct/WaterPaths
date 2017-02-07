//
// Created by bernardo on 2/3/17.
//

#ifndef TRIANGLEMODEL_RESTRICTIONS_H
#define TRIANGLEMODEL_RESTRICTIONS_H


#include "../SystemComponents/Utility.h"
#include "DroughtMitigationPolicy.h"

class Restrictions : public DroughtMitigationPolicy {

    const vector<double> stage_multipliers;
    const vector<double> stage_triggers;
    double current_multiplier = 0;

public:
    const int utility_id;

    Restrictions(const int id, const vector<double> &stage_multipliers,
                 const vector<double> &stage_triggers, const int utility_id);

    virtual void applyRestriction(int week, Utility *utility) override;

    double getCurrent_multiplier() const;
};


#endif //TRIANGLEMODEL_RESTRICTIONS_H
