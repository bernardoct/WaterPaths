//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include <algorithm>
#include "ContinuityModelROF.h"
#include "../Utils/Utils.h"

ContinuityModelROF::ContinuityModelROF(
        vector<WaterSource *> water_sources,
        const Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> utilities,
        vector<MinEnvironFlowControl *> &min_env_flow_controls,
        vector<vector<double>> *utilities_rdm,
        vector<vector<double>> *water_sources_rdm,
        const unsigned int realization_id) : ContinuityModel(water_sources, utilities, min_env_flow_controls,
                                                             water_sources_graph, water_sources_to_utilities, utilities_rdm,
                                                             water_sources_rdm,
                                                             realization_id),
                                             n_topo_sources((int) sources_topological_order.size()) {

    /// update utilities' total stored volume
    for (Utility *u : this->continuity_utilities) {
        u->updateTotalStoredVolume();
        u->setNoFinaicalCalculations();
    }

    storage_to_rof_realization = Matrix3D<double>(
            (unsigned int) n_utilities,
            (unsigned int) NO_OF_INSURANCE_STORAGE_TIERS + 1,
            (unsigned int) ceil(WEEKS_IN_YEAR));
    storage_to_rof_table = new Matrix3D<double>(
            (unsigned int) n_utilities,
            (unsigned int) NO_OF_INSURANCE_STORAGE_TIERS + 1,
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
                          continuity_model_rof.utilities_rdm,
                          continuity_model_rof.water_sources_rdm,
                          continuity_model_rof.realization_id),
          n_topo_sources(continuity_model_rof.n_topo_sources) {
}

ContinuityModelROF::~ContinuityModelROF() = default;

/**
 * Runs one the full rof calculations for realization #realization_id for a
 * given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateLongTermROF(int week) {

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure((unsigned long) n_utilities, 0.0);
    vector<double> year_failure((unsigned long) n_utilities, 0.0);

    storage_to_rof_table->reset(NON_FAILURE);

    /// checks if new infrastructure became available and, if so, set the
    /// corresponding realization
    /// infrastructure online.
    updateOnlineInfrastructure(week);

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly
    /// realization.
    for (int yr = 0; yr < NUMBER_REALIZATIONS_ROF; ++yr) {
        /// reset current reservoirs' and utilities' storage and combined
        /// storage, respectively, in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(LONG_TERM_ROF);

        for (int w = 0; w < WEEKS_ROF_LONG_TERM; ++w) {
            /// one week continuity time-step.
            continuityStep(w + week, yr, APPLY_DEMAND_BUFFER);

            /// check total available storage for each utility and, if smaller
            /// than the fail ration, increase the number of failed years of
            /// that utility by 1 (FAILURE).
            for (int u = 0; u < n_utilities; ++u)
                if (continuity_utilities[u]->getStorageToCapacityRatio() <=
                    STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[u] = FAILURE;
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

/**
 * Runs one the full rof calculations for realization #realization_id for a
 * given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateShortTermROF(int week) {

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure((unsigned long) n_utilities, 0.0);
    vector<double> year_failure((unsigned long) n_utilities, 0.0);
    double to_full[n_sources];

    for (int ws = 0; ws < n_sources; ++ws) {
        to_full[ws] = realization_water_sources[ws]->getCapacity() -
                      realization_water_sources[ws]->getAvailable_volume();
    }

    int week_of_the_year = Utils::weekOfTheYear(week);

    /// checks if new infrastructure became available and, if so, set the
    /// corresponding realization infrastructure online.
    updateOnlineInfrastructure(week);

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50)
    /// yearly realization.
    for (int yr = 0; yr < NUMBER_REALIZATIONS_ROF; ++yr) {
        storage_to_rof_realization.reset(NON_FAILURE);

        /// reset current reservoirs' and utilities' storage and combined
        /// storage, respectively, in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = 0; w < WEEKS_ROF_SHORT_TERM; ++w) {

            /// one week continuity time-step.
            continuityStep(w + week, yr, !APPLY_DEMAND_BUFFER);

            /// check total available storage for each utility and, if smaller
            /// than the fail ration, increase the number of failed years of
            /// that utility by 1 (FAILURE).
            for (int u = 0; u < n_utilities; ++u)
                if (continuity_utilities[u]->getStorageToCapacityRatio() <=
                    STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[u] = FAILURE;

            /// calculated week of storage-rof table
            updateStorageToROFTable(INSURANCE_SHIFT_STORAGE_CURVES_THRESHOLD,
                                    week_of_the_year, to_full);
        }

        /// update storage-rof table
        *storage_to_rof_table += storage_to_rof_realization /
                                 NUMBER_REALIZATIONS_ROF;

        /// Count failures and reset failures counter.
        for (int uu = 0; uu < n_utilities; ++uu) {
            risk_of_failure[uu] += year_failure[uu];
            year_failure[uu] = NON_FAILURE;
        }
    }

    /// Set first tier for ROF table calculation close to where the first
    /// failure was observed for last week's table, so to save computations.
    for (int s = 0; s < NO_OF_INSURANCE_STORAGE_TIERS; ++s) {
        int count_failures = 0;
        for (int u = 0; u < n_utilities; ++u) {
            if ((*storage_to_rof_table)(u, NO_OF_INSURANCE_STORAGE_TIERS - s,
                                        week_of_the_year) > 0.)
                ++count_failures;
        }
        if (count_failures == 0)
            beginning_tier = max(0, s - 1);
        else
            break;
    }

    cout << "Week " << week << " B. Tier " << beginning_tier << endl;
    storage_to_rof_table->print(week_of_the_year);
    cout << endl;

    /// Finish ROF calculations
    for (int i = 0; i < n_utilities; ++i) {
        risk_of_failure[i] /= NUMBER_REALIZATIONS_ROF;
    }

    return risk_of_failure;
}

void ContinuityModelROF::updateStorageToROFTable(double storage_percent_decrement,
                                                 int week_of_the_year,
                                                 const double *to_full) {

    double available_volumes[n_sources];
    for (int ws = 0; ws < n_sources; ++ws)
        available_volumes[ws] =
                continuity_water_sources[ws]->getAvailable_volume();

    /// loops over the percent storage levels to populate table. The loop
    /// begins from one level above the level  where at least one failure was
    /// observed in the last iteration. This saves a lot of computational time.
    for (int s = beginning_tier; s <= NO_OF_INSURANCE_STORAGE_TIERS; ++s) {
        /// calculate delta storage for all reservoirs and array that will
        /// receive the shifted storage curves.
        double percent_decrement_storage_level =
                (double) s * storage_percent_decrement;
        double delta_storage[n_sources];
        double available_volumes_shifted[n_sources];
        std::copy(available_volumes, available_volumes + n_sources,
                  available_volumes_shifted);

        /// calculate the difference between the simulated available water and
        /// the one for the table calculated above based on the percent
        /// decrement.
        for (int k = 0; k < n_sources; ++k)
            delta_storage[k] = to_full[k] - water_sources_capacities[k] *
                                            percent_decrement_storage_level;

        /// Shift storages.
        shiftStorages(available_volumes_shifted, delta_storage);

        /// Checks for utilities failures.
        int count_fails = 0;
        for (unsigned int u = 0; u < n_utilities; ++u) {
            double utility_storage = 0;
            /// Calculate combined stored volume for each utility based on
            /// shifted storages.
            for (int ws : water_sources_online_to_utilities[u])
                utility_storage += available_volumes_shifted[ws] *
                                   continuity_water_sources[ws]
                                           ->getAllocatedFraction(u);
            /// Register failure in the table for each utility meeting
            /// failure criteria.
            if (utility_storage / utilities_capacities[u] <
                STORAGE_CAPACITY_RATIO_FAIL) {
                storage_to_rof_realization(u, NO_OF_INSURANCE_STORAGE_TIERS - s,
                                           week_of_the_year) = FAILURE;
                count_fails++;
            }
        }

        /// If all utilities have failed, stop dropping storage level and label
        /// all storage levels below failures.
        if (count_fails == n_utilities) {
            for (int ss = s; ss <= NO_OF_INSURANCE_STORAGE_TIERS; ++ss) {
                for (unsigned int u = 0; u < n_utilities; ++u) {
                    storage_to_rof_realization(u,
                                               NO_OF_INSURANCE_STORAGE_TIERS - ss,
                                               week_of_the_year) = FAILURE;
                }
            }
            break;
        }
    }
}

//FIXME: MAKE THIS MORE EFFICIENT. THIS METHOD IS THE MOST EXPENSIVE ONE IN THE CODE.
void ContinuityModelROF::shiftStorages(
        double * available_volumes_shifted,
        const double * delta_storage) {

    /// Add deltas to all sources following the topological order, so that
    /// upstream is calculated before downstream.
    for (int ws : sources_topological_order) {

        /// calculate initial estimate for shifted
        available_volumes_shifted[ws] += delta_storage[ws];

        double available_volume_to_full = water_sources_capacities[ws] -
                                          available_volumes_shifted[ws];

        /// if not full, retrieve spill to downstream source.
        if (available_volume_to_full > 0) {
            /// Calculate spilled water. Since the curves are shifted as the
            /// weeks of the rof realizations are calculated, the minimum
            /// environmental outflows below will be the ones at the time when
            /// the storage is being shifted.
            double spillage =
                    continuity_water_sources[ws]->getTotal_outflow() -
                    continuity_water_sources[ws]
                            ->getMin_environmental_outflow();

            double spillage_retrieved = min(available_volume_to_full,
                                            spillage);

            available_volumes_shifted[ws] += spillage_retrieved;

            if (storage_wout_downstream[ws])
                available_volumes_shifted[downstream_sources[ws]] -=
                        spillage_retrieved;
        }
    }
}

/**
 * reset reservoirs' and utilities' storage and last release, and combined
 * storage, respectively, they currently have in the corresponding realization
 * simulation.
 */
void ContinuityModelROF::resetUtilitiesAndReservoirs(int rof_type) {

    /// update water sources info. If short-term rof, return to current storage;
    /// if long-term, make them full.
    if (rof_type == SHORT_TERM_ROF)
        for (int i = 0; i < n_sources; ++i) {   // Current available volume
            continuity_water_sources[i]->setAvailableAllocatedVolumes
                    (realization_water_sources[i]
                             ->getAvailable_allocated_volumes(),
                     realization_water_sources[i]->getAvailable_volume());
            continuity_water_sources[i]->setOutflow_previous_week(
                    realization_water_sources[i]->getTotal_outflow());
        }
    else
        for (int i = 0; i < n_sources; ++i) {   // Full capacity
            continuity_water_sources[i]->setFull();
            continuity_water_sources[i]->setOutflow_previous_week(
                    realization_water_sources[i]->getTotal_outflow());
        }

    /// update utilities combined storage.
    for (Utility *u : continuity_utilities) {
        u->updateTotalStoredVolume();
    }
}

/**
 * Pass to the rof continuity model the locations of the utilities of the realization it calculated rofs
 * for. This is done so that the rof calculations can reset the reservoir and utilities states for each of
 * the rofs' year realizations.
 * @param water_sources_realization
 */
void ContinuityModelROF::connectRealizationWaterSources(
        const vector<WaterSource *> &water_sources_realization) {
    ContinuityModelROF::realization_water_sources = water_sources_realization;
}

/**
 * Checks if new infrastructure became online.
 */
void ContinuityModelROF::updateOnlineInfrastructure(int week) {
    for (int i = 0; i < n_sources; ++i)
        for (int u = 0; u < n_utilities; ++u)
            if (realization_water_sources[i]->isOnline() &&
                !continuity_water_sources[i]->isOnline()) {
                for (int u : utilities_to_water_sources[i]) {
                    water_sources_online_to_utilities[u].push_back(i);
                    continuity_utilities[u]
                            ->setWaterSourceOnline((unsigned int) i);
                }
            }

    if (Utils::isFirstWeekOfTheYear(week) || week == 0)
        for (int u = 0; u < n_utilities; ++u) {
            utilities_capacities[u] =
                    continuity_utilities[u]->getTotal_storage_capacity();
        }
}

const Matrix3D<double> *ContinuityModelROF::getStorage_to_rof_table() const {
    return storage_to_rof_table;
}

