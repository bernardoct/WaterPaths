//
// Created by bernardoct on 6/26/17.
//

#include <algorithm>
#include "WwtpDischargeRule.h"
#include "../Utils/Constants.h"



WwtpDischargeRule::WwtpDischargeRule() = default;

WwtpDischargeRule::WwtpDischargeRule(
        vector<vector<double>> year_series_fraction_discharge,
        vector<int> discharge_to_source_ids) :
        year_series_fraction_discharge(year_series_fraction_discharge),
        discharge_to_source_ids(discharge_to_source_ids) {

    if (!discharge_to_source_ids.empty()) {
        int length = *max_element(discharge_to_source_ids.begin(),
                                  discharge_to_source_ids.end()) + 1;
        source_id_to_vector_index = vector<int>(
                (unsigned long) length,
                Constants::NON_INITIALIZED);
        for (unsigned long i = 0; i < discharge_to_source_ids.size(); ++i)
            source_id_to_vector_index[discharge_to_source_ids.at(
                    (unsigned long) i)] = i;
    }

    for (auto &f : year_series_fraction_discharge) {
        if ((int) f.size() != (int) Constants::WEEKS_IN_YEAR + 1)
            throw invalid_argument("Wastewater discharge rules but contain "
                                             "exactly 53 (for years with "
                                             "extra week) values.");
    }
    if (year_series_fraction_discharge.size() != discharge_to_source_ids.size()) {
        printf("Number of series: %lu\nNumber of source IDs: %lu\n", 
            year_series_fraction_discharge.size(),
            discharge_to_source_ids.size());
        throw invalid_argument("Number of wwtp discharge time series must be "
                                          "the same as number of sources ids.");
    }
}

WwtpDischargeRule::WwtpDischargeRule(WwtpDischargeRule &wwtp_discharge_rule) :
	source_id_to_vector_index(
                wwtp_discharge_rule.source_id_to_vector_index),
        year_series_fraction_discharge(
                wwtp_discharge_rule.year_series_fraction_discharge),
        discharge_to_source_ids(wwtp_discharge_rule.discharge_to_source_ids) {}

WwtpDischargeRule &WwtpDischargeRule::operator=(
        const WwtpDischargeRule &wwtp_discharge_rule) {
    discharge_to_source_ids =
            wwtp_discharge_rule.discharge_to_source_ids;
    year_series_fraction_discharge =
            wwtp_discharge_rule.year_series_fraction_discharge;
    source_id_to_vector_index = wwtp_discharge_rule.source_id_to_vector_index;
    return *this;
}

/**
 * Returns the fraction of week's demand to be discharged as effluent.
 * @param water_source_id ID of source receiving the effluent.
 * @return fraction of demand to be discharged.
 */
double WwtpDischargeRule::get_dependent_variable(int water_source_id, int week) {
    return year_series_fraction_discharge[source_id_to_vector_index[water_source_id]][week];
}

double WwtpDischargeRule::get_dependent_variable(double x, int week) {
    throw_with_nested(invalid_argument("WWTP discharge rules need a water source ID (int)"
                                     " and week number (int) to return the "
                                     "fraction of total_demand discharged to that "
                                     "source."));
}

double WwtpDischargeRule::get_dependent_variable(double x) {
    throw_with_nested(invalid_argument("WWTP discharge rules need a water source ID (int)"
                                     " and week number (int) to return the "
                                     "fraction of total_demand discharged to that "
                                     "source."));
}

double WwtpDischargeRule::get_dependent_variable(int x) {
    throw_with_nested(invalid_argument("WWTP discharge rules need a water source ID (int)"
                                     " and week number (int) to return the "
                                     "fraction of total_demand discharged to that "
                                     "source."));
}
