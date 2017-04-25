//
// Created by bernardoct on 4/24/17.
//

#include "Insurance.h"

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
        if (rof_triggers[i] > NONE) {
            ids_of_utilities_with_policies.push_back(i);
        }
    }
}

// ================================= SET UP COPY CONSTRUCTOR ====================================

void Insurance::applyPolicy(int week) {

}

void Insurance::addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources,
                                    const Graph *water_sources_graph) {
    for (int i : ids_of_utilities_with_policies)
        this->utilities.push_back((Utility *&&) utilities.at((unsigned long) i));
    this->water_sources = water_sources;
    this->water_sources_graph = water_sources_graph;
}
