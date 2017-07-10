//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include <algorithm>
#include "ContinuityModelROF.h"
#include "../Utils/Utils.h"

ContinuityModelROF::ContinuityModelROF(
        const vector<WaterSource *> &water_sources,
        const Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        const vector<Utility *> &utilities,
        vector<MinEnvironFlowControl *> &min_env_flow_controls,
        const unsigned int realization_id) : ContinuityModel(water_sources,
                                                             utilities,
                                                             min_env_flow_controls,
                                                             water_sources_graph,
                                                             water_sources_to_utilities,
                                                             realization_id) {

    /// update utilities' total stored volume
    for (Utility *u : this->continuity_utilities) {
        u->updateTotalStoredVolume();
    }

    storage_to_rof_realization = Matrix3D<double>(
            (unsigned int) continuity_utilities.size(),
            (unsigned int) NO_OF_INSURANCE_STORAGE_TIERS,
            (unsigned int) ceil(WEEKS_IN_YEAR));
    storage_to_rof_table = new Matrix3D<double>(
            (unsigned int) continuity_utilities.size(),
            (unsigned int) NO_OF_INSURANCE_STORAGE_TIERS,
            (unsigned int) ceil(WEEKS_IN_YEAR));

    /// Record which sources have no downstream sources.
    storage_wout_downstream = new bool[sources_topological_order.size()];
    for (int ws : sources_topological_order)
        storage_wout_downstream[ws] = downstream_sources[ws] != NON_INITIALIZED;
}

ContinuityModelROF::ContinuityModelROF(ContinuityModelROF &continuity_model_rof)
        : ContinuityModel(continuity_model_rof.continuity_water_sources,
                          continuity_model_rof.continuity_utilities,
                          continuity_model_rof.min_env_flow_controls,
                          continuity_model_rof.water_sources_graph,
                          continuity_model_rof.water_sources_to_utilities,
                          continuity_model_rof.realization_id) {
}

ContinuityModelROF::~ContinuityModelROF() {
}

/**
 * Runs one the full rof calculations for realization #realization_id for a given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateROF(int week, int rof_type) {

    unsigned long n_utilities = continuity_utilities.size();

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure(n_utilities, 0.0);
    vector<double> year_failure(n_utilities, 0.0);

    /// If this is the first week of the year, reset storage-rof table.
    if (rof_type == LONG_TERM_ROF)
        storage_to_rof_table->reset(NON_FAILURE);

    int week_of_the_year = Utils::weekOfTheYear(week);

    /// checks if new infrastructure became available and, if so, set the corresponding realization
    /// infrastructure online.
    updateOnlineInfrastructure(week);

    int n_weeks_rof;
    /// short-term rof calculations.
    if (rof_type == SHORT_TERM_ROF)
        n_weeks_rof = WEEKS_ROF_SHORT_TERM;
    /// long-term rof calculations.
    else {
        n_weeks_rof = WEEKS_ROF_LONG_TERM;
    }

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        beginning_res_level = NO_OF_INSURANCE_STORAGE_TIERS;
        /// reset current reservoirs' and utilities' storage and combined storage, respectively,
        /// in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(rof_type);

        for (int w = week; w < week + n_weeks_rof; ++w) {

            /// one week continuity time-step.
            continuityStep(w, r);

            /// check total available storage for each utility and, if smaller than the fail ration,
            /// increase the number of failed years of that utility by 1 (FAILURE).
            for (int u = 0; u < n_utilities; ++u)
                if (continuity_utilities[u]->getStorageToCapacityRatio() <=
                    STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[u] = FAILURE;

            /// calculated week of storage-rof table
            if (rof_type == SHORT_TERM_ROF)
                updateStorageToROFTable(
                        INSURANCE_SHIFT_STORAGE_CURVES_THRESHOLD,
                        week_of_the_year);
        }

        //FIXME: INSTEAD OF ADDING THE 3D MATRIX EVERY WEEK (WHICH MEANS ADDING THE WHOLE YEAR TABLE EVERY WEEK),
        //FIXME: REPLACE THIS BY A MATRIX 2D SUMMATION FOR THE SAKE OF PERFORMANCE.
        /// update storage-rof table
        if (rof_type == SHORT_TERM_ROF) {
            *storage_to_rof_table += storage_to_rof_realization /
                                     NUMBER_REALIZATIONS_ROF;
        }

        /// Count failures and reset failures counter.
        for (int uu = 0; uu < n_utilities; ++uu) {
            risk_of_failure[uu] += year_failure[uu];
            year_failure[uu] = NON_FAILURE;
        }
    }

    /// Finish ROF calculations
    for (int i = 0; i < n_utilities; ++i) {
        risk_of_failure[i] /= NUMBER_REALIZATIONS_ROF;
    }

    return risk_of_failure;
}

void ContinuityModelROF::updateStorageToROFTable(
        double storage_percent_decrement, int week_of_the_year) {

    int n_water_sources = (int) continuity_water_sources.size();
    int n_utilities = (int) continuity_utilities.size();

    double available_volumes[n_water_sources];
    for (int ws = 0; ws < n_water_sources; ++ws)
        available_volumes[ws] =
                continuity_water_sources[ws]->getAvailable_volume();

    /// loops over the percent storage levels to populate table. The loop begins from two levels above the level
    /// where at least one failure was observed in the last iteration. This saves a lot of computational time.
    for (int s = beginning_res_level; s >= 0; --s) {
        /// calculate delta storage for all reservoirs and array that will receive the shifted storage curves.
        double percent_percent_storage_level = (double) s *
                                               storage_percent_decrement;
        double delta_storage[n_water_sources];
        std::copy(available_volumes,
                  available_volumes + n_water_sources,
                  delta_storage);
        double available_volumes_shifted[n_water_sources];
        std::copy(available_volumes,
                  available_volumes + n_water_sources,
                  available_volumes_shifted);

        /// calculate the difference between the simulated available water and the one for the table calculated above
        /// based on the percent decrement.
        for (int k = 0; k < n_water_sources; ++k)
            delta_storage[k] = water_sources_capacities[k] *
                               percent_percent_storage_level -
                               available_volumes[k];

        /// Shift storages.
        shiftStorages(available_volumes_shifted, delta_storage);

        /// Checks for utilities failures.
        int count_fails = 0;
        for (unsigned int u = 0; u < n_utilities; ++u) {
            double utility_storage = 0;
            /// Calculate combined stored volume for each utility based on shifted storages.
            for (int ws : water_sources_to_utilities[u])
                utility_storage += available_volumes_shifted[ws];
            /// Register failure in the table for each utility meeting failure criteria.
            if (utility_storage / utilities_capacities[u] <
                STORAGE_CAPACITY_RATIO_FAIL) {
                storage_to_rof_realization(u, s, week_of_the_year) = FAILURE;
                count_fails++;
            }
        }

        /// If all utilities have failed, stop dropping level.
        if (count_fails == n_utilities) {
            for (int ss = s; ss >= 0; --ss) {
                for (unsigned int u = 0; u < n_utilities; ++u) {
                    /// Calculate combined stored volume for each utility based on shifted storages.
                    storage_to_rof_realization(u,
                                               ss,
                                               week_of_the_year) =
                            FAILURE;
                }
            }
            break;
        }

        /// Sets the beginning percent storage for the next realization two levels above the first level in which
        /// at least one failure was observed.
        if (count_fails == 0)
            beginning_res_level = s + 2;
        else if (beginning_res_level < s + 2)
            beginning_res_level = s + 2;
    }
}

//FIXME: MAKE THIS MORE EFFICIENT. THIS METHOD IS THE MOST EXPENSIVE ONE IN THE CODE.
void ContinuityModelROF::shiftStorages(
        double *available_volumes_shifted,
        double *delta_storage) {

    /// Add deltas to all sources following the topological order, so that
    /// upstream is calculated before downstream.
    for (int ws : sources_topological_order) {
//        bool no_downstream = downstream_sources[ws] != NON_INITIALIZED;

        /// calculate initial estimate for shifted
        available_volumes_shifted[ws] += delta_storage[ws];

        /// if not full, retrieve spill to downstream source.
        if (available_volumes_shifted[ws] < water_sources_capacities[ws]) {
            /// Calculate spilled water. Since the curves are shifted as the
            /// weeks of the rof realizations are calculated, the minimum
            /// environmental outflows below will be the ones pertinent to
            /// the time in which the storage is being shifted.
            double excess_spilled =
                    continuity_water_sources[ws]->getTotal_outflow() -
                    continuity_water_sources[ws]
                            ->getMin_environmental_outflow();

            available_volumes_shifted[ws] += excess_spilled;
            //FIXME: CHECK IF THIS SUBSTITUTION RETURNS THE SAME RESULTS AS COMMENTED LINES
//            if (no_downstream)
            if (storage_wout_downstream[ws])
                available_volumes_shifted[downstream_sources[ws]] -=
                        excess_spilled;
        }

        /// if after retrieving spill the source is full, send excess
        /// downstream. Works for intakes as well, since their capacities are
        /// 0, meaning all excess from above overflows.
        if (available_volumes_shifted[ws] > water_sources_capacities[ws]) {
            //FIXME: CHECK IF THIS SUBSTITUTION RETURNS THE SAME RESULTS AS COMMENTED LINES
//            if (no_downstream)
            if (storage_wout_downstream[ws])
                available_volumes_shifted[downstream_sources[ws]] +=
                        available_volumes_shifted[ws] -
                        water_sources_capacities[ws];

            available_volumes_shifted[ws] = water_sources_capacities[ws];
            /// prevent negative storages.
        } else
            available_volumes_shifted[ws] = max(available_volumes_shifted[ws],
                                                0.);
    }
}

/**
 * reset reservoirs' and utilities' storage and last release, and combined storage, respectively, they
 * currently have in the corresponding realization simulation.
 */
void ContinuityModelROF::resetUtilitiesAndReservoirs(int rof_type) {

    /// update water sources info. If short-term rof, return to current storage; if long-term, make them full.
    if (rof_type == SHORT_TERM_ROF)
        for (int i = 0; i < continuity_water_sources.size();
             ++i) {   // Current available volume
            continuity_water_sources[i]->setAvailable_volume(
                    realization_water_sources[i]->getAvailable_volume());
            continuity_water_sources[i]->setOutflow_previous_week(
                    realization_water_sources[i]->getTotal_outflow());
        }
    else
        for (int i = 0; i < continuity_water_sources.size();
             ++i) {   // Full capacity
            continuity_water_sources[i]->setAvailable_volume(
                    realization_water_sources[i]->getCapacity());
            continuity_water_sources[i]->setOutflow_previous_week(
                    realization_water_sources[i]->getTotal_outflow());
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
void ContinuityModelROF::setRealization_water_sources(
        const vector<WaterSource *> &water_sources_realization) {
    ContinuityModelROF::realization_water_sources = water_sources_realization;
}

/**
 * Checks if new infrastructure became online.
 */
void ContinuityModelROF::updateOnlineInfrastructure(int week) {
    for (int i = 0; i < realization_water_sources.size(); ++i) {
        if (realization_water_sources[i]->isOnline() &&
            !continuity_water_sources[i]->isOnline()) {
            continuity_water_sources[i]->setOnline();
            water_sources_capacities[i] =
                    continuity_water_sources[i]->getCapacity();
        }
    }

    if (Utils::isFirstWeekOfTheYear(week) || week == 0)
        for (int u = 0; u < continuity_utilities.size(); ++u) {
            utilities_capacities[u] =
                    continuity_utilities[u]->getTotal_storage_capacity();
        }
}

const Matrix3D<double> *ContinuityModelROF::getStorage_to_rof_table() const {
    return storage_to_rof_table;
}

