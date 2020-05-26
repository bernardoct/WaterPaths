//
// Created by bernardo on 11/27/19.
//

#include "UtilityParser.h"
#include "../Base/WaterSourceParser.h"
#include "../../Utils/Utils.h"
#include "../Exceptions/MissingParameter.h"
#include "../Exceptions/InconsistentMutuallyImplicativeParameters.h"
#include "../AuxDataParser.h"
#include "../AuxParserFunctions.h"

UtilityParser::UtilityParser() = default;

UtilityParser::~UtilityParser() {
    demands_all_realizations = nullptr;
}

void UtilityParser::preProcessBlock(vector<vector<string>> &block, int l,
                                    const map<string, int> &ws_name_to_id) {

    AuxParserFunctions::replaceNameById(block, "[UTILITY]", l,
                                        "wwtp_discharge_rule", 2,
                                        ws_name_to_id);
    AuxParserFunctions::replaceNameById(block, "[UTILITY]", l,
                                        "rof_infra_construction_order", 1,
                                        ws_name_to_id);
    AuxParserFunctions::replaceNameById(block, "[UTILITY]", l,
                                        "demand_infra_construction_order", 1,
                                        ws_name_to_id);
}

void UtilityParser::parseVariables(vector<vector<string>> &block,
                                   int n_realizations,
                                   map<string, vector<vector<double>>> &pre_loaded_data) {

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> line = block[i];
        if (line[0] == "name") {
            name = line[1];
            rows_read.push_back(i);
        } else if (line[0] == "percent_contingency_fund_contribution") {
            percent_contingency_fund_contribution = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "demand_buffer") {
            demand_buffer = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "infra_discount_rate") {
            infra_discount_rate = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "number_of_week_demands") {
            number_of_week_demands = stoi(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "demand_infra_construction_order") {
            Utils::tokenizeString(line[1], demand_infra_construction_order,
                                  ',');
            rows_read.push_back(i);
        } else if (line[0] == "rof_infra_construction_order") {
            Utils::tokenizeString(line[1], rof_infra_construction_order, ',');
            rows_read.push_back(i);
        } else if (line[0] == "infra_construction_triggers") {
            Utils::tokenizeString(line[1], infra_construction_triggers, ',');
            rows_read.push_back(i);
        } else if (line[0] == "infra_if_built_remove") {
            for (auto n = line.begin() + 1; n != line.end(); ++n) {
                vector<int> iifbr;
                Utils::tokenizeString(*n, iifbr, ',');
                infra_if_built_remove.push_back(iifbr);
            }
            rows_read.push_back(i);
        } else if (line[0] == "construction_pre_requisites") {
            for (auto n = line.begin() + 1; n != line.end(); ++n) {
                vector<int> cpp;
                Utils::tokenizeString(*n, cpp, ',');
                construction_pre_requisites.push_back(cpp);
            }
            rows_read.push_back(i);
        } else if (line[0] == "typesMonthlyDemandFraction") {
            typesMonthlyDemandFraction = pre_loaded_data.at(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "typesMonthlyWaterPrice") {
            typesMonthlyWaterPrice = pre_loaded_data.at(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "demands") {
            demands_all_realizations = &pre_loaded_data.at(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "wwtp_discharge_rule") {
            vector<int> discharge_to_sources_ids;
            Utils::tokenizeString(line[2], discharge_to_sources_ids, ',');
            wwtp_discharge_rule = WwtpDischargeRule(
                    pre_loaded_data.at(line[1]),
                    discharge_to_sources_ids
            );
            rows_read.push_back(i);
        } else if (line[0] == "water_source_to_wtp") {
            for (auto n = line.begin() + 1; n != line.end(); ++n) {
                vector<int> wswtp;
                Utils::tokenizeString(*n, wswtp, ',');
                water_source_to_wtp.push_back(wswtp);
            }
            rows_read.push_back(i);
        } else if (line[0] == "utility_owned_wtp_capacities") {
            Utils::tokenizeString(line[1], utility_owned_wtp_capacities, ',');
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

Utility *
UtilityParser::generateUtility(int id, vector<vector<string>> &block,
                               int line_no, int n_realizations,
                               const map<string, int> &ws_name_to_id,
                               map<string, vector<vector<double>>> &pre_loaded_data) {
    preProcessBlock(block, line_no, ws_name_to_id);
    parseVariables(block, n_realizations, pre_loaded_data);

    AuxParserFunctions::checkForUnreadTags(line_no, block, tag_name);
    checkMissingOrExtraParams(block, line_no);

    if (infra_construction_triggers.empty()) {
        return new Utility(
                name, id, *demands_all_realizations,
                number_of_week_demands,
                percent_contingency_fund_contribution,
                typesMonthlyDemandFraction,
                typesMonthlyWaterPrice, wwtp_discharge_rule,
                demand_buffer, water_source_to_wtp,
                utility_owned_wtp_capacities
        );
    } else {
        return new Utility(
                name, id, *demands_all_realizations,
                number_of_week_demands,
                percent_contingency_fund_contribution,
                typesMonthlyDemandFraction,
                typesMonthlyWaterPrice, wwtp_discharge_rule,
                demand_buffer, water_source_to_wtp,
                utility_owned_wtp_capacities, rof_infra_construction_order,
                demand_infra_construction_order,
                infra_construction_triggers, infra_discount_rate,
                infra_if_built_remove
        );
    }
}


void UtilityParser::checkMissingOrExtraParams(vector<vector<string>> &block,
                                              int line_no) {
    if (name.empty()) {
        throw MissingParameter("name", tag_name, line_no);
    }
    if (demands_all_realizations->empty()) {
        throw MissingParameter("demands_all_realizations", tag_name, line_no);
    }
    if (number_of_week_demands == NON_INITIALIZED) {
        throw MissingParameter("number_of_week_demands", tag_name, line_no);
    }
    if (percent_contingency_fund_contribution == NON_INITIALIZED) {
        throw MissingParameter("percent_contingency_fund_contribution",
                               tag_name, line_no);
    }
    if (typesMonthlyDemandFraction.empty()) {
        throw MissingParameter("typesMonthlyDemandFraction", tag_name, line_no);
    }
    if (typesMonthlyWaterPrice.empty()) {
        throw MissingParameter("typesMonthlyWaterPrice", tag_name, line_no);
    }
    if (demand_buffer == NON_INITIALIZED) {
        throw MissingParameter("demand_buffer", tag_name, line_no);
    }

    bool all_construction_vars_initialized = (
            (!infra_construction_triggers.empty() ||
             !demand_infra_construction_order.empty()) &&
            !rof_infra_construction_order.empty() &&
            infra_discount_rate != NON_INITIALIZED);
    bool no_construction_var_initialized = (
            !infra_construction_triggers.empty() &&
            !demand_infra_construction_order.empty() &&
            !rof_infra_construction_order.empty() &&
            infra_discount_rate == NON_INITIALIZED &&
            !infra_if_built_remove.empty());

    if (!(all_construction_vars_initialized ||
          no_construction_var_initialized)) {

        throw InconsistentMutuallyImplicativeParameters(
                "either rof_infra_construction_order or "
                "demand_infra_construction_order, infra_if_built_remove, "
                "infra_construction_triggers, infra_discount_rate",
                tag_name, line_no);
    }
}
