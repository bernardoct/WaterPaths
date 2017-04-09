//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include "ContinuityModelROF.h"

ContinuityModelROF::ContinuityModelROF(const vector<WaterSource *> &water_source,
                                       const WaterSourceGraph &water_sources_graph,
                                       const vector<vector<int>> &water_sources_to_utilities,
                                       const vector<Utility *> &utilities,
                                       const int realization_id) : ContinuityModel(water_source,
                                                                                   utilities,
                                                                                   water_sources_graph,
                                                                                   water_sources_to_utilities),
                                                                   realization_id(realization_id) {

    /// update utilities' total stored volume
    for (Utility *u : this->utilities) {
        u->updateTotalStoredVolume();
    }
}

/**
 * Runs one the full rof calculations for realization #realization_id for a given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateROF(int week, int rof_type) {

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure(utilities.size(), 0.0);
    vector<double> year_failure(utilities.size(), 0.0);

    int n_weeks_rof;
    /// short-term rof calculations.
    if (rof_type == SHORT_TERM_ROF)
        n_weeks_rof = WEEKS_ROF_SHORT_TERM;
    /// long-term rof calculations.
    else
        n_weeks_rof = WEEKS_ROF_LONG_TERM;

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        for (int w = week; w < week + n_weeks_rof; ++w) {

            /// one week continuity time-step.
            this->continuityStep(w, r);

            /// check total available storage for each utility and, if smaller than the fail ration,
            /// increase the number of failed years of that utility by 1 (FAILURE).
            for (int i = 0; i < utilities.size(); ++i) {
                if (utilities[i]->getStorageToCapacityRatio() <= STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[i] = FAILURE;
            }
        }

        for (int j = 0; j < utilities.size(); ++j) {
            risk_of_failure[j] += year_failure[j];
            year_failure[j] = NON_FAILURE;
        }

        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        this->resetUtilitiesAndReservoirs();
    }

    /// Finish ROF calculations
    for (int i = 0; i < utilities.size(); ++i) {
        risk_of_failure[i] /= NUMBER_REALIZATIONS_ROF;
    }

    return risk_of_failure;
}

/**
 * reset reservoirs' and utilities' storage and last release, and combined storage, respectively, they
 * currently have in the corresponding realization simulation.
 */
void ContinuityModelROF::resetUtilitiesAndReservoirs() {

    /// update water sources info.
    for (int i = 0; i < water_sources.size(); ++i) {
        water_sources[i]->setAvailable_volume(water_sources_realization[i]->getAvailable_volume());
        water_sources[i]->setOutflow_previous_week(water_sources_realization[i]->getTotal_outflow());
    }

    /// update utilities combined storage.
    for (Utility *u : this->utilities) {
        u->updateTotalStoredVolume();
    }
}

/**
 * Pass to the rof continuity model the locations of the utilities of the realization it calculated rofs
 * for. This is done so that the rof calculations can reset the reservoir and utilities states for each of
 * the rofs' year realizations.
 * @param water_sources_realization
 */
void ContinuityModelROF::setWater_sources_realization(const vector<WaterSource *> &water_sources_realization) {
    ContinuityModelROF::water_sources_realization = water_sources_realization;
}
