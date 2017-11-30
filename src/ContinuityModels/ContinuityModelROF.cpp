//
// Created by bernardo on 1/26/17.
//

#include <iostream>
#include <algorithm>
#include "ContinuityModelROF.h"
#include "../Utils/Utils.h"

#define restrict __restrict__
#define __assume_aligned __builtin_assume_aligned

ContinuityModelROF::ContinuityModelROF(
        vector<WaterSource *> water_sources,
        const Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> utilities,
        vector<MinEnvironFlowControl *> &min_env_flow_controls,
        const unsigned int realization_id) : ContinuityModel(water_sources,
                                                             utilities,
                                                             min_env_flow_controls,
                                                             water_sources_graph,
                                                             water_sources_to_utilities,
                                                             realization_id),
                                             n_topo_sources((int) sources_topological_order.size()) {

    /// update utilities' total stored volume
    for (Utility *u : this->continuity_utilities) {
        u->updateTotalStoredVolume();
        u->setNoFinaicalCalculations();
    }

    storage_to_rof_realization = Matrix3D<double>(
            (unsigned int) continuity_utilities.size(),
            (unsigned int) NO_OF_STORAGE_TO_ROF_TABLE_TIERS,
            (unsigned int) ceil(WEEKS_IN_YEAR));
    storage_to_rof_table = new Matrix3D<double>(
            (unsigned int) continuity_utilities.size(),
            (unsigned int) NO_OF_STORAGE_TO_ROF_TABLE_TIERS,
            (unsigned int) ceil(WEEKS_IN_YEAR));

    /// Record which sources have no downstream sources.
    capacities_toposorted = new double[n_topo_sources];
    downstream_sources_toposort = new int[n_topo_sources];
    topo_sorted_to_all_sources = new int[n_sources];
    memset(topo_sorted_to_all_sources, -1, sizeof(int) * n_sources);
    for (int ws = 0; ws < n_topo_sources; ++ws) {
        capacities_toposorted[ws] = water_sources[sources_topological_order[ws]]->getCapacity();
        downstream_sources_toposort[ws] = downstream_sources[sources_topological_order[ws]];
        topo_sorted_to_all_sources[sources_topological_order[ws]] = ws;
    }
}

ContinuityModelROF::ContinuityModelROF(ContinuityModelROF &continuity_model_rof)
        : ContinuityModel(continuity_model_rof.continuity_water_sources,
                          continuity_model_rof.continuity_utilities,
                          continuity_model_rof.min_env_flow_controls,
                          continuity_model_rof.water_sources_graph,
                          continuity_model_rof.water_sources_to_utilities,
                          continuity_model_rof.realization_id),
          n_topo_sources(continuity_model_rof.n_topo_sources) {

    std::copy(continuity_model_rof.capacities_toposorted,
              continuity_model_rof.capacities_toposorted + n_topo_sources,
              capacities_toposorted);
    std::copy(continuity_model_rof.topo_sorted_to_all_sources,
              continuity_model_rof.topo_sorted_to_all_sources + n_topo_sources,
              topo_sorted_to_all_sources);
    std::copy(continuity_model_rof.downstream_sources_toposort,
              continuity_model_rof.downstream_sources_toposort + n_topo_sources,
              downstream_sources_toposort);
}

ContinuityModelROF::~ContinuityModelROF() = default;

/**
 * Runs one the full rof calculations for realization #realization_id for a given week.
 * @param week for which rof is to be calculated.
 */
vector<double> ContinuityModelROF::calculateShortTermROF(int week) {

    // vector where risks of failure will be stored.
    vector<double> risk_of_failure((unsigned long) n_utilities, 0.0);
    vector<double> year_failure((unsigned long) n_utilities, 0.0);

    int week_of_the_year = Utils::weekOfTheYear(week);

    /// checks if new infrastructure became available and, if so, set the
    /// corresponding realization infrastructure online.
    updateOnlineInfrastructure(week);

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50)
    /// yearly realization.
    for (int yr = 0; yr < NUMBER_REALIZATIONS_ROF; ++yr) {
        storage_to_rof_realization.reset(NON_FAILURE);

        /// loop to calculate storage to rof table
        /// necessary because that table assumes initially-full storage
        resetUtilitiesAndReservoirs(LONG_TERM_ROF);

        for (int w = 0; w < WEEKS_ROF_SHORT_TERM; ++w) {

            /// one week continuity time-step.
            continuityStep(w + week, yr, !APPLY_DEMAND_BUFFER);

            /// calculated week of storage-rof table
            updateStorageToROFTable(week_of_the_year);
        }

        /// update storage-rof table
        *storage_to_rof_table += storage_to_rof_realization /
                                 NUMBER_REALIZATIONS_ROF;

        /// reset current reservoirs' and utilities' storage and combined
        /// storage, respectively, in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = 0; w < WEEKS_ROF_SHORT_TERM; ++w) {

            /// one week continuity time-step.
            continuityStep(w + week, yr, !APPLY_DEMAND_BUFFER);

            /// check total available storage for each utility and, if smaller
            /// than the fail ratio, increase the number of failed years of
            /// that utility by 1 (FAILURE).
            for (int u = 0; u < n_utilities; ++u) {

                if (continuity_utilities[u]->getStorageToCapacityRatio() > 1) {
                    /// check for continuity failure
                    cout << "Utility (name and ID): " << continuity_utilities[u]->name << ", "
                         << continuity_utilities[u]->id << endl;
                    cout << "Storage-to-Capacity Ratio: "
                         << continuity_utilities[u]->getStorageToCapacityRatio() << endl;
                    __throw_out_of_range("Storage-to-Capacity ratio for Utility is greater than 1 "
                                                 "within short-term ROF calculations.");
                }

                if (continuity_utilities[u]->getStorageToCapacityRatio() <= STORAGE_CAPACITY_RATIO_FAIL)
                    year_failure[u] = FAILURE;
            }
        }

        /// Count failures and reset failures counter.
        for (int uu = 0; uu < n_utilities; ++uu) {
            risk_of_failure[uu] += year_failure[uu];
            year_failure[uu] = NON_FAILURE;
        }
    }

    /// Set first tier for ROF table calculation close to where the first
    /// failure was observed for last week's table, so to save computations.
//    for (int s = 0; s < NO_OF_STORAGE_TO_ROF_TABLE_TIERS; ++s) {
//        int count_failures = 0;
//        for (int u = 0; u < n_utilities; ++u) {
//            if ((*storage_to_rof_table)(u, NO_OF_STORAGE_TO_ROF_TABLE_TIERS - s,
//                        week_of_the_year) > 0.)
//                ++count_failures;
//        }
//        if (count_failures == 0)
//            beginning_tier = max(0, s - 1);
//        else
//            break;
//    }

//    cout << "Week " << week_of_the_year << " B. Tier " << beginning_tier << endl;

    if (week > 1404 & week < 1457) {
        /// print table for future week when raw water transfers/insurance payouts are happening
        storage_to_rof_table->print(week_of_the_year);
        cout << endl;
    }

    /// Finish ROF calculations
    for (int i = 0; i < n_utilities; ++i) {
        risk_of_failure[i] /= NUMBER_REALIZATIONS_ROF;
    }

    return risk_of_failure;
}

/**
 * Runs one the full rof calculations for realization #realization_id for a given week.
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

void ContinuityModelROF::updateStorageToROFTable(int week_of_the_year) {
    double available_volumes[n_topo_sources];
//    __declspec(align(64)) double spillage[n_topo_sources];
    double spillage[n_topo_sources] __attribute__ ((aligned(64)));
    for (int ws = 0; ws < n_topo_sources; ++ws) {
        available_volumes[ws] = continuity_water_sources[sources_topological_order[ws]]->getAvailable_volume();
        spillage[ws] = continuity_water_sources[ws]->getTotal_outflow() -
                continuity_water_sources[ws]->getMin_environmental_outflow();
    }

    /// loops over the percent storage levels to populate table. The loop
    /// begins from one level above the level  where at least one failure was
    /// observed in the last iteration. This saves a lot of computational time.
//    __declspec(align(64)) double delta_storage[n_topo_sources];
//    __declspec(align(64)) double available_volumes_shifted[n_topo_sources];
//    double delta_storage[n_topo_sources] __attribute__ ((aligned(64)));
//    double available_volumes_shifted[n_topo_sources] __attribute__ ((aligned(64)));
    for (int s = NO_OF_STORAGE_TO_ROF_TABLE_TIERS; s > 0; --s) {
        /// calculate delta storage for all reservoirs and array that will
        /// receive the shifted storage curves.

        /// Shift storages.
        shiftStorages(available_volumes, spillage);

        //FIXME: ADJUST FOR TOPOLOGICAL RE-SORTING
        /// Checks for utilities failures.
        int count_fails = 0;
        double utilities_storages[n_utilities];
        memset(utilities_storages, 0, sizeof(int) * n_utilities);
        for (unsigned int u = 0; u < n_utilities; ++u) {
            /// Calculate combined stored volume for each utility based on
            /// shifted storages.

            utilities_storages[u] = 0;
            for (int ws : water_sources_online_to_utilities[u]) {
                utilities_storages[u] += available_volumes[topo_sorted_to_all_sources[ws]] *
                                         continuity_water_sources[ws]->getAllocatedFraction(u) *
                                         (continuity_water_sources[ws]->getAllocatedTreatmentCapacity(u) > 0);
            }
        }
        /// Register failure in the table for each utility meeting failure criteria.
        double adjusted_fail_threshold = STORAGE_CAPACITY_RATIO_FAIL +
                                         ((double)NO_OF_STORAGE_TO_ROF_TABLE_TIERS - (double) s) /
                                                 (double)NO_OF_STORAGE_TO_ROF_TABLE_TIERS;

        for (unsigned int u = 0; u < n_utilities; ++u) {
            /// Register failure in the table for each utility meeting
            /// failure criteria, as well as any storage levels below

            if (utilities_storages[u] / utilities_capacities[u] < adjusted_fail_threshold) {
                for (int tier = (s - 1); tier > -1; --tier)
                    storage_to_rof_realization(u, tier, week_of_the_year) = FAILURE;
                count_fails++;
            }
        }

        for (unsigned int u = 0; u < n_utilities; ++u) {
            /// CHECK FOR CONTINUITY FALIURES

            if (utilities_storages[u]/utilities_capacities[u] > 1) {

                cout << "Utility (name and ID): " << continuity_utilities[u]->name << ", "
                     << continuity_utilities[u]->id << endl;
                cout << "Storage: " << utilities_storages[u] << endl;
                cout << "Capacity: " << utilities_capacities[u] << endl;

                for (int ws : water_sources_online_to_utilities[u]) {
                    cout << "Member Source: " << continuity_water_sources[ws]->name << ", (Allocated) Storage: "
                         << continuity_water_sources[ws]->getAvailableAllocatedVolume(u) << ", (Allocated) Capacity: "
                         << continuity_water_sources[ws]->getAllocatedCapacity(u) << ", Assigned Storage during Calc: "
                         << available_volumes[topo_sorted_to_all_sources[ws]]*
                            continuity_water_sources[ws]->getAllocatedFraction(u) *
                            (continuity_water_sources[ws]->getAllocatedTreatmentCapacity(u) > 0) << endl;
                }
                __throw_out_of_range("Storage-to-Capacity ratio for Utility is greater than 1 "
                                             "within Storage-to-ROF table calculations.");
            }
        }

        /// If all utilities have failed, stop dropping storage level
        if (count_fails == n_utilities) {
//            for (int ss = s; ss <= NO_OF_STORAGE_TO_ROF_TABLE_TIERS; ++ss) {
//                for (unsigned int u = 0; u < n_utilities; ++u) {
//                    storage_to_rof_realization(u, NO_OF_STORAGE_TO_ROF_TABLE_TIERS - ss, week_of_the_year) = FAILURE;
//                }
//            }
            break;
        }
    }
}

//FIXME: MAKE THIS MORE EFFICIENT. THIS METHOD IS THE MOST EXPENSIVE ONE IN THE CODE.
/// shiftStorages function that accepts one parameter
/// and calculates estimated week volume plus potential spillage
void ContinuityModelROF::shiftStorages(double* available_volumes, const double* spillage) {
    __assume_aligned(available_volumes, 64);
    __assume_aligned(capacities_toposorted, 64);
    __assume_aligned(spillage, 64);

    for (int ws = 0; ws < n_topo_sources - 1; ++ws) {
        double spillage_retrieved = min(spillage[ws], capacities_toposorted[ws] - available_volumes[ws]);
        if (spillage_retrieved < 0)
            spillage_retrieved = 0;

        available_volumes[ws] += spillage_retrieved;
        available_volumes[downstream_sources_toposort[ws]] -= spillage_retrieved;

        if (available_volumes[ws] > capacities_toposorted[ws])
            available_volumes[ws] = capacities_toposorted[ws];
        if (available_volumes[downstream_sources_toposort[ws]] >
                capacities_toposorted[downstream_sources_toposort[ws]])
            available_volumes[downstream_sources_toposort[ws]] = capacities_toposorted[downstream_sources_toposort[ws]];
    }
    /// if not full, retrieve spill to downstream source for source left out of loop above
    double spillage_retrieved = min(spillage[n_topo_sources-1], capacities_toposorted[n_topo_sources-1] -
            available_volumes[n_topo_sources-1]);
    if (spillage_retrieved < 0)
        spillage_retrieved = 0;
    available_volumes[n_topo_sources-1] += spillage_retrieved;

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
                    (realization_water_sources[i]->getAvailable_allocated_volumes(),
                     realization_water_sources[i]->getAvailable_volume());
            continuity_water_sources[i]->setOutflow_previous_week(realization_water_sources[i]->getTotal_outflow());
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
void ContinuityModelROF::setRealization_water_sources(
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
                    continuity_utilities[u]->setWaterSourceOnline((unsigned int) i);
                }
            }

    for (int ws : sources_topological_order)
        capacities_toposorted[ws] =
                realization_water_sources[sources_topological_order[ws]]->getCapacity();

    if (Utils::isFirstWeekOfTheYear(week) || week == 0)
        for (int u = 0; u < n_utilities; ++u) {
            utilities_capacities[u] =
                    continuity_utilities[u]->getTotal_storage_capacity();
        }
}

const Matrix3D<double> *ContinuityModelROF::getStorage_to_rof_table() const {
    return storage_to_rof_table;
}

