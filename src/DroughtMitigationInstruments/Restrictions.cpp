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
    utilities_ids = vector<int>(1, id);
}

Restrictions::Restrictions(const Restrictions &restrictions) : DroughtMitigationPolicy(restrictions),
                                                               stage_multipliers(restrictions.stage_multipliers),
                                                               stage_triggers(restrictions.stage_triggers) {
    utilities_ids = restrictions.utilities_ids;
}

Restrictions::~Restrictions() {}

void Restrictions::applyPolicy(int week) {

    current_multiplier = 1.0;
    /// Loop through restriction stage rof triggers to see which stage should be applied, if any.
    for (int i = 0; i < stage_triggers.size(); ++i) {
        if (realization_utilities.at(0)->getRisk_of_failure() > stage_triggers[i]) {
            /// Demand multiplier to be applied, based on the rofs.
            current_multiplier = stage_multipliers[i];
        } else break;
    }

    /// Apply demand multiplier
    realization_utilities.at(0)->setDemand_multiplier(current_multiplier);
}

double Restrictions::getCurrent_multiplier() const {
    return current_multiplier;
}

void Restrictions::addSystemComponents(vector<Utility *> systems_utilities, vector<WaterSource *> water_sources) {
    /// Get utility whose IDs correspond to restriction policy ID.
    for (Utility *u : systems_utilities) {
        if (u->id == id) {
            if (!realization_utilities.empty())
                throw std::logic_error("This restriction policy already has a systems_utilities assigned to it.");
            /// Link utility to policy.
            this->realization_utilities.push_back(u);
        }
    }
    if (systems_utilities.empty())
        throw std::invalid_argument("Restriction policy ID must match systems_utilities's ID.");
}
