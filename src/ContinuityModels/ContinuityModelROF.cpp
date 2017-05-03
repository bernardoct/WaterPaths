//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include <algorithm>
#include "ContinuityModelROF.h"
#include "../Utils/Utils.h"

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


    storage_to_rof_realization = Matrix3D<double>((unsigned int) continuity_utilities.size(),
                                                  (unsigned int) NO_OF_INSURANCE_STORAGE_TIERS,
                                                  (unsigned int) WEEKS_IN_YEAR);
    storage_to_rof_table = Matrix3D<double>((unsigned int) continuity_utilities.size(),
                                            (unsigned int) NO_OF_INSURANCE_STORAGE_TIERS,
                                            (unsigned int) WEEKS_IN_YEAR);

}

ContinuityModelROF::ContinuityModelROF(ContinuityModelROF &continuity_model_rof)
        : ContinuityModel(continuity_model_rof.continuity_water_sources,
                          continuity_model_rof.continuity_utilities,
                          continuity_model_rof.water_sources_graph,
                          continuity_model_rof.water_sources_to_utilities),
          realization_id(continuity_model_rof.realization_id) {
}

ContinuityModelROF::~ContinuityModelROF() {
}

/**
 * Runs one the full rof calculations for realization #realization_id for a given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateROF(int week, int rof_type) {

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure(continuity_utilities.size(), 0.0);
    vector<double> year_failure(continuity_utilities.size(), 0.0);
    /// If this is the first week of the year, reset storage-rof table.
    if (Utils::isFirstWeekOfTheYear(week))
        storage_to_rof_table.reset(false);

    unsigned int week_of_the_year = (unsigned int) (((double) week / WEEKS_IN_YEAR - week / WEEKS_IN_YEAR) *
                                                    WEEKS_IN_YEAR);

    /// checks if new infrastructure became available and, if so, set the corresponding realization
    /// infrastructure online.
    updateOnlineInfrastructure(week);

    int n_weeks_rof;
    /// short-term rof calculations.
    if (rof_type == SHORT_TERM_ROF)
        n_weeks_rof = WEEKS_ROF_SHORT_TERM;
    /// long-term rof calculations.
    else
        n_weeks_rof = WEEKS_ROF_LONG_TERM;

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        beginning_res_level = NO_OF_INSURANCE_STORAGE_TIERS;
        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(rof_type);

//        storage_to_rof_realization.reset(NON_FAILURE);

        for (int w = week; w < week + n_weeks_rof; ++w) {

            /// one week continuity time-step.
            continuityStep(w, r);

            /// check total available storage for each utility and, if smaller than the fail ration,
            /// increase the number of failed years of that utility by 1 (FAILURE).
            for (int u = 0; u < continuity_utilities.size(); ++u)
                if (continuity_utilities[u]->getStorageToCapacityRatio() <= STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[u] = FAILURE;

            /// calculated week of storage-rof table
//            if (rof_type == SHORT_TERM_ROF)
//                updateStorageToROFTable(INSURANCE_SHIFT_STORAGE_CURVES_THRESHOLD, week_of_the_year);

        }

        /// update storage-rof table
//        storage_to_rof_table += storage_to_rof_realization / NUMBER_REALIZATIONS_ROF;

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

//    storage_to_rof_table.print(0);
//    cout << endl;
    
    return risk_of_failure;
}

void ContinuityModelROF::updateStorageToROFTable(double storage_percent_decrement, int week_of_the_year) {

    int n_water_sources = (int) continuity_water_sources.size();
    int n_utilities = (int) continuity_utilities.size();

    double available_volumes[continuity_water_sources.size()];
    for (int ws = 0; ws < continuity_water_sources.size(); ++ws)
        available_volumes[ws] = continuity_water_sources[ws]->getAvailable_volume();

    /// loops over the percent storage levels to populate table.
//    for (int i = NO_OF_INSURANCE_STORAGE_TIERS; i >= 0; --i) {
    for (int i = beginning_res_level; i >= 0; --i) {
        double percent_percent_storage_level = (double) i * storage_percent_decrement;
        double delta_storage[n_water_sources] = {};
        std::copy(available_volumes, available_volumes + n_water_sources, delta_storage);
        double available_volumes_shifted[n_water_sources] = {};
        std::copy(available_volumes, available_volumes + n_water_sources, available_volumes_shifted);

        /// calculate the difference between the simulated available water and the one for the table calculated above
        /// based on the percent decrement.
        for (int k = 0; k < n_water_sources; ++k) delta_storage[k] =
                                                          water_sources_capacities[k] * percent_percent_storage_level -
                                                          available_volumes[k];

        /// Shift storages.
        shiftStorages(available_volumes_shifted, delta_storage);

        /// Checks for utilities failures.
        int count_fails = 0;
        for (unsigned int u = 0; u < water_sources_to_utilities.size(); ++u) {
            double utility_storage = 0;
            /// Calculate combined stored volume for each utility based on shifted storages.
            for (int ws : water_sources_to_utilities[u])
                utility_storage += available_volumes_shifted[ws];
            /// Account for utilities failing and update table.
            if (utility_storage / utilities_capacities[u] < STORAGE_CAPACITY_RATIO_FAIL) {
                storage_to_rof_realization(u, i, week_of_the_year) = FAILURE;
                count_fails++;
            }
        }

        /// If all utilities have failed, stop dropping level.
        if (count_fails == n_utilities) {
            for (int j = i; j >= 0; --j) {
                for (unsigned int u = 0; u < water_sources_to_utilities.size(); ++u) {
                    /// Calculate combined stored volume for each utility based on shifted storages.
                    storage_to_rof_realization(u, j, week_of_the_year) = FAILURE;

                }
            }
            break;
        }

        if (count_fails == 0)
            beginning_res_level = i + 2;
        else if (beginning_res_level < i + 2)
            beginning_res_level = i + 2;
//        cout << count_fails << " ";
    }
//    cout << endl << beginning_res_level << endl;
//    storage_to_rof_realization.print(week_of_the_year);
//    cout << endl;
}


void ContinuityModelROF::shiftStorages(double *available_volumes_shifted, double *delta_storage) {

    /// Add deltas to all sources following the topological order, so that upstream is calculated before downstream.
    for (int ws : sources_topological_order) {
        bool no_downstream = downstream_sources[ws] != NON_INITIALIZED;
        /// calculate initial estimate for shifted
        available_volumes_shifted[ws] += delta_storage[ws];

        /// if not full, retrieve spill to downstream source.
        if (available_volumes_shifted[ws] < water_sources_capacities[ws]) {
            double excess_spilled = continuity_water_sources[ws]->getTotal_outflow() -
                                    continuity_water_sources[ws]->min_environmental_outflow;
            available_volumes_shifted[ws] += excess_spilled;
            if (no_downstream)
                available_volumes_shifted[downstream_sources[ws]] -= excess_spilled;
        }

        /// if after retrieving spill the source is full, send excess downstream. Works for intakes as well, since
        /// their capacities are 0, meaning all excess from above overflows.
        if (available_volumes_shifted[ws] > water_sources_capacities[ws]) {
            if (no_downstream) {
                available_volumes_shifted[downstream_sources[ws]] += available_volumes_shifted[ws] -
                                                                     water_sources_capacities[ws];
            }
            available_volumes_shifted[ws] = water_sources_capacities[ws];
            /// prevent negative storages.
        } else
            available_volumes_shifted[ws] = max(available_volumes_shifted[ws], 0.);
    }
}


/*
vector<vector<double>> ContinuityModelROF::shiftStorageTimeSeries(vector<vector<double>> storage_series,
                                                                  vector<double> water_sources_capacities,
                                                                  vector<int> downstream_sources,
                                                                  double capacity_percentile,
                                                                  int first_week,
                                                                  int last_week) {
    unsigned long n_sources = storage_series.size();
    vector<double> delta_storage(n_sources, 0.);

    for (int ws = 0; ws < n_sources; ++ws) {
        delta_storage[ws] = water_sources_capacities[ws] * capacity_percentile - storage_series[ws][0];
    }

    for (int w = first_week; w < last_week; ++w) {
        for (int ws = 0; ws < n_sources; ++ws) {
            double capacity = water_sources_capacities[ws];
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
void ContinuityModelROF::updateOnlineInfrastructure(int week) {
    for (int i = 0; i < realization_water_sources.size(); ++i) {
        if (realization_water_sources[i]->isOnline() && !continuity_water_sources[i]->isOnline()) {
            continuity_water_sources[i]->setOnline();
            water_sources_capacities[i] = continuity_water_sources[i]->capacity;
        }
    }

    if (Utils::isFirstWeekOfTheYear(week) || week == 0)
        for (int u = 0; u < continuity_utilities.size(); ++u) {
            utilities_capacities[u] = continuity_utilities[u]->getTotal_storage_capacity();
        }
}