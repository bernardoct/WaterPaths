//
// Created by bernardoct on 5/1/17.
//

#include "InsuranceStorageToROF.h"
#include "../Utils/Utils.h"


InsuranceStorageToROF::InsuranceStorageToROF(const int id, const vector<WaterSource *> &water_sources,
                                             const Graph &water_sources_graph,
                                             const vector<vector<int>> &water_sources_to_utilities,
                                             const vector<Utility *> &utilities, vector<int> insured_utilities_ids,
                                             double *rof_triggers,
                                             const double insurance_premium, const double *fixed_payouts)
        : DroughtMitigationPolicy(id, INSURANCE_STORAGE_ROF),
          ContinuityModelROF(Utils::copyWaterSourceVector(water_sources),
                             water_sources_graph,
                             water_sources_to_utilities,
                             Utils::copyUtilityVector(utilities, true),
                             realization_id),
          insurance_premium(insurance_premium),
          fixed_payouts(fixed_payouts),
          rof_triggers(rof_triggers) {

    utilities_ids = insured_utilities_ids;

    for (Utility *u : continuity_utilities) {
        u->clearWaterSources();
    }

}

InsuranceStorageToROF::InsuranceStorageToROF(const InsuranceStorageToROF &insurance) :
        DroughtMitigationPolicy(insurance.id, insurance.type),
        ContinuityModelROF(Utils::copyWaterSourceVector(insurance.continuity_water_sources),
                           insurance.water_sources_graph,
                           insurance.water_sources_to_utilities,
                           Utils::copyUtilityVector(insurance.continuity_utilities, true),
                           insurance.realization_id),
        insurance_premium(insurance.insurance_premium),
        fixed_payouts(insurance.fixed_payouts),
        rof_triggers(insurance.rof_triggers) {

    utilities_ids = insurance.utilities_ids;
}

InsuranceStorageToROF::~InsuranceStorageToROF() {
}

void InsuranceStorageToROF::applyPolicy(int week) {
    if (Utils::isFirstWeekOfTheYear(week)) {
        priceInsurance(Utils::weekOfTheYear(week));
    }

    //FIXME: REMOVE THIS PRINT WHEN DONE DEBUGGING.
    for (int u = 0; u < continuity_utilities.size(); ++u) {
        if (realization_utilities[u]->getRisk_of_failure() > rof_triggers[u]) {
            realization_utilities[u]->addToContingencyFund(fixed_payouts[u]);
            cout << "Week " << week << ": Utility " << u << " paid." << endl;
        }
    }
}

void InsuranceStorageToROF::addSystemComponents(vector<Utility *> utilities, vector<WaterSource *> water_sources) {
    realization_utilities = vector<Utility *>(utilities.size());
    for (int i : utilities_ids)
        realization_utilities[i] = utilities[i];

    setRealization_water_sources(water_sources);
}

void InsuranceStorageToROF::priceInsurance(int week) {

    int n_utilities = (int) realization_utilities.size();

    /// Reset prices.
    if (insurance_price) delete[] insurance_price;
    insurance_price = new double[n_utilities];
    for (int u : utilities_ids) insurance_price[u] = 0;

    /// checks if new infrastructure became available and, if so, set the corresponding realization
    /// infrastructure online.
    updateOnlineInfrastructure(week);

    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        beginning_res_level = NO_OF_INSURANCE_STORAGE_TIERS;
        /// reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        /// have in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);

        for (int w = week; w < week + WEEKS_IN_YEAR; ++w) {

            /// one week continuity time-step.
            continuityStep(w, r);

            /// Get utilities' approximate rof from storage-rof-table.
            double utilities_rofs[n_utilities];
            getUtilitiesApproxROFs(UtilitiesStorageCapacityRatio(), storage_to_rof_table_, week, utilities_rofs);

            for (int u : utilities_ids)
                if (utilities_rofs[u] > rof_triggers[u])
                    insurance_price[u] += fixed_payouts[u] * insurance_premium;
        }
    }

    /// Average out insurance price across realizations
    for (int u : utilities_ids) {
        insurance_price[u] /= NUMBER_REALIZATIONS_ROF;
        cout << insurance_price[u] << " ";
    }
    cout << endl;
}

/**
 *
 * @param u_storage_capacity_ratio
 * @param storage_to_rof_table
 * @param week
 * @return
 */
void InsuranceStorageToROF::getUtilitiesApproxROFs(double *u_storage_capacity_ratio,
                                                   const Matrix3D<double> *storage_to_rof_table,
                                                   int week, double *utilities_approx_rof) {
    int n_utilities = (int) continuity_utilities.size();

    for (int u = 0; u < n_utilities; ++u) {
        double ss = floor(u_storage_capacity_ratio[u] * NO_OF_INSURANCE_STORAGE_TIERS) - 1;
        int s = (int) ss;

        utilities_approx_rof[u] = ((*storage_to_rof_table)(u, s, week) + (*storage_to_rof_table)(u, s - 1, week)) / 2;
    }
}

double *InsuranceStorageToROF::UtilitiesStorageCapacityRatio() {

    int n_utilities = (int) realization_utilities.size();
    double *u_storage_capacity_ratio = new double[n_utilities];
    for (int u = 0; u < n_utilities; ++u)
        u_storage_capacity_ratio[u] = realization_utilities[u]->getStorageToCapacityRatio();

    return u_storage_capacity_ratio;
}
