//
// Created by bernardoct on 4/30/17.
//

#include <vector>
#include <cmath>
#include <iostream>
#include "InsurancePseudoROF.h"
#include "../Utils/Utils.h"

/**
 * Insurance policy.
 * @param id Policy ID.
 * @param rof_triggers ROF triggers of all utilities using insurance. They MUST be entered sorted in ascending order
 * by utility ID. If utility does not use insurance, its trigger must be set to NONE.
 * @param insurance_premium risk premium insurance companies would charge (1.2 = 20%)
 */
InsurancePseudoROF::InsurancePseudoROF(const int id, const std::vector<double> &rof_triggers,
                                       const double insurance_premium,
                                       const std::vector<WaterSource *> &water_sources,
                                       const std::vector<Utility *> &utilities,
                                       const Graph &water_sources_graph,
                                       const std::vector<vector<int>> &water_sources_to_utilities,
                                       const std::vector<double> fixed_payouts)
        : DroughtMitigationPolicy(id, INSURANCE), ContinuityModelROF(Utils::copyWaterSourceVector(water_sources),
                                                                     water_sources_graph,
                                                                     water_sources_to_utilities,
                                                                     Utils::copyUtilityVector(utilities),
                                                                     NON_INITIALIZED), rof_triggers(rof_triggers),
          insurance_premium(insurance_premium), fixed_payouts(fixed_payouts) {

    /// Register utilities with insurance policies.
    for (int i = 0; i < rof_triggers.size(); ++i) {
        if (rof_triggers[i] != NONE) {
            ids_of_utilities_with_policies.push_back(i);
        }
    }

    for (int ws = 0; ws < water_sources.size(); ++ws) {
        capacities.push_back(water_sources[ws]->capacity);
    }

    for (vector<int> ds : water_sources_graph.getDownSources())
        if (ds.empty())
            downstream_sources.push_back(NON_INITIALIZED);
        else
            downstream_sources.push_back(ds[0]);

    sources_topological_order = water_sources_graph.getTopological_order();
}

InsurancePseudoROF::InsurancePseudoROF(const InsurancePseudoROF &insurance) : DroughtMitigationPolicy(insurance.id,
                                                                                                      INSURANCE),
                                                                              ContinuityModelROF(
                                                                                      Utils::copyWaterSourceVector(
                                                                                              insurance.continuity_water_sources),
                                                                                      insurance.water_sources_graph,
                                                                                      insurance.water_sources_to_utilities,
                                                                                      Utils::copyUtilityVector(
                                                                                              insurance.continuity_utilities,
                                                                                              true),
                                                                                      NON_INITIALIZED),
                                                                              insurance_premium(
                                                                                      insurance.insurance_premium),
                                                                              rof_triggers(insurance.rof_triggers),
                                                                              fixed_payouts(insurance.fixed_payouts),
                                                                              ids_of_utilities_with_policies(
                                                                                      insurance.ids_of_utilities_with_policies),
                                                                              downstream_sources(
                                                                                      insurance.downstream_sources),
                                                                              sources_topological_order(
                                                                                      insurance.sources_topological_order),
                                                                              capacities(insurance.capacities) {
}

InsurancePseudoROF::~InsurancePseudoROF() {

}

void InsurancePseudoROF::applyPolicy(int week) {
    /// if last week of the year, price insurance for coming year.
    if (Utils::isFirstWeekOfTheYear(week + 1)) {
        insurance_prices = priceInsurance(week);
    }

    /// check if payouts are triggered.
    for (int u = 0; u < continuity_utilities.size(); ++u) {
        if (continuity_utilities[u]->getRisk_of_failure() > rof_triggers[u])
            continuity_utilities[u]->addToContingencyFund(fixed_payouts[u]);
    }
}

void InsurancePseudoROF::addSystemComponents(vector<Utility *> utilities, std::vector<WaterSource *> water_sources) {
    for (int i : ids_of_utilities_with_policies)
        this->realization_utilities.push_back((Utility *&&) utilities.at((unsigned long) i));

    setRealization_water_sources(water_sources);
}

vector<double> InsurancePseudoROF::priceInsurance(int week) {
    unsigned long n_utilities = continuity_utilities.size();
    unsigned long n_water_sources = continuity_water_sources.size();

    /// one insrance price for each utility
    vector<double> insurance_prices(n_utilities, 0.0);

    /// storages for all water sources in all weeks of all realizations
    std::vector<std::vector<std::vector<double>>> realizations_storages(NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                                        std::vector<vector<double>>(n_water_sources,
                                                                                                    std::vector<double>(
                                                                                                            2 *
                                                                                                            WEEKS_ROF_SHORT_TERM,
                                                                                                            0.0)));
    /// rofs of all utilities in all weeks of all realizations
    std::vector<std::vector<std::vector<double>>> realizations_rofs(NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                                    std::vector<vector<double>>(n_utilities,
                                                                                                vector<double>(
                                                                                                        WEEKS_ROF_SHORT_TERM,
                                                                                                        0.0)));

    /// calculate storage curves for all realizations.
    calculateRealizationsStorages(&realizations_storages, week);

    /// for each realization, get rofs and check insurance use for the sake of pricing it.
    for (unsigned long r = 0; r < NUMBER_REALIZATIONS_INSURANCE_PRICING; ++r) {
        /// Get rof series for all utilities in all weeks of realization r.
        calculatePseudoRofs(&realizations_storages, &(realizations_rofs[r]), r);

        /// Increase insurance price every time insurance is triggered in any realization.
        for (unsigned long u = 0; u < n_utilities; ++u) {
            for (unsigned long w = 0; w < WEEKS_ROF_SHORT_TERM; ++w) {
                if (realizations_rofs[r][u][w] < rof_triggers[u])
                    insurance_prices[u] += fixed_payouts[u] * insurance_premium / NUMBER_REALIZATIONS_INSURANCE_PRICING;
            }
        }
    }

    return insurance_prices;
}

/**
 * Calculate time series of storages for water sources and failures for utilities for all realizations.
 * @param realizations_utility_week_fail pointer to vector where failures are to be stored.
 * @param realizations_storages pointer to vector where storages are to be stored.
 * @param week
 */
void
InsurancePseudoROF::calculateRealizationsStorages(std::vector<std::vector<std::vector<double>>> *realizations_storages,
                                                  int week) {
    unsigned long n_utilities = continuity_utilities.size();
    unsigned long n_water_sources = continuity_water_sources.size();
    unsigned long n_realizations = realizations_storages->size();

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (unsigned long r = 0; r < n_realizations; r++) {

        /// reset current reservoirs' and utilities' storage and combined storage in the corresponding
        /// realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = week; w < week + 2 * WEEKS_ROF_SHORT_TERM; ++w) {

            /// one week continuity time-step.
            continuityStep(w, (int) r);

            /// record storages and check week for failure for each utility in each realization.
            for (int ws = 0; ws < n_water_sources; ++ws) {
                /// for each water source, record its stored volume, and add it to utility's stored ratio
                realizations_storages->at(r)[ws][w - week] = continuity_water_sources[ws]->getAvailable_volume();
            }
        }
    }
}

void InsurancePseudoROF::calculatePseudoRofs(vector<vector<vector<double>>> *realizations_storages,
                                             vector<vector<double>> *realizations_rofs,
                                             unsigned long r) {
    int n_realizations = (int) realizations_storages->size();
    unsigned long n_utilities = continuity_utilities.size();
    unsigned long n_water_sources = realizations_storages->at(0).size();

    vector<vector<double>> realization_adjusted_storage_curves;
    vector<vector<double>> realization_adjusted_storage_curves_dummy;
    vector<double> storage0(n_water_sources);

    /// time loop for insurance realization
    for (unsigned long w = 0; w < WEEKS_ROF_SHORT_TERM; ++w) {
        /// adjust other realization curves for them to become rof realization storage curves
        for (int ws = 0; ws < n_water_sources; ++ws) {
            storage0[ws] = realizations_storages->at(r)[ws][w];
        }
        /// rof-realization loop for insurance realization
        for (unsigned long rr = 1; rr < n_realizations; ++rr) {
            /// prevent insurance realization from using itself for rof calculations
            if (rr != r) {
                realization_adjusted_storage_curves = shiftStorageCurves(&realizations_storages->at(rr), storage0, w);
                if (realization_adjusted_storage_curves.empty())
                    for (int ws = 0; ws < n_water_sources; ++ws) {
                        realization_adjusted_storage_curves = (*realizations_storages)[r];
                    }

                /// for each utility, in every week of rof realization, check storage curves for water sources to see
                /// if they failed for that realization
                for (unsigned long u = 0; u < n_utilities; ++u) {
                    for (unsigned long ww = w; ww < w + WEEKS_ROF_SHORT_TERM; ++ww) {
                        vector<double> utilities_combined_storage(n_utilities, 0.0);
                        for (int ws : water_sources_to_utilities[u]) {
                            utilities_combined_storage[u] += realization_adjusted_storage_curves[ws][ww];
                        }
                        /// if, at any point in time, storage-capacity ratio criteria is violated for utility u, count failure
                        if (utilities_combined_storage[u] / capacities[u] < STORAGE_CAPACITY_RATIO_FAIL) {
                            realizations_rofs->at(u)[w] += 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    /// normalize failure counts by number of realization for every utility.
    for (auto &ufr : *realizations_rofs) {
        for (double &week_failures : ufr) {
            week_failures /= n_realizations;
        }
    }
}

/**
 * Shifts storage curves of a system of sources up or down based on starting storage levels for each source.
 * @param storage_curves_2yrs
 * @param storage0
 * @param first_week
 * @return
 */
vector<vector<double>> InsurancePseudoROF::shiftStorageCurves(vector<vector<double>> *storage_curves_2yrs,
                                                              vector<double> storage0, unsigned long first_week) {

    unsigned long n_curves = storage_curves_2yrs->size();
    unsigned long n_weeks = WEEKS_ROF_SHORT_TERM;
    vector<vector<double>> storage_curves(n_curves, vector<double>(n_weeks, 0.));
    // FIXME: MEMORY DEALLOCATION ERROR WHEN USING LINES 235-239 INSTEAD OF 243-247 AND 253. I COULDN'T FIGURE OUT THE DETAILS YET, BUT IT NEEDS TO BE FIXED BECAUSE IT RUNS REALLY SLOW OTHERWISE.

    /// calculate added/removed storage from week 0.
    vector<double> delta_storage(n_curves);

    bool shift_curves = false;
    for (int ws = 0; ws < n_curves; ++ws) {
        delta_storage[ws] = storage0[ws] - (*storage_curves_2yrs)[ws][first_week];
        if (delta_storage[ws] / capacities[ws] > INSURANCE_SHIFT_STORAGE_CURVES_THRESHOLD) shift_curves = true;
    }

    if (shift_curves) {
        /// create matrix of year long curves.
//        for (unsigned long c = 0; c < n_curves; ++c) {
//                storage_curves[c] = vector<double>(storage_curves_2yrs->at(c).begin() + first_week,
//                                                   storage_curves_2yrs->at(c).end() + first_week + n_weeks);
//
//        }

        /// loop over time adjusting sources storages.
        for (unsigned long w = first_week; w < first_week + n_weeks; ++w) {
            for (int ws : sources_topological_order) {
                storage_curves[ws][w] += (*storage_curves_2yrs)[ws][w] + delta_storage[ws];
//                storage_curves[ws][w] += delta_storage[ws];

                /// if source overflows, send excess downstream. Works for intakes as well, since their capacities are 0,
                /// meaning all excess from above overflows.
                if (storage_curves[ws][w] > capacities[ws]) {
                    if (downstream_sources[ws] != NON_INITIALIZED)
                        storage_curves[downstream_sources[ws]][w] += storage_curves[ws][w] - capacities[ws];
                    storage_curves[ws][w] = capacities[ws];
                    /// prevent negative storages.
                } else if (storage_curves[ws][w] < 0)
                    storage_curves[ws][w] = 0;
            }
        }
    }

    return storage_curves;
}

double const InsurancePseudoROF::getInsurancePrice(int u) const {
    return insurance_prices[u];
}