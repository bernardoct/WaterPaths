//
// Created by bernardoct on 5/1/17.
//

#include <iostream>
#include "InsuranceStorageToROF.h"
#include "../Utils/Utils.h"

InsuranceStorageToROF::InsuranceStorageToROF(const int id, vector<WaterSource *> &water_sources,
                                             const Graph &water_sources_graph,
                                             const vector<vector<int>> &water_sources_to_utilities,
                                             vector<Utility *> &utilities,
                                             vector<MinEnvFlowControl *> min_env_flow_controls,
                                             vector<vector<double>>& utilities_rdm,
                                             vector<vector<double>>& water_sources_rdm, vector<double> &rof_triggers,
                                             const double insurance_premium, const vector<double> &fixed_payouts,
                                             unsigned long total_simulation_time)
        : DroughtMitigationPolicy(id, INSURANCE_STORAGE_ROF),
          ContinuityModelROF(Utils::copyWaterSourceVector(water_sources), water_sources_graph,
                             water_sources_to_utilities, Utils::copyUtilityVector(utilities, true),
                             Utils::copyMinEnvFlowControlVector(min_env_flow_controls), utilities_rdm.at(0), water_sources_rdm.at(0),
                             total_simulation_time, false, (unsigned int) NON_INITIALIZED),
          insurance_premium(insurance_premium), fixed_payouts(fixed_payouts),
          rof_triggers(rof_triggers),
          utilities_revenue_last_year(vector<double>((unsigned long) n_utilities, 0.)),
          utilities_revenue_update(vector<double>((unsigned long) n_utilities, 0.)),
          total_simulation_time(total_simulation_time){

    for (Utility *u : utilities) utilities_ids.push_back(u->id);

    for (Utility *u : continuity_utilities) {
        u->clearWaterSources();
        u->resetTotal_storage_capacity();
    }

    insurance_price = vector<double>((unsigned long) n_utilities);
}

InsuranceStorageToROF::InsuranceStorageToROF(
        InsuranceStorageToROF &insurance) :
        DroughtMitigationPolicy(insurance.id, insurance.type),
        ContinuityModelROF(Utils::copyWaterSourceVector(insurance.continuity_water_sources),
                           insurance.water_sources_graph,
                           insurance.water_sources_to_utilities,
                           Utils::copyUtilityVector(insurance.continuity_utilities, true),
                           Utils::copyMinEnvFlowControlVector(insurance.min_env_flow_controls),
                           insurance.utilities_rdm,
                           insurance.water_sources_rdm,
                           insurance.total_simulation_time,
                           false, insurance.realization_id),
        insurance_premium(insurance.insurance_premium),
        fixed_payouts(insurance.fixed_payouts),
        rof_triggers(insurance.rof_triggers),
        utilities_revenue_last_year(
                vector<double>((unsigned long) n_utilities, 0.)),
        utilities_revenue_update(
                vector<double>((unsigned long) n_utilities, 0.)),
        insurance_price(vector<double>((unsigned long) n_utilities)),
        total_simulation_time(insurance.total_simulation_time) {
    utilities_ids = insurance.utilities_ids;
}

InsuranceStorageToROF::~InsuranceStorageToROF() = default;


void InsuranceStorageToROF::applyPolicy(int week) {
    int week_of_year = Utils::weekOfTheYear(week);

    /// Update utilities year revenue.
    for (int u = 0; u < continuity_utilities.size(); ++u) {
        utilities_revenue_update[u] +=
                DroughtMitigationPolicy::realization_utilities[u]->getGrossRevenue();
    }

    /// If first week of the year, price insurance for coming year and update
    /// gross revenue to base payouts.
    if (Utils::isFirstWeekOfTheYear(week + 1)) {
        utilities_revenue_last_year = utilities_revenue_update;
        utilities_revenue_update = vector<double>((unsigned long) n_utilities,
                0.);

        priceInsurance(week_of_year + 1);
    }

    /// Do not make payouts during the first year, when no insurance was purchased.
    if (week > WEEKS_IN_YEAR) {

        /// Get ROFs from table.
        vector<double> utilities_rof(DroughtMitigationPolicy::realization_utilities.size(), 0.);
        vector<double> utilities_storage_capa_ratio(DroughtMitigationPolicy::realization_utilities.size(), 0.0);

        for (int u = 0; u < DroughtMitigationPolicy::realization_utilities.size(); ++u) {
            utilities_storage_capa_ratio[u] =
                    DroughtMitigationPolicy::realization_utilities[u]->getStorageToCapacityRatio();
        }
        getUtilitiesApproxROFs(utilities_storage_capa_ratio,
                               week,
                               utilities_rof);

        /// Make payouts, if needed.
        for (int u = 0; u < continuity_utilities.size(); ++u)
            if (utilities_rof[u] > rof_triggers[u])
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(
                        fixed_payouts[u] * utilities_revenue_last_year[u]);
            else
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(NONE);
    }
}

void InsuranceStorageToROF::addSystemComponents(vector<Utility *> utilities,
                                                vector<WaterSource *> water_sources,
                                                vector<MinEnvFlowControl *> min_env_flow_controls) {
    DroughtMitigationPolicy::realization_utilities = vector<Utility *>(utilities.size());
    for (int i : utilities_ids)
        DroughtMitigationPolicy::realization_utilities[i] = utilities[i];

    connectRealizationWaterSources(water_sources);
}

/**
 * Runs a ROF set of 50 year long simulations in order to estimate how likely payouts are expected
 * to occur. The price of the insurance is set as the average sum of payouts across all 50 years
 * times the insurance premium.
 * @param week
 */
void InsuranceStorageToROF::priceInsurance(int week) {

    /// Reset prices.
    for (int u : utilities_ids) insurance_price[u] = 0;

    /// checks if new infrastructure became available and, if so, set the corresponding realization
    /// infrastructure online.
    updateOnlineInfrastructure(week);

    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        beginning_tier = NO_OF_INSURANCE_STORAGE_TIERS;
        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = week; w < week + WEEKS_IN_YEAR; ++w) {
            /// one week continuity time-step.
            continuityStep(w, r);

            /// Get utilities' approximate rof from storage-rof-table.
            vector<double> utilities_storage_capacity_ratio((unsigned) n_utilities);
//                    UtilitiesStorageCapacityRatio();
            for (int u = 0; u < n_utilities; ++u) {
                utilities_storage_capacity_ratio[u] =
                        this->continuity_utilities[u]->getStorageToCapacityRatio();
            }
            vector<double> utilities_rofs((unsigned long) n_utilities);
            getUtilitiesApproxROFs(utilities_storage_capacity_ratio,
                                   week - (int) WEEKS_IN_YEAR,
                                   utilities_rofs);
            /// Increase the price of the insurance if payout is triggered.
            const vector<int> uids = utilities_ids;
            for (const int &u : uids) {
                if (utilities_rofs[u] > rof_triggers[u]) {
                    insurance_price[u] +=
                            fixed_payouts[u] * utilities_revenue_last_year[u] *
                            insurance_premium;
                }
            }
        }
    }

    /// Average out insurance price across realizations
    for (int u : utilities_ids) {
        insurance_price[u] /= NUMBER_REALIZATIONS_ROF;
        DroughtMitigationPolicy::realization_utilities[u]->
                purchaseInsurance(insurance_price[u]);
    }
}

/**
 *
 * @param u_storage_capacity_ratio utilities' current storage to capacity ratio.
 * @param storage_to_rof_table table correlating storage capacities to rof
 * @param week week of the year.
 * @return
 */
void InsuranceStorageToROF::getUtilitiesApproxROFs(
        const vector<double> &u_storage_capacity_ratio,
        int week, vector<double>& utilities_approx_rof) {

    for (int u = 0; u < n_utilities; ++u) {
        /// get storage index in the table corresponding to the utility's combined storage.
        auto s = (int) floor(u_storage_capacity_ratio[u] *
                            NO_OF_INSURANCE_STORAGE_TIERS);
        s = min(s, NO_OF_INSURANCE_STORAGE_TIERS - 1);

        /// get estimated rof value from the table.
        if (s == NO_OF_INSURANCE_STORAGE_TIERS - 1)
            utilities_approx_rof[u] = storage_to_rof_table_[u](week, s);
        else
            utilities_approx_rof[u] =
                    (storage_to_rof_table_[u](week, s) +
                     storage_to_rof_table_[u](week, s + 1)) / 2.;
    }
}

/**
 * Calculation of storage-capacity ratio within the rof model for insurance price.
 * @return
 */
vector<double> InsuranceStorageToROF::UtilitiesStorageCapacityRatio() {

    auto n_utilities = continuity_utilities.size();
    vector<double> u_storage_capacity_ratio(n_utilities);
    for (unsigned long u = 0; u < n_utilities; ++u)
        u_storage_capacity_ratio[u] =
                continuity_utilities[u]->getStorageToCapacityRatio();

    return u_storage_capacity_ratio;
}

void InsuranceStorageToROF::setRealization(unsigned int realization_id, vector<double> &utilities_rdm,
                                           vector<double> &water_sources_rdm, vector<double> &policy_rdm) {
    ContinuityModel::setRealization(realization_id, utilities_rdm, water_sources_rdm);
}
