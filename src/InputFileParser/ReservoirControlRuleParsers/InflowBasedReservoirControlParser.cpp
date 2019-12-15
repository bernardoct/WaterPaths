//
// Created by Bernardo on 11/30/2019.
//

#include "InflowBasedReservoirControlParser.h"
#include "../../Utils/Utils.h"
#include "../AuxParserFunctions.h"
#include "../../Controls/InflowMinEnvFlowControl.h"
#include "../Exceptions/MissingParameter.h"

InflowBasedReservoirControlParser::InflowBasedReservoirControlParser()
        : ReservoirControlRuleParser("[INFLOW-BASED RESERVOIR CONTROL RULE]") {

}

void InflowBasedReservoirControlParser::parseVariables(
        vector<vector<string>> &block, int n_realizations,
        int n_weeks, int line_no, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id,
        map<string, vector<vector<double>>> &pre_loaded_data) {
    ReservoirControlRuleParser::parseVariables(block, n_realizations, n_weeks,
                                               line_no, ws_name_to_id,
                                               utility_name_to_id,
                                               pre_loaded_data);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> line = block[i];
        if (line[0] == "inflows") {
            Utils::tokenizeString(line[1], inflows, ',');
            rows_read.push_back(i);
        } else if (line[0] == "releases") {
            Utils::tokenizeString(line[1], releases, ',');
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

MinEnvFlowControl *
InflowBasedReservoirControlParser::generateReservoirControlRule(
        vector<vector<string>> &block, int line_no, int n_realizations,
        int n_weeks, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id,
        map<string, vector<vector<double>>> &pre_loaded_data) {
    parseVariables(block, n_realizations, n_weeks, line_no, ws_name_to_id,
                   utility_name_to_id, pre_loaded_data);
    checkMissingOrExtraParams(line_no, block);

    return new InflowMinEnvFlowControl(water_source_id, inflows, releases);
}

void InflowBasedReservoirControlParser::checkMissingOrExtraParams(
        int line_no, vector<vector<string>> &block) {
    ReservoirControlRuleParser::checkMissingOrExtraParams(line_no, block);

    if (inflows.empty())
        throw MissingParameter("inflows", tag, line_no);
    else if (releases.empty()) {
        throw MissingParameter("releases", tag, line_no);
    }
}

const vector<double> &InflowBasedReservoirControlParser::getInflows() const {
    return inflows;
}

const vector<double> &InflowBasedReservoirControlParser::getReleases() const {
    return releases;
}
