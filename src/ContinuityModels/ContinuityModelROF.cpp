//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include "ContinuityModelROF.h"

ContinuityModelROF::ContinuityModelROF(const vector<WaterSource *> &water_source,
                                       const vector<vector<int>> &water_source_connectivity_matrix,
                                       const vector<Utility *> &utilities,
                                       const vector<vector<int>> &water_source_utility_connectivity_matrix,
                                       const int rof_type, const int realization_id) : ContinuityModel(water_source,
                                                                             water_source_connectivity_matrix,
                                                                             utilities,
                                                                             water_source_utility_connectivity_matrix),
                                                             rof_type(rof_type), realization_id(realization_id) {
    for (Utility *u : this->utilities) {
        u->updateTotalStoredVolume();
    }
}

/**
 * Runs one realization over total_simulation_time weeks for the input system.
 * @param total_simulation_time
 */
vector<double> ContinuityModelROF::calculateROF(int week) {

    vector<double> rofs(utilities.size(), 0);
    if (rof_type == SHORT_TERM_ROF) {
        for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
            for (int w = week; w < week + WEEKS_ROF_SHORT_TERM; ++w) {
                this->continuityStep(w, r);

                for (int i = 0; i < utilities.size(); ++i) {
                    if (utilities[i]->getStorageToCapacityRatio() <=
                        STORAGE_CAPACITY_RATIO_FAIL)
                        rofs[i] += ROF_SHORT_TERM_PRECISION;
                    cout << w << " " << utilities[i]->getStorageToCapacityRatio() << " ";
                }
                cout << endl;
            }
            this->resetUtilitiesAndReservoirs();
            cout << endl;
        }
    }
}


void ContinuityModelROF::resetUtilitiesAndReservoirs() {

    for (int i = 0; i < water_sources.size(); ++i) {
        water_sources[i]->setAvailable_volume(water_sources_realization[i]->getAvailable_volume());
        water_sources[i]->setOutflow_previous_week(water_sources_realization[i]->getOutflow_previous_week());
    }
}

void ContinuityModelROF::setWater_sources_realization(const vector<WaterSource *> &water_sources_realization) {
    ContinuityModelROF::water_sources_realization = water_sources_realization;
}
