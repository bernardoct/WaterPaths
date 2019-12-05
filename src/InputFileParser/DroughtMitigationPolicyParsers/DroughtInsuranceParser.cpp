//
// Created by Bernardo on 11/29/2019.
//

#include "DroughtInsuranceParser.h"
#include "../../Utils/Utils.h"
#include "../Exceptions/MissingParameter.h"
#include "../../DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "../AuxParserFunctions.h"


DroughtInsuranceParser::DroughtInsuranceParser()
        : DroughtMitigationPolicyParser("[INSURANCE POLICY]") {}

DroughtInsuranceParser::~DroughtInsuranceParser() = default;

void DroughtInsuranceParser::parseVariables(vector<vector<string>> &block,
                                            int n_realizations, int n_weeks,
                                            int line_no, Graph &utilities_graph,
                                            Graph &ws_graph,
                                            const map<string, int> &utility_name_to_id) {
    DroughtMitigationPolicyParser::parseVariables(block, n_realizations,
                                                  n_weeks, line_no,
                                                  utilities_graph, ws_graph,
                                                  utility_name_to_id);

    vector<double> insurance_triggers_raw, fixed_payouts_raw;
    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> line = block[i];
        if (line[0] == "insurance_triggers") {
            Utils::tokenizeString(line[1], insurance_triggers_raw, ',');
            rows_read.push_back(i);
        }
        if (line[0] == "insurance_premium") {
            insurance_premium = stod(line[1]);
            rows_read.push_back(i);
        }
        if (line[0] == "fixed_payouts") {
            Utils::tokenizeString(line[1], fixed_payouts_raw, ',');
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);

    if ((insurance_triggers_raw.size() != fixed_payouts_raw.size()) ||
        (apply_to_utilities.size() != fixed_payouts_raw.size())) {
        char error[300];
        sprintf(error, "Parameters insurance_triggers, fixed_payouts, and "
                       "insured_utilities in tag %s, line %d, must have same "
                       "number of records.", tag_name.c_str(), line_no);
        throw invalid_argument(error);
    }

    insurance_triggers = vector<double>(
            AuxParserFunctions::getMax(utility_name_to_id).second, 1.1);
    fixed_payouts = vector<double>(
            AuxParserFunctions::getMax(utility_name_to_id).second, 1.1);

    for (unsigned long j = 0; j < apply_to_utilities.size(); ++j) {
        insurance_triggers[apply_to_utilities[j]] = insurance_triggers_raw[j];
        fixed_payouts[apply_to_utilities[j]] = fixed_payouts_raw[j];
    }

}

void DroughtInsuranceParser::checkMissingOrExtraParams(int line_no,
                                                       vector<vector<string>> &block) {
    DroughtMitigationPolicyParser::checkMissingOrExtraParams(line_no, block);
    if (insurance_triggers.empty())
        throw MissingParameter("insurance_triggers", tag_name, line_no);
    if (insurance_premium == NON_INITIALIZED)
        throw MissingParameter("insurance_premium", tag_name, line_no);
    if (fixed_payouts.empty())
        throw MissingParameter("fixed_payouts", tag_name, line_no);
}

DroughtMitigationPolicy *
DroughtInsuranceParser::generateSource(
        int id, vector<vector<string>> &block,
        int line_no, int n_realizations, int n_weeks,
        const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id,
        Graph &utilities_graph, Graph &ws_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities, vector<WaterSource *> &water_sources,
        vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvFlowControl *> min_env_flow_controls,
        vector<vector<double>> &utilities_rdm,
        vector<vector<double>> &water_sources_rdm,
        vector<vector<double>> &policy_rdm) {
    parseVariables(block, n_realizations, n_weeks, line_no, utilities_graph,
                   ws_graph, utility_name_to_id);
    checkMissingOrExtraParams(line_no, block);

    return new InsuranceStorageToROF(id, water_sources, ws_graph,
                                     water_sources_to_utilities, utilities,
                                     drought_mitigation_policies,
                                     min_env_flow_controls, utilities_rdm,
                                     water_sources_rdm, policy_rdm,
                                     insurance_triggers, insurance_premium,
                                     fixed_payouts, n_weeks);
}
