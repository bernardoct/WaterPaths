//
// Created by bernardoct on 5/1/17.
//

#include <iostream>
#include "InsuranceStorageToROF.h"
#include "../Utils/Utils.h"

InsuranceStorageToROF::InsuranceStorageToROF(
        const int id,
        vector<WaterSource *> &water_sources,
        const Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        vector<MinEnvironFlowControl *>
        min_env_flow_controls,
        double *rof_triggers,
        const double insurance_premium,
        const double *fixed_payouts)
        : DroughtMitigationPolicy(id, INSURANCE_STORAGE_ROF),
          ContinuityModelROF(Utils::copyWaterSourceVector(water_sources),
                             water_sources_graph,
                             water_sources_to_utilities,
                             Utils::copyUtilityVector(utilities, true),
                             min_env_flow_controls,
                             NON_INITIALIZED),
          insurance_premium(insurance_premium), fixed_payouts(fixed_payouts),
          rof_triggers(rof_triggers),
          utilities_revenue_last_year(new double[utilities.size()]()),
          utilities_revenue_update(new double[utilities.size()]()) {

    for (Utility *u : utilities) utilities_ids.push_back(u->id);

    for (Utility *u : continuity_utilities) {
        u->clearWaterSources();
    }
}

InsuranceStorageToROF::InsuranceStorageToROF(
        InsuranceStorageToROF &insurance) :
        DroughtMitigationPolicy(insurance.id, insurance.type),
        ContinuityModelROF(Utils::copyWaterSourceVector(
                insurance.continuity_water_sources),
                           insurance.water_sources_graph,
                           insurance.water_sources_to_utilities,
                           Utils::copyUtilityVector(
                                   insurance.continuity_utilities, true),
                           insurance.min_env_flow_controls,
                           insurance.realization_id),
        insurance_premium(insurance.insurance_premium),
        fixed_payouts(insurance.fixed_payouts),
        rof_triggers(insurance.rof_triggers),
        utilities_revenue_last_year(
                new double[insurance.continuity_utilities.size()]()),
        utilities_revenue_update(
                new double[insurance.continuity_utilities.size()]()) {

    utilities_ids = insurance.utilities_ids;
}

InsuranceStorageToROF::~InsuranceStorageToROF() = default;

void InsuranceStorageToROF::applyPolicy(int week) {
    int week_of_year = Utils::weekOfTheYear(week);

    /// Update utilities year revenue.
    for (int u = 0; u < continuity_utilities.size(); ++u) {
        utilities_revenue_update[u] +=
                realization_utilities[u]->getGrossRevenue();
    }

    /// If first week of the year, price insurance for coming year and update
    /// gross revenue to base payouts.
    if (Utils::isFirstWeekOfTheYear(week + 1)) {
        delete utilities_revenue_last_year;
        utilities_revenue_last_year = utilities_revenue_update;
        utilities_revenue_update = new double[continuity_utilities.size()]();

        priceInsurance(week_of_year);
        storage_to_rof_table_prev_year = Matrix3D<double>
                (*storage_to_rof_table_);
    }

    /// Do not make payouts during the first year, when no insurance was purchased.
    if (week > WEEKS_IN_YEAR) {

        /// Get ROFs from table.
        double utilities_rof[realization_utilities.size()] = {};
        double utilities_storage_capa_ratio[realization_utilities.size()] = {};
        for (int u = 0; u < realization_utilities.size(); ++u) {
            utilities_storage_capa_ratio[u] =
                    realization_utilities[u]->getStorageToCapacityRatio();
        }
        getUtilitiesApproxROFs(utilities_storage_capa_ratio,
                               &storage_to_rof_table_prev_year,
                               week_of_year,
                               utilities_rof);

        //FIXME: FOR DEBUGGING.
//        cout << week << " " << week_of_year << endl;
//        storage_to_rof_table_prev_year.print(week_of_year);
//        cout << endl;

        /// Make payouts, if needed.
        for (int u = 0; u < continuity_utilities.size(); ++u) {
            if (utilities_rof[u] >
                rof_triggers[u]) {
                realization_utilities[u]->addInsurancePayout(
                        fixed_payouts[u] * utilities_revenue_last_year[u]);
            }
        }
    }
}

void InsuranceStorageToROF::addSystemComponents(vector<Utility *> utilities,
                                                vector<WaterSource *> water_sources,
                                                vector<MinEnvironFlowControl *> min_env_flow_controls) {
    realization_utilities = vector<Utility *>(utilities.size());
    for (int i : utilities_ids)
        realization_utilities[i] = utilities[i];

    setRealization_water_sources(water_sources);
}

/**
 * Runs a ROF set of 50 year long simulations in order to estimate how likely payouts are expected
 * to occur. The price of the insurance is set as the average sum of payouts across all 50 years
 * times the insurance premium.
 * @param week
 */
void InsuranceStorageToROF::priceInsurance(int week) {

    int n_utilities = (int) realization_utilities.size();

    /// Reset prices.
    if (week > WEEKS_IN_YEAR) delete[] insurance_price;
    insurance_price = new double[n_utilities];
    for (int u : utilities_ids) insurance_price[u] = 0;

    /// checks if new infrastructure became available and, if so, set the corresponding realization
    /// infrastructure online.
    updateOnlineInfrastructure(week);

    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        beginning_res_level = NO_OF_STORAGE_TO_ROF_TABLE_TIERS;
        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = week; w < week + WEEKS_IN_YEAR; ++w) {
            /// one week continuity time-step.
            continuityStep(w, r);

            /// Get utilities' approximate rof from storage-rof-table.
            double utilities_rofs[n_utilities];
            getUtilitiesApproxROFs(UtilitiesStorageCapacityRatio(),
                                   storage_to_rof_table_,
                                   Utils::weekOfTheYear(w), utilities_rofs);

            /// Increase the price of the insurance if payout is triggered.
            for (int u : utilities_ids)
                if (utilities_rofs[u] > rof_triggers[u])
                    insurance_price[u] +=
                            fixed_payouts[u] * utilities_revenue_last_year[u] *
                            insurance_premium;
        }
    }

    /// Average out insurance price across realizations
    for (int u : utilities_ids) {
        insurance_price[u] /= NUMBER_REALIZATIONS_ROF;
        realization_utilities[u]->purchaseInsurance(insurance_price[u]);
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
        double *u_storage_capacity_ratio,
        const Matrix3D<double> *storage_to_rof_table,
        int week, double *utilities_approx_rof) {
    int n_utilities = storage_to_rof_table->get_i();

    for (int u = 0; u < n_utilities; ++u) {
        /// get storage index in the table corresponding to the utility's combined storage.
        int s = (int) floor(u_storage_capacity_ratio[u] *
                                    NO_OF_STORAGE_TO_ROF_TABLE_TIERS);
        s = min(s, NO_OF_STORAGE_TO_ROF_TABLE_TIERS - 1);

        /// get estimated rof value from the table.
        if (s == NO_OF_STORAGE_TO_ROF_TABLE_TIERS - 1)
            utilities_approx_rof[u] = (*storage_to_rof_table)(u, s, week);
        else
            utilities_approx_rof[u] =
                    ((*storage_to_rof_table)(u, s, week) +
                     (*storage_to_rof_table)(u, s + 1, week)) / 2.;
    }
}

/**
 * Calculation of storage-capacity ratio within the rof model for insurance price.
 * @return
 */
double *InsuranceStorageToROF::UtilitiesStorageCapacityRatio() {

    int n_utilities = (int) continuity_utilities.size();
    double *u_storage_capacity_ratio = new double[n_utilities];
    for (int u = 0; u < n_utilities; ++u)
        u_storage_capacity_ratio[u] =
                continuity_utilities[u]->getStorageToCapacityRatio();

    return u_storage_capacity_ratio;
}

void InsuranceStorageToROF::setRealization(unsigned int realization_id) {
    ContinuityModel::setRealization(realization_id);
}
