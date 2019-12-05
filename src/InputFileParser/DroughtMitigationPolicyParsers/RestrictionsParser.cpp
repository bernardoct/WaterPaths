//
// Created by Bernardo on 11/29/2019.
//

#include "RestrictionsParser.h"
#include "../../Utils/Utils.h"
#include "../AuxParserFunctions.h"
#include "../Exceptions/MissingParameter.h"
#include "../Exceptions/InconsistentMutuallyImplicativeParameters.h"

RestrictionsParser::RestrictionsParser()
        : DroughtMitigationPolicyParser("[RESTRICTIONS POLICY]") {}

void RestrictionsParser::parseVariables(vector<vector<string>> &block,
                                        int n_realizations, int n_weeks, int line_no,
                                        Graph &utilities_graph,
                                        Graph &ws_graph,
                                        const map<string, int> &utility_name_to_id) {
    DroughtMitigationPolicyParser::parseVariables(block, n_realizations,
                                                  n_weeks, line_no,
                                                  utilities_graph, ws_graph, utility_name_to_id);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> line = block[i];
        if (line[0] == "stage_multipliers") {
            Utils::tokenizeString(line[1], stage_multipliers, ',');
            rows_read.push_back(i);
        } else if (line[0] == "stage_triggers") {
            Utils::tokenizeString(line[1], stage_triggers, ',');
            rows_read.push_back(i);
        } else if (line[0] == "typesMonthlyDemandFraction") {
            typesMonthlyDemandFraction = Utils::parse2DCsvFile(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "typesMonthlyWaterPrice") {
            typesMonthlyWaterPrice = Utils::parse2DCsvFile(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "priceMultipliers") {
            priceMultipliers = Utils::parse2DCsvFile(line[1]);
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

DroughtMitigationPolicy *
RestrictionsParser::generateSource(int id, vector<vector<string>> &block, int line_no,
                                   int n_realizations, int n_weeks,
                                   const map<string, int> &ws_name_to_id,
                                   const map<string, int> &utility_name_to_id,
                                   Graph &utilities_graph, Graph &ws_graph,
                                   const vector<vector<int>> &water_sources_to_utilities,
                                   vector<Utility *> &utilities,
                                   vector<WaterSource *> &water_source,
                                   vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                                   vector<MinEnvFlowControl *> min_env_flow_controls,
                                   vector<vector<double>>& utilities_rdm,
                                   vector<vector<double>>& water_sources_rdm,
                                   vector<vector<double>>& policy_rdm) {
    parseVariables(block, n_realizations, n_weeks, line_no,
                   utilities_graph, ws_graph, utility_name_to_id);
    checkMissingOrExtraParams(line_no, block);

    if (priceMultipliers.empty()) {
        return new Restrictions(id, stage_multipliers, stage_triggers);
    } else {
        return new Restrictions(id, stage_multipliers, stage_triggers,
                                &typesMonthlyDemandFraction,
                                &typesMonthlyWaterPrice, &priceMultipliers);
    }
}

void RestrictionsParser::checkMissingOrExtraParams(int line_no,
                                                   vector<vector<string>> &block) {
    DroughtMitigationPolicyParser::checkMissingOrExtraParams(line_no, block);
    if (stage_multipliers.empty()) {
        throw MissingParameter("stage_multipliers", tag_name, line_no);
    } else if (stage_triggers.empty()) {
        throw MissingParameter("stage_triggers", tag_name, line_no);
    }

    if (!((typesMonthlyDemandFraction.empty() &&
           typesMonthlyWaterPrice.empty() && priceMultipliers.empty()) ||
          (!typesMonthlyDemandFraction.empty() &&
           !typesMonthlyWaterPrice.empty() && !priceMultipliers.empty()))) {
        throw InconsistentMutuallyImplicativeParameters(
                "typesMonthlyDemandFraction, typesMonthlyWaterPrice, "
                "priceMultipliers", tag_name, line_no);
    }
}

