#include <algorithm>
#include "Insurance.h"
#include "../Utils/Utils.h"

/**
 * Insurance policy.
 * @param id Policy ID.
 * @param rof_triggers ROF triggers of all utilities using insurance. They MUST be entered sorted in ascending order
 * by utility ID. If utility does not use insurance, its trigger must be set to NONE.
 * @param insurance_premium risk premium insurance companies would charge (1.2 = 20%)
 */
Insurance::Insurance(const int id, const vector<double> &rof_triggers, const double insurance_premium,
                     const vector<WaterSource *> &water_sources, const vector<Utility *> &utilities,
                     const Graph &water_sources_graph,
                     const vector<vector<int>> &water_sources_to_utilities)
        : DroughtMitigationPolicy(id, INSURANCE), ContinuityModelROF(Utils::copyWaterSourceVector(water_sources),
                                                                     water_sources_graph,
                                                                     water_sources_to_utilities,
                                                                     Utils::copyUtilityVector(utilities),
                                                                     NON_INITIALIZED), rof_triggers(rof_triggers),
          insurance_premium(insurance_premium) {

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
                                                   rof_triggers(insurance.rof_triggers) {}

void Insurance::applyPolicy(int week) {
    if (Utils::isFirstWeekOfTheYear(week)) {
        insurance_price = priceInsurance(0);
    }
}

void Insurance::addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) {
    for (int i : ids_of_utilities_with_policies)
        this->utilities.push_back((Utility *&&) utilities.at((unsigned long) i));

    for (Utility *u : this->utilities) {
        this->sources_to_utilities_ids.push_back(vector<int>());
        if (rof_triggers.at((unsigned long) u->id) != NONE) {
            for (auto ws : u->getWaterSources())
                this->sources_to_utilities_ids[sources_to_utilities_ids.size() - 1].push_back(ws.second->id);
        }
    }

    setWater_sources_realization(water_sources);
}

/**
 * Calculates the insurance price based on an estimate of the expected payout in the coming year.
 * @param week
 * @return
 */
double Insurance::priceInsurance(int week) {
    vector<vector<vector<double>>> realizations_storages(WEEKS_ROF_SHORT_TERM,
                                                         std::vector<vector<double>>(
                                                                 NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                                 vector<double>(continuity_water_sources.size(), 0.0)));
    vector<vector<vector<int>>> realizations_week_fail(WEEKS_ROF_SHORT_TERM,
                                                       std::vector<vector<int>>(NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                                                vector<int>(
                                                                                        continuity_water_sources.size(),
                                                                                        NON_FAILURE)));

    double **week_storages = new double *[NUMBER_REALIZATIONS_INSURANCE_PRICING];
    double **week_storage_distances = new double *[NUMBER_REALIZATIONS_INSURANCE_PRICING];

    for (int r = 0; r < NUMBER_REALIZATIONS_INSURANCE_PRICING; ++r) {
        week_storages[r] = new double[continuity_water_sources.size()];
        week_storage_distances[r] = new double[NUMBER_REALIZATIONS_INSURANCE_PRICING];
    }

    /// perform a continuity simulation for NUMBER_REALIZATIONS_ROF (50) yearly realization.
    for (int r = 0; r < NUMBER_REALIZATIONS_INSURANCE_PRICING; r++) {

        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        this->resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = week; w < week + WEEKS_ROF_SHORT_TERM; ++w) {

            /// one week continuity time-step.
            this->continuityStep(w, r);

            /// record storages.
            for (int ws = 0; ws < continuity_water_sources.size(); ++ws) {
                realizations_storages[w][r][ws] = continuity_water_sources[ws]->getAvailable_volume();
                if (realizations_storages[w][r][ws] < STORAGE_CAPACITY_RATIO_FAIL)
                    realizations_week_fail[w][r][ws] = FAILURE;
            }
        }
    }

    for (int w = 0; w < WEEKS_ROF_SHORT_TERM; ++w) {
        for (int r = 0; r < NUMBER_REALIZATIONS_INSURANCE_PRICING; ++r) {
            for (int ws = 0; ws < continuity_water_sources.size(); ++ws) {
                week_storages[r][ws] = realizations_storages[w][r][ws];
            }
        }
    }

    vector<vector<vector<double>>> distances_between_realizations(WEEKS_ROF_SHORT_TERM, std::vector<vector<double>>(
                                                                  NUMBER_REALIZATIONS_INSURANCE_PRICING,
                                                          vector<double>(NUMBER_REALIZATIONS_INSURANCE_PRICING, 0.0)));
    vector<vector<double>> rof_series_utilities(continuity_water_sources.size(),
                                                vector<double>(WEEKS_ROF_SHORT_TERM, 0.0));
    vector<int> indexes_realizations_rof;
    double utility_combined_storage;
    vector<double> utility_combined_capacity;
    for (Utility *u : utilities) {
        utility_combined_capacity.push_back(u->getTotal_storage_capacity());
    }

    for (int w = 0; w < WEEKS_ROF_SHORT_TERM - 1; ++w) {
        for (int i = 0; i < NUMBER_REALIZATIONS_INSURANCE_PRICING; ++i) {
            for (int j = i; j < NUMBER_REALIZATIONS_INSURANCE_PRICING; ++j) {
                distances_between_realizations[w][i][j] = Utils::l2distanceSquare(realizations_storages[w][i],
                                                                               realizations_storages[w][j]);
                distances_between_realizations[w][j][i] = distances_between_realizations[w][i][j];
            }
        }
    }
}