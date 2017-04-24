//
// Created by bernardo on 2/3/17.
//

#include <iostream>
#include "Restrictions.h"

/**
 * Restriction policy.
 * @param id
 * @param stage_multipliers
 * @param stage_triggers
 * @todo set lower ROF threshold for utilities to lift restrictions.
 * @todo implement drought surcharges.
 */
Restrictions::Restrictions(const int id, const vector<double> &stage_multipliers,
                           const vector<double> &stage_triggers)
        : DroughtMitigationPolicy(id, RESTRICTIONS),
          stage_multipliers(stage_multipliers),
          stage_triggers(stage_triggers) {
    this->utilities_ids = vector<int>(1, id);
}

Restrictions::Restrictions(const Restrictions &restrictions) : DroughtMitigationPolicy(restrictions.id, RESTRICTIONS),
                                                               stage_multipliers(restrictions.stage_multipliers),
                                                               stage_triggers(restrictions.stage_triggers),
                                                               utility(restrictions.utility) {
    this->utilities_ids = restrictions.utilities_ids;
}

Restrictions::~Restrictions() {}

bool Restrictions::operator<(const Restrictions other) {
    return this->utilities_ids[0] < other.utilities_ids[0];
}

bool Restrictions::operator>(const Restrictions other) {
    return this->utilities_ids[0] > other.utilities_ids[0];
}

void Restrictions::applyPolicy(int week) {

    current_multiplier = 1.0;
    for (int i = 0; i < stage_triggers.size(); ++i) {
        if (utility->getRisk_of_failure() > stage_triggers[i]) {
            current_multiplier = stage_multipliers[i];
        } else break;
    }

    utility->setDemand_multiplier(current_multiplier);
}

double Restrictions::getCurrent_multiplier() const {
    return current_multiplier;
}

void Restrictions::addUtility(Utility *utility) {
    if (utility->id != id) throw std::invalid_argument("Restriction policy ID must match utility's ID.");
    this->utility = utility;
}
