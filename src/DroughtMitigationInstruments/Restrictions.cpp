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
    for (int i = 0; i < stage_triggers.size(); ++i) {
        if (utilities.at(0)->getRisk_of_failure() > stage_triggers[i]) {
            current_multiplier = stage_multipliers[i];
        } else break;
    }

    utilities.at(0)->setDemand_multiplier(current_multiplier);
}

double Restrictions::getCurrent_multiplier() const {
    return current_multiplier;
}

void Restrictions::addSystemComponents(vector<Utility *> systems_utilities, vector<WaterSource *> water_sources,
                                       const Graph *water_sources_graph) {
    for (Utility *u : systems_utilities) {
        if (u->id == id) {
            if (!this->utilities.empty())
                throw std::logic_error("This restriction policy already has a systems_utilities assigned to it.");
            this->utilities.push_back(u);
        }
    }
    if (systems_utilities.empty())
        throw std::invalid_argument("Restriction policy ID must match systems_utilities's ID.");
}
