//
// Created by bernardoct on 8/25/17.
//

#include <numeric>
#include <algorithm>
#include "ObjectivesCalculator.h"
#include "Utils.h"

double ObjectivesCalculator::calculateReliabilityObjective(
        vector<UtilitiesDataCollector *> utility_collector) {
    unsigned long n_realizations = utility_collector.size();
    unsigned long n_weeks = utility_collector[0]->getCombined_storage().size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    vector<vector<int>> realizations_year_reliabilities(n_realizations,
                                                        vector<int>(n_years,
                                                                    0));
    vector<int> year_reliabilities(n_years,
                                   0);

    /// Creates a table with years that failed in each realization.
    for (int r = 0; r < n_realizations; ++r) {
        for (int y = 0; y < n_years; ++y) {
            for (int w = (int) round(y * WEEKS_IN_YEAR); w < min((int) n_weeks,
                                                                 (int) round(
                                                                         (y +
                                                                          1) *
                                                                         WEEKS_IN_YEAR));
                 ++w) {
                if (utility_collector[r]->getCombined_storage()[w] /
                    utility_collector[r]->getCapacity()[w] <
                    STORAGE_CAPACITY_RATIO_FAIL) {
                    realizations_year_reliabilities[r][y] = FAILURE;
                }
            }
        }
    }

    /// Creates a vector with the number of realizations that failed for each year.
    for (int y = 0; y < n_years; ++y) {
        for (int r = 0; r < n_realizations; ++r) {
            if (realizations_year_reliabilities[r][y] ==
                FAILURE)
                year_reliabilities[y]++;
        }
    }

    double check_non_zero = accumulate(year_reliabilities.rbegin(),
                                       year_reliabilities.rend(),
                                       0.0);

    /// Returns year with most realization failures, divided by the number of realizations (reliability objective).
    if (check_non_zero > 0)
        return 1. - (double) *max_element(year_reliabilities.begin(),
                                          year_reliabilities.end()) /
                    n_realizations;
    else
        return 1.;
}

double ObjectivesCalculator::calculateRestrictionFrequencyObjective(
        vector<RestrictionsDataCollector *> restriction_data) {

    unsigned long n_realizations = restriction_data.size();

    /// Check if there were restriction policies in place.
    if (!restriction_data.empty()) {
        unsigned long n_weeks = restriction_data[0]
                ->getRestriction_multipliers().size();
        unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

        vector<vector<double>> year_restriction_frequencies(n_realizations,
                                                            vector<double>(n_years,
                                                                           0));
        double restriction_frequency = 0;

        /// Counts how many years across all realizations had restrictions.
        for (int r = 0; r < n_realizations; ++r) {
            for (int y = 0; y < n_years; ++y) {
                for (int w = (int) round(y * WEEKS_IN_YEAR);
                     w < min((int) n_weeks,
                             (int) round((y + 1) * WEEKS_IN_YEAR)); ++w) {
                    if (restriction_data[r]->getRestriction_multipliers()[w] !=
                        1.0) {
                        restriction_frequency++;
                        break;
                    }
                }
            }
        }

        return restriction_frequency / (n_realizations * n_years);
    } else
        return NONE;
}

double ObjectivesCalculator::calculateNetPresentCostInfrastructureObjective(
        vector<UtilitiesDataCollector *> utility_data) {

    double infrastructure_npc = 0;
    for (auto r : utility_data)
        infrastructure_npc += accumulate(
                r->getNet_present_infrastructure_cost().begin(),
                r->getNet_present_infrastructure_cost().end(),
                0.0);

    return infrastructure_npc / utility_data.size();
}

double ObjectivesCalculator::calculatePeakFinancialCostsObjective(
        vector<UtilitiesDataCollector *> utility_data) {
    unsigned long n_realizations = utility_data.size();
    unsigned long n_weeks = utility_data[0]->getGross_revenues().size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    double realizations_year_debt_payment = 0;
    double realizations_year_cont_fund_contribution = 0;
    double realizations_year_gross_revenue = 0;
    double realizations_year_insurance_contract_cost = 0;
    vector<double> year_financial_costs;
    vector<double> realization_financial_costs(n_realizations,
                                               0);

    /// Creates a table with years that failed in each realization.
    int y;
    for (int r = 0; r < n_realizations; ++r) {
        year_financial_costs.assign(n_years,
                                    0.0);
        y = 0;
        for (int w = 0; w < n_weeks; ++w) {
            /// accumulate year's info by summing weekly amounts.
            realizations_year_debt_payment +=
                    utility_data[r]->getDebt_service_payments()[w];
            realizations_year_cont_fund_contribution +=
                    utility_data[r]->getContingency_fund_contribution()[w];
            realizations_year_gross_revenue +=
                    utility_data[r]->getGross_revenues()[w];
            realizations_year_insurance_contract_cost +=
                    utility_data[r]->getInsurance_contract_cost()[w];

            /// if last week of the year, close the books and calculate
            /// financial cost for the year.
            if (Utils::isFirstWeekOfTheYear(w + 1)) {
                year_financial_costs[y] +=
                        (realizations_year_debt_payment +
                         realizations_year_cont_fund_contribution +
                         realizations_year_insurance_contract_cost) /
                        realizations_year_gross_revenue;
                /// update year count.
                y++;

                /// reset accounts.
                realizations_year_debt_payment = 0;
                realizations_year_cont_fund_contribution = 0;
                realizations_year_gross_revenue = 0;
                realizations_year_insurance_contract_cost = 0;
            }
        }
        /// store highest year cost as the cost financial cost of the realization.
        realization_financial_costs[r] =
                *max_element(year_financial_costs.begin(),
                             year_financial_costs.end());
    }

    /// returns average of realizations' costs.
    return accumulate(realization_financial_costs.begin(),
                      realization_financial_costs.end(),
                      0.0) / n_realizations;
}

double ObjectivesCalculator::calculateWorseCaseCostsObjective(
        vector<UtilitiesDataCollector *> utility_data) {
    unsigned long n_realizations = utility_data.size();
    unsigned long n_weeks = utility_data[0]->getGross_revenues().size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    vector<double> worse_year_financial_costs(n_realizations,
                                              0);
    vector<double> year_financial_costs;
    double year_drought_mitigation_cost = 0;
    double year_gross_revenue = 0;

    /// Creates a table with years that failed in each realization.
    int y;
    for (int r = 0; r < n_realizations; ++r) {
        y = 0;
        year_financial_costs.assign(n_years,
                                    0);
        for (int w = 0; w < n_weeks; ++w) {
            /// accumulate year's info by summing weekly amounts.
            year_drought_mitigation_cost +=
                    utility_data[r]->getDrought_mitigation_cost()[w];
            year_gross_revenue += utility_data[r]->getGross_revenues()[w];

            /// if last week of the year, close the books and calculate financial cost for the year.
            if (Utils::isFirstWeekOfTheYear(w + 1)) {
                year_financial_costs[y] =
                        max(year_drought_mitigation_cost
                            - utility_data[r]->getContingency_fund_size()[w],
                            0.0)
                        / year_gross_revenue;

                year_gross_revenue = 0;
                year_drought_mitigation_cost = 0;
                /// update year count.
                y++;
            }
        }
        /// store highest year cost as the drought mitigation cost of the realization.
        worse_year_financial_costs[r] = *max_element(
                year_financial_costs.begin(),
                year_financial_costs.end());
    }

    /// sort costs to get the worse 1 percentile.
    sort(worse_year_financial_costs.begin(),
         worse_year_financial_costs.end());

    return worse_year_financial_costs.at(
            (unsigned long) floor(WORSE_CASE_COST_PERCENTILE * n_realizations));
}