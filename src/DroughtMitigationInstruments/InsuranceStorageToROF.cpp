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
                                             vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                                             vector<MinEnvFlowControl *> min_env_flow_controls,
                                             vector<vector<double>>& utilities_rdm,
                                             vector<vector<double>>& water_sources_rdm,
                                             vector<vector<double>>& policy_rdm, vector<double> &rof_triggers,
                                             const double insurance_premium, const vector<double> &fixed_payouts,
                                             unsigned long total_simulation_time)
        : DroughtMitigationPolicy(id, INSURANCE_STORAGE_ROF),
          ContinuityModelROF(Utils::copyWaterSourceVector(water_sources), water_sources_graph,
                             water_sources_to_utilities, Utils::copyUtilityVector(utilities, true),
                             Utils::copyMinEnvFlowControlVector(min_env_flow_controls), utilities_rdm.at(0),
                             water_sources_rdm.at(0), total_simulation_time, false, (unsigned int) NON_INITIALIZED),
          rof_triggers(rof_triggers),
          total_simulation_time(total_simulation_time),
          insurance_premium(insurance_premium),
          insurance_price(NONE),
          fixed_payouts(fixed_payouts),
          utilities_revenue_update(vector<double>((unsigned long) n_utilities, 0.)),
          utilities_revenue_last_year(vector<double>((unsigned long) n_utilities, 0.)),
          drought_mitigation_policies(Utils::copyDroughtMitigationPolicyVector(drought_mitigation_policies))
{

    for (Utility *u : utilities) utilities_ids.push_back(u->id);

    for (Utility *u : continuity_utilities) {
        u->clearWaterSources();
        u->resetTotal_storage_capacity();
    }

    // Remove financial instruments, if any.
    auto it = this->drought_mitigation_policies.begin();
    while(it != this->drought_mitigation_policies.end()) {
        if((*it)->type > SUPPLY_INSTRUMENT_LIM) {
            it = this->drought_mitigation_policies.erase(it);
        }
        else ++it;
    }

    insurance_price = vector<double>((unsigned long) n_utilities, 0.);
    payout_multiplier = vector<double>((unsigned long) n_utilities, 1.);
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
        rof_triggers(insurance.rof_triggers),
        total_simulation_time(insurance.total_simulation_time),
        insurance_premium(insurance.insurance_premium),
        insurance_price(insurance.insurance_price),
        payout_multiplier(insurance.payout_multiplier),
        fixed_payouts(insurance.fixed_payouts),
        utilities_revenue_update(vector<double>((unsigned long) n_utilities, 0.)),
        utilities_revenue_last_year(vector<double>((unsigned long) n_utilities, 0.)),
        drought_mitigation_policies(Utils::copyDroughtMitigationPolicyVector(insurance.drought_mitigation_policies)) {
    utilities_ids = insurance.utilities_ids;
}

InsuranceStorageToROF::~InsuranceStorageToROF() {
    // Delete drought mitigation policies.
    for (auto dmp : drought_mitigation_policies) {
        delete dmp;
    }
}


void InsuranceStorageToROF::applyPolicy(int week) {
    // Update utilities year revenue.
    for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
        utilities_revenue_update[u] +=
                DroughtMitigationPolicy::realization_utilities[u]->getGrossRevenue();
    }

    // If first week of the year, price insurance for coming year and update
    // gross revenue to base payouts.
    if (Utils::isFirstWeekOfTheYear(week + 1)) {
        utilities_revenue_last_year = utilities_revenue_update;
        std::fill(utilities_revenue_update.begin(), utilities_revenue_update.end(), NONE);

        priceInsurance(week);
    }

    // Do not make payouts during the first year, when no insurance was purchased.
    if (week > WEEKS_IN_YEAR) {
        vector<double> utilities_rof(DroughtMitigationPolicy::realization_utilities.size(), NONE);

        // Get utilities ROFs
        for (unsigned long u = 0; u < DroughtMitigationPolicy::realization_utilities.size(); ++u) {
            utilities_rof[u] = DroughtMitigationPolicy::realization_utilities[u]->getRisk_of_failure();
        }

        // Make payouts, if needed.
        for (unsigned long u = 0; u < continuity_utilities.size(); ++u)
            if (utilities_rof[u] > rof_triggers[u])
                // payout multiplier adjusts for partially paid insurance price due to insufficient contingency fund.
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(
                        fixed_payouts[u] * utilities_revenue_last_year[u] * payout_multiplier[u]);
            else
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(NONE);
    }
}

void InsuranceStorageToROF::addSystemComponents(vector<Utility *> utilities,
                                                vector<WaterSource *> water_sources,
                                                vector<MinEnvFlowControl *> min_env_flow_controls) {
    DroughtMitigationPolicy::realization_utilities = vector<Utility *>(utilities.size());
    for (int i : utilities_ids) {
        DroughtMitigationPolicy::realization_utilities[i] = utilities[i];
    }

    for (Utility *u : utilities) {
        utility_base_storage_capacity.push_back(u->getTotal_storage_capacity());
    }
    current_storage_table_shift = vector<double>(utilities.size());
    current_base_storage_table_shift = vector<double>(utilities.size());

    connectRealizationWaterSources(water_sources);
}

/**
 * Runs a ROF set of 50 year long simulations in order to estimate how likely payouts are expected
 * to occur. The price of the insurance is set as the average sum of payouts across all 50 years
 * times the insurance premium.
 * @param week
 */
void InsuranceStorageToROF::priceInsurance(int week) {
    // Reset prices.
    for (int u : utilities_ids) insurance_price[u] = 0;

    // checks if new infrastructure became available and, if so, set the corresponding realization
    // infrastructure online.
    updateOnlineInfrastructure(week);

    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        // reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        // have in the corresponding realization simulation.
        resetUtilitiesAndReservoirs(SHORT_TERM_ROF);// CHECK IF NEED TO OVERWRITE THIS FUNCTION AND RESET RESTRICTION AND TRANSFER VOLUMES

        for (int w = week - (int) WEEKS_IN_YEAR + 1; w <= week; ++w) {
            // one week continuity time-step.
            continuityStep(w, r);

            // Get utilities' approximate rof from storage-rof-table.
            auto utilities_rofs = InsuranceStorageToROF::calculateShortTermROFTable(
                        w, continuity_utilities, n_utilities);

            for (int u = 0; u < continuity_utilities.size(); ++u) {
                continuity_utilities[u]->setRisk_of_failure(utilities_rofs[u]);
            }

            // apply supply drought mitigation instruments.
            for (DroughtMitigationPolicy* dmp : drought_mitigation_policies) {
                dmp->applyPolicy(week);
            }

            // Increase the price of the insurance if payout is triggered and reset dmp utility-variables.
            for (const int &u : utilities_ids) {
                if (utilities_rofs[u] > rof_triggers[u]) {
                    insurance_price[u] += fixed_payouts[u] *
                            utilities_revenue_last_year[u] * insurance_premium;
                }
                continuity_utilities[u]->resetDroughtMitigationVariables();
            }
        }
    }

    // Average out insurance price across realizations
    for (int u : utilities_ids) {
        insurance_price[u] /= NUMBER_REALIZATIONS_ROF;

        if (insurance_price[u] == 0) {
            payout_multiplier[u] = 0.;
        } else {
            payout_multiplier[u] = 1.;
        }
        DroughtMitigationPolicy::realization_utilities[u]->
                purchaseInsurance(insurance_price[u]);
    }
}

void InsuranceStorageToROF::setRealization(unsigned long realization_id, vector<double> &utilities_rdm,
                                           vector<double> &water_sources_rdm, vector<double> &policy_rdm) {
    ContinuityModel::setRealization(realization_id, utilities_rdm, water_sources_rdm);

    // Pass corresponding utilities to drought mitigation instruments.
    for (DroughtMitigationPolicy *dmp : this->drought_mitigation_policies) {
        dmp->addSystemComponents(continuity_utilities, continuity_water_sources, min_env_flow_controls);
        dmp->setRealization(realization_id, utilities_rdm, water_sources_rdm,
                            policy_rdm);
    }
}

/**
 * Runs one the full rof calculations for realization #realization_id for a
 * given week.
 * @param week for which rof is to be calculated.
 * @todo This is mostly a copy and paste from ContinuityModelROF.cpp, so
 * although I'm now on a fix this should be done in a better way at some point.
 */
vector<double> InsuranceStorageToROF::calculateShortTermROFTable(int week,
                                                              const vector<Utility *> &utilities,
                                                              const int &n_utilities) {
    // vector where risks of failure will be stored.
    vector<double> risk_of_failure((unsigned long) n_utilities, 0.0);
    double m;
    for (int u = 0; u < n_utilities; ++u) {
        // Get current stored volume for utility u.
        double utility_storage =
                utilities[u]->getTotal_stored_volume();
        // Ratio of current and status-quo utility storage capacities
        //        double m = current_and_base_storage_capacity_ratio[u];
        m = utilities[u]->getTotal_storage_capacity() /
                (utility_base_storage_capacity[u] + current_base_storage_table_shift[u]);
        // Calculate base table tier that contains the desired ROF by
        // shifting the table around based on new infrastructure -- the
        // shift is made by the part (m - 1) * STORAGE_CAPACITY_RATIO_FAIL *
        // utility_base_storage_capacity[u] - current_storage_table_shift[u]
        double storage_convert = utility_storage +
                                 STORAGE_CAPACITY_RATIO_FAIL *
                                 (utility_base_storage_capacity[u] + current_base_storage_table_shift[u]) *
                                 (1. - m) + current_storage_table_shift[u];
        int tier = (int) (storage_convert * NO_OF_INSURANCE_STORAGE_TIERS /
                         (utility_base_storage_capacity[u] + current_base_storage_table_shift[u]));

        //FIXME: NEED CURRENT_BASE_STORAGE_TABLE_SHIFT VALUES TO BE NON-ZERO, HOW DOES THIS CLASS COMMUNICATE
        //WITH OTHERS TO PASS THE VECTOR OF SHIFTS?
//        cout << "InsuranceROF: In week " << week << " for " << utilities[u]->name
//             << ", storage is " << utility_storage
//             << " while base storage is " << utility_base_storage_capacity[u]
//             << " and is shifted by " << current_base_storage_table_shift[u]
//             << " leading to a tier of " << tier << endl;

        if (tier > NO_OF_INSURANCE_STORAGE_TIERS - 2) {
            cout << "VIOLATION OF TIER IN INSURANCESTORAGETOROF" << endl;
            tier = NO_OF_INSURANCE_STORAGE_TIERS - 2;
        }

        // Mean ROF between the two tiers of the ROF table where
        // current storage is located.
//        risk_of_failure[u] = getRofFromRealizationTable(u, week, tier);
        risk_of_failure[u] = (getRofFromRealizationTable(u, week, tier) +
                getRofFromRealizationTable(u, week, min(NO_OF_INSURANCE_STORAGE_TIERS, tier + 1))) / 2;
    }

    return risk_of_failure;
}

void InsuranceStorageToROF::updateOnlineInfrastructure(int week) {
    ContinuityModelROF::updateOnlineInfrastructure(week);

    // Use capacity multiplier if using pre-computed tables (which are created with such multiplier)
    if (week < WEEKS_IN_YEAR + 1 && use_imported_tables) {
        for (double &u : utility_base_storage_capacity) {
            u *= BASE_STORAGE_CAPACITY_MULTIPLIER;
        }
    } else if (!use_imported_tables) {
        for (int u = 0; u < n_utilities; ++u) {
            utility_base_storage_capacity[u] = continuity_utilities[u]->getTotal_storage_capacity();
        }
    }
}

