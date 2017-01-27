//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include "ContinuityModelROF.h"

ContinuityModelROF::ContinuityModelROF(const vector<WaterSource *> &water_source,
                                       vector<vector<int>> &water_source_connectivity_matrix,
                                       const vector<Utility *> &utilities,
                                       vector<vector<int>> &water_source_utility_connectivity_matrix,
                                       const int rof_type) : ContinuityModel(water_source,
                                                                             water_source_connectivity_matrix,
                                                                             utilities,
                                                                             water_source_utility_connectivity_matrix),
                                                             rof_type(rof_type) {

    // DEEP COPY WATER SOURCES AND UTILITIES FOR VARIABLES THAT MATTER (STORED VOLUMES).
}


/**
 * Runs one realization over total_simulation_time weeks for the input system.
 * @param total_simulation_time
 */
void ContinuityModelROF::calculateROF() {

    if (rof_type == SHORT_TERM_ROF) {

        for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
            for (Utility *u : utilities) {
                u->updateTotalStoredVolume();
            }

            for (int week = 0; week < WEEKS_ROF_SHORT_TERM; ++week) {
                this->continuityStep(week, r);
            }
        }
    }
}