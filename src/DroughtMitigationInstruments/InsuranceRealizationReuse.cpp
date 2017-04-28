#include <algorithm>
#include "InsuranceRealizationReuse.h"
#include "../Utils/Utils.h"

/**
 * Insurance policy.
 * @param id Policy ID.
 * @param rof_triggers ROF triggers of all utilities using insurance. They MUST be entered sorted in ascending order
 * by utility ID. If utility does not use insurance, its trigger must be set to NONE.
 * @param insurance_premium risk premium insurance companies would charge (1.2 = 20%)
 */
Insurance::Insurance(const int id, const std::vector<double> &rof_triggers, const double insurance_premium,
                     const std::vector<WaterSource *> &water_sources, const std::vector<Utility *> &utilities,
                     const Graph &water_sources_graph, const std::vector<vector<int>> &water_sources_to_utilities,
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
}

Insurance::Insurance(const Insurance &insurance) : DroughtMitigationPolicy(insurance.id, INSURANCE),
                                                   ContinuityModelROF(Utils::copyWaterSourceVector(
                                                           insurance.continuity_water_sources),
                                                                      insurance.water_sources_graph,
                                                                      insurance.water_sources_to_utilities,
                                                                      Utils::copyUtilityVector(
                                                                              insurance.continuity_utilities),
                                                                      NON_INITIALIZED),
                                                   insurance_premium(insurance.insurance_premium),
                                                   rof_triggers(insurance.rof_triggers),
                                                   fixed_payouts(insurance.fixed_payouts) {}

void Insurance::applyPolicy(int week) {
    if (Utils::isFirstWeekOfTheYear(week)) {
        insurance_prices = priceInsurance(week);
    }
}

void Insurance::addSystemComponents(vector<Utility *> utilities, std::vector<WaterSource *> water_sources) {
    for (int i : ids_of_utilities_with_policies)
        this->realization_utilities.push_back((Utility *&&) utilities.at((unsigned long) i));

    setRealization_water_sources(water_sources);
}

/**
 * Calculates the insurance price based on an estimate of the expected payout in the coming year.
 * @param week
 * @return
 */
vector<double> Insurance::priceInsurance(int week) {

    unsigned long n_utilities = realization_utilities.size();

    std::vector<vector<vector<bool>>> realizations_utility_week_fail;
    std::vector<vector<vector<double>>> realizations_storages;

    /// checks if new infrastructure became available and, if so, set the corresponding realization infrastructure online.
    updateOnlineInfrastructure();

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    calculateRealizationsStoragesAndFailures(&realizations_utility_week_fail, &realizations_storages, week);

    /// get utilities combined storage capacities.
    std::vector<double> utility_combined_capacity;
    for (Utility *u : realization_utilities) {
        utility_combined_capacity.push_back(u->getTotal_storage_capacity());
    }

    /// calculate metric for differences in storage between realizations. The euclidian distance, calculated between
    /// two arrays of available water (one for each water source) is used for its simplicity.
    std::vector<vector<vector<double>>> distances_between_realizations(WEEKS_ROF_SHORT_TERM);
    /// calculate distances.
    for (int w = 0; w < WEEKS_ROF_SHORT_TERM - 1; ++w) {
        distances_between_realizations[w] = Utils::calculateDistances(realizations_storages[w]);
    }

    std::vector<double> insurance_prices(n_utilities, 0.0);
    /// calculate insurance prices for each utility.
    for (int u = 0; u < n_utilities; ++u) {
        /// checks if utility has a policy.
        if (rof_triggers[u] != NONE)
            insurance_prices[u] = calculateUtilityInsurancePrice(&realizations_utility_week_fail,
                                                                 &distances_between_realizations,
                                                                 u);
    }

    return insurance_prices;
}

/**
 * Runs on rof sub-realization for a given utility and a given pricing realization.
 * @param realizations_utility_week_fail matrix of weeks by realizations by utility with failure/success records.
 * @param rr sub-realization index (between 0 and NUMBER_REALIZATIONS_INSURANCE_PRICING).
 * @param u utility index.
 * @param w week index in the pricing realization;
 * @return failure or success.
 */
double Insurance::runRofSubRealization(vector<vector<bool>> *realizations_utility_week_fail, int rr, int w) {

    vector<vector<bool>> realizations_utility_week_fail_test = *realizations_utility_week_fail;
    double failure = NON_FAILURE;

    /// run time loop for rof realization.
    for (int ww = w; ww < w + WEEKS_ROF_SHORT_TERM; ++ww) {
        /// check which simulates storages time series should be used.
        if (ww < WEEKS_ROF_SHORT_TERM) {
            /// check for failure and, if found, mark year as failure and break off the loop.
            if ((*realizations_utility_week_fail)[ww][rr]) {
                failure = FAILURE;
                break;
            }
        } else {
            if (realizations_utility_week_fail_test[ww - WEEKS_ROF_SHORT_TERM][rr - 1]) {
                failure = FAILURE;
                break;
            }
        }
    }

    return failure;
}

/**
 * Calculate time series of storages for water sources and failures for utilities for all realizations.
 * @param realizations_utility_week_fail pointer to vector where failures are to be stored.
 * @param realizations_storages pointer to vector where storages are to be stored.
 * @param week
 */
void Insurance::calculateRealizationsStoragesAndFailures(
        std::vector<std::vector<std::vector<bool>>> *realizations_utility_week_fail,
        std::vector<std::vector<std::vector<double>>> *realizations_storages,
        int week) {
    unsigned long n_utilities = continuity_utilities.size();
    unsigned long n_water_sources = continuity_water_sources.size();

    realizations_utility_week_fail = new std::vector<vector<vector<bool>>>(n_utilities,
                                                                           std::vector<vector<bool>>(
                                                                                   WEEKS_ROF_SHORT_TERM,
                                                                                   std::vector<bool>(
                                                                                           NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                                                           NON_FAILURE)));
    realizations_storages = new std::vector<vector<vector<double>>>(WEEKS_ROF_SHORT_TERM,
                                                                    std::vector<vector<double>>(
                                                                            NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                                            std::vector<double>(
                                                                                    n_water_sources,
                                                                                    0.0)));

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (int r = 0; r < NUMBER_REALIZATIONS_INSURANCE_PRICING; r++) {

        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        this->resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = week; w < week + WEEKS_ROF_SHORT_TERM; ++w) {

            /// one week continuity time-step.
            this->continuityStep(w, r);

            double combined_storage;
            /// record storages and check week for failure for each utility in each realization.
            for (int u = 0; u < n_utilities; ++u) {
                combined_storage = 0;
                /// for each water source, record its stored volume, and add it to utility's stored ratio.
                for (int ws : water_sources_to_utilities[u]) {
                    combined_storage += continuity_water_sources[ws]->getAvailable_volume() /
                                        continuity_utilities[u]->getTotal_storage_capacity();
                    (*realizations_storages)[w][r][ws] = continuity_water_sources[ws]->getAvailable_volume();
                }
                /// record failures for a given utility, wek, and realization.
                if (combined_storage < STORAGE_CAPACITY_RATIO_FAIL)
                    (*realizations_utility_week_fail)[u][w][r] = FAILURE;
            }
        }
    }
}

/**
 * Calculate insurance price for utility u based off approximated rofs.
 * @param realizations_utility_week_fail
 * @param distances_between_realizations
 * @param u
 * @return insurance price for utility u.
 */
double Insurance::calculateUtilityInsurancePrice(
        std::vector<std::vector<std::vector<bool>>> *realizations_utility_week_fail,
        std::vector<std::vector<std::vector<double>>> *distances_between_realizations,
        int u) {

    double insurance_price = 0;
    double number_of_failed_realizations = 0;
    int n_realizations_total = (int) distances_between_realizations->at(0).size();
    int n_weeks = (int) realizations_utility_week_fail->at(0).size();
    double number_of_similar_sub_realizations = ceil(n_realizations_total *
                                                     INSURANCE_PRICING_SIMILARITY_QUANTILE);

    /// runs future realizations (pricing realizations) to see how much is expected to be paid in payouts for utility u.
    /// it starts from 1 because part of the previous time series is used for the pseudo-rof calculations, which would
    /// make it looks for a series with negative index.
    for (int r = 1; r < n_realizations_total; ++r) {
        /// runs time loop for each pricing realization.
        for (int w = 0; w < n_weeks; ++w) {
            number_of_failed_realizations = 0;
            /// runs rof realizations, based on the pricing realizations, that begin with the most similar water
            /// levels for all water sources, based on the previously calculated euclidian distances, in order
            /// to calculate rof for a given week of the pricing realization.
            vector<int> realizations_quantile = Utils::getQuantileIndeces((*distances_between_realizations)[w][r],
                                                                          INSURANCE_PRICING_SIMILARITY_QUANTILE);
            for (int rr : realizations_quantile) {
                if (rr != r && rr != 0)
                    number_of_failed_realizations += runRofSubRealization(&((*realizations_utility_week_fail)[u]), rr,
                                                                          w);
            }
            /// calculate rof and use it to determine the occurrence of payouts, which are in turn used to
            /// update the insurance prices.
            if (number_of_failed_realizations / number_of_similar_sub_realizations > rof_triggers[u])
                insurance_price += fixed_payouts[u] * insurance_premium;
        }
    }

    return insurance_price;
}