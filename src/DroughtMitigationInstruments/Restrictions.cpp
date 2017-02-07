//
// Created by bernardo on 2/3/17.
//

#include "Restrictions.h"

Restrictions::Restrictions(const int id, const vector<double> &stage_multipliers,
                           const vector<double> &stage_triggers, const int utility_id)
        : DroughtMitigationPolicy(id), utility_id(utility_id),
          stage_multipliers(stage_multipliers),
          stage_triggers(stage_triggers) {}


void Restrictions::applyRestriction(int week, Utility *utility) {

    current_multiplier = 0;
    for (int i = 0; i < stage_triggers.size(); ++i) {
        if (utility->getRisk_of_failure() > stage_triggers[i]) {
            utility->setDemand(week, stage_multipliers[i]);
            current_multiplier = stage_multipliers[i];
            break;
        }
    }

    return;
}

double Restrictions::getCurrent_multiplier() const {
    return current_multiplier;
}
