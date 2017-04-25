//
// Created by bernardoct on 4/24/17.
//

#include "Insurance.h"
#include "../Utils/Utils.h"

/**
 * Insurance policy.
 * @param id Policy ID.
 * @param rof_triggers ROF triggers of all utilities using insurance. They MUST be entered sorted in ascending order
 * by utility ID. If utility does not use insurance, its trigger must be set to NONE.
 * @param insurance_premium risk premium insurance companies would charge (1.2 = 20%)
 */
Insurance::Insurance(const int id, const vector<double> &rof_triggers, const double insurance_premium)
        : DroughtMitigationPolicy(id, INSURANCE),
          rof_triggers(rof_triggers),
          insurance_premium(insurance_premium) {

    for (int i = 0; i < rof_triggers.size(); ++i) {
        if (rof_triggers[i] != NONE) {
            ids_of_utilities_with_policies.push_back(i);
        }
    }
}

Insurance::Insurance(const Insurance &insurance) : DroughtMitigationPolicy(insurance.id, INSURANCE),
                                                   insurance_premium(insurance.insurance_premium),
                                                   rof_triggers(insurance.rof_triggers) {}

void Insurance::applyPolicy(int week) {
    if (week >= WEEKS_IN_YEAR) {
        if (Utils::isFirstWeekOfTheYear(week)) {

        }
    }


}

void Insurance::addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources,
                                    const Graph *water_sources_graph) {
    for (int i : ids_of_utilities_with_policies)
        this->utilities.push_back((Utility *&&) utilities.at((unsigned long) i));
    this->water_sources = water_sources;
    this->water_sources_graph = water_sources_graph;

    for (Utility *u : this->utilities) {
        this->sources_to_utilities_ids.push_back(vector<int>());
        if (rof_triggers.at((unsigned long) u->id) != NONE) {
            for (auto ws : u->getWaterSources())
                this->sources_to_utilities_ids[sources_to_utilities_ids.size() - 1].push_back(ws.second->id);
        }
    }

    storages = std::vector<vector<double>>(water_sources.size(), vector<double>());
}

double Insurance::insurancePricing(vector<vector<double>> storage_levels) {

}

