//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include <algorithm>
#include "ContinuityModelROF.h"

ContinuityModelROF::ContinuityModelROF(const vector<WaterSource *> &water_sources,
                                       const Graph &water_sources_graph,
                                       const vector<vector<int>> &water_sources_to_utilities,
                                       const vector<Utility *> &utilities,
                                       const int realization_id) : ContinuityModel(water_sources,
                                                                                   utilities,
                                                                                   water_sources_graph,
                                                                                   water_sources_to_utilities),
                                                                   realization_id(realization_id) {

    /// update utilities' total stored volume
    for (Utility *u : this->continuity_utilities) {
        u->updateTotalStoredVolume();
    }

    /// the variables below are to make the storage-ROF table calculation faster.
    for (int ws = 0; ws < water_sources.size(); ++ws) {
        capacities.push_back(water_sources[ws]->capacity);
    }

    for (vector<int> ds : water_sources_graph.getDownSources())
        if (ds.empty())
            downstream_sources.push_back(NON_INITIALIZED);
        else
            downstream_sources.push_back(ds[0]);

    sources_topological_order = water_sources_graph.getTopological_order();

    storage_to_rof_table = new vector<vector<double>>();

}

ContinuityModelROF::ContinuityModelROF(ContinuityModelROF &continuity_model_rof)
        : ContinuityModel(continuity_model_rof.continuity_water_sources,
                          continuity_model_rof.continuity_utilities,
                          continuity_model_rof.water_sources_graph,
                          continuity_model_rof.water_sources_to_utilities),
          realization_id(continuity_model_rof.realization_id) {
}

ContinuityModelROF::~ContinuityModelROF() {
    delete storage_to_rof_table;
}

/**
 * Runs one the full rof calculations for realization #realization_id for a given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateROF(int week, int rof_type) {

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure(continuity_utilities.size(), 0.0);
    vector<double> year_failure(continuity_utilities.size(), 0.0);

    /// checks if new infrastructure became available and, if so, set the corresponding realization infrastructure online.
    updateOnlineInfrastructure();

    int n_weeks_rof;
    /// short-term rof calculations.
    if (rof_type == SHORT_TERM_ROF)
        n_weeks_rof = WEEKS_ROF_SHORT_TERM;
    /// long-term rof calculations.
    else
        n_weeks_rof = WEEKS_ROF_LONG_TERM;

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {

        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(rof_type);

        for (int w = week; w < week + n_weeks_rof; ++w) {

            /// one week continuity time-step.
            continuityStep(w, r);

            /// check total available storage for each utility and, if smaller than the fail ration,
            /// increase the number of failed years of that utility by 1 (FAILURE).
            for (int u = 0; u < continuity_utilities.size(); ++u)
                if (continuity_utilities[u]->getStorageToCapacityRatio() <= STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[u] = FAILURE;
        }

        /// finalize ROF calculation and reset failure counter.
        for (int j = 0; j < continuity_utilities.size(); ++j) {
            risk_of_failure[j] += year_failure[j];
            year_failure[j] = NON_FAILURE;
        }
    }

    /// Finish ROF calculations
    for (int i = 0; i < continuity_utilities.size(); ++i) {
        risk_of_failure[i] /= NUMBER_REALIZATIONS_ROF;
    }

    return risk_of_failure;
}

void ContinuityModelROF::updateStorageToROFTable(double storage_percent_increment, vector<double> available_volumes) {

    for (double sl = 1.0; sl > 0; sl -= storage_percent_increment) {
        vector<double> reduced_capacities;
        vector<double> delta_storage = available_volumes;

        transform(capacities.begin(), capacities.end(), reduced_capacities.begin(),
                  bind1st(multiplies<double>(), sl));
        transform(reduced_capacities.begin(), reduced_capacities.end(), available_volumes.begin(),
                  back_inserter(delta_storage), minus<double>());

        for (int ws : sources_topological_order) {
            available_volumes[ws] += delta_storage[ws];

            /// if source overflows, send excess downstream. Works for intakes as well, since their capacities are 0,
            /// meaning all excess from above overflows.
            if (available_volumes[ws] > capacities[ws]) {
                if (downstream_sources[ws] != NON_INITIALIZED)
                    available_volumes[downstream_sources[ws]] += available_volumes[ws] - capacities[ws];
                available_volumes[ws] = capacities[ws];
                /// prevent negative storages.
            } else if (available_volumes[ws] < 0)
                available_volumes[ws] = 0;
        }
    }
}



/*
vector<vector<double>> ContinuityModelROF::shiftStorageTimeSeries(vector<vector<double>> storage_series,
                                                                  vector<double> capacities,
                                                                  vector<int> downstream_sources,
                                                                  double capacity_percentile,
                                                                  int first_week,
                                                                  int last_week) {
    unsigned long n_sources = storage_series.size();
    vector<double> delta_storage(n_sources, 0.);

    for (int ws = 0; ws < n_sources; ++ws) {
        delta_storage[ws] = capacities[ws] * capacity_percentile - storage_series[ws][0];
    }

    for (int w = first_week; w < last_week; ++w) {
        for (int ws = 0; ws < n_sources; ++ws) {
            double capacity = capacities[ws];
            storage_series[ws][w] = max(0.0, storage_series[ws][w] + delta_storage[ws]);

            if (storage_series[ws][w] > capacity) {
                if (downstream_sources[ws] > NON_INITIALIZED) storage_series[downstream_sources[ws]][w] +=
                                                                      storage_series[ws][w] - capacity;
                storage_series[ws][w] = capacity;
            }
        }
    }

    return storage_series;
}
*/
/**
 * reset reservoirs' and utilities' storage and last release, and combined storage, respectively, they
 * currently have in the corresponding realization simulation.
 */
void ContinuityModelROF::resetUtilitiesAndReservoirs(int rof_type) {

    /// update water sources info. If short-term rof, return to current storage; if long-term, make them full.
    if (rof_type == SHORT_TERM_ROF)
        for (int i = 0; i < continuity_water_sources.size(); ++i) {
            continuity_water_sources[i]->setAvailable_volume(realization_water_sources[i]->getAvailable_volume());
            continuity_water_sources[i]->setOutflow_previous_week(realization_water_sources[i]->getTotal_outflow());
        }
    else
        for (int i = 0; i < continuity_water_sources.size(); ++i) {
            continuity_water_sources[i]->setAvailable_volume(realization_water_sources[i]->capacity);
            continuity_water_sources[i]->setOutflow_previous_week(realization_water_sources[i]->getTotal_outflow());
        }

    /// update utilities combined storage.
    for (Utility *u : this->continuity_utilities) {
        u->updateTotalStoredVolume();
    }
}

/**
 * Pass to the rof continuity model the locations of the utilities of the realization it calculated rofs
 * for. This is done so that the rof calculations can reset the reservoir and utilities states for each of
 * the rofs' year realizations.
 * @param water_sources_realization
 */
void ContinuityModelROF::setRealization_water_sources(const vector<WaterSource *> &water_sources_realization) {
    ContinuityModelROF::realization_water_sources = water_sources_realization;
}

/**
 * Checks if new infrastructure became online.
 */
void ContinuityModelROF::updateOnlineInfrastructure() {
    for (int i = 0; i < realization_water_sources.size(); ++i) {
        if (realization_water_sources[i]->isOnline() && !continuity_water_sources[i]->isOnline())
            continuity_water_sources[i]->setOnline();
    }
}