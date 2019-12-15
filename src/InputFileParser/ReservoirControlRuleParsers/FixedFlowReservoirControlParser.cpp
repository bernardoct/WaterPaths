//
// Created by Bernardo on 11/29/2019.
//

#include "FixedFlowReservoirControlParser.h"
#include "../AuxParserFunctions.h"
#include "../../Controls/FixedMinEnvFlowControl.h"
#include "../Exceptions/MissingParameter.h"

FixedFlowReservoirControlParser::FixedFlowReservoirControlParser()
        : ReservoirControlRuleParser("[FIXED FLOW RESERVOIR CONTROL RULE]") {}

void FixedFlowReservoirControlParser::parseVariables(
        vector<vector<string>> &block, int n_realizations, int n_weeks,
        int line_no, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id,
        map<string, vector<vector<double>>> &pre_loaded_data) {
    ReservoirControlRuleParser::parseVariables(block, n_realizations, n_weeks,
                                               line_no, ws_name_to_id,
                                               utility_name_to_id,
                                               pre_loaded_data
    );

    vector<unsigned long> rows_read;
    for (unsigned long i = 0; i < block.size(); ++i) {
        auto line = block[i];
        if (line[0] == "release") {
            release = stod(line[1]);
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

MinEnvFlowControl *
FixedFlowReservoirControlParser::generateReservoirControlRule(
        vector<vector<string>> &block, int line_no, int n_realizations,
        int n_weeks, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id,
        map<string, vector<vector<double>>> &pre_loaded_data) {
    parseVariables(block, n_realizations, n_weeks, line_no, ws_name_to_id,
                   utility_name_to_id, pre_loaded_data);
    checkMissingOrExtraParams(line_no, block);

    return new FixedMinEnvFlowControl(water_source_id, release);
}

void FixedFlowReservoirControlParser::checkMissingOrExtraParams(int line_no,
                                                                vector<vector<string>> &block) {
    ReservoirControlRuleParser::checkMissingOrExtraParams(line_no, block);

    if (release == NON_INITIALIZED)
        throw MissingParameter("release", tag, line_no);
}

double FixedFlowReservoirControlParser::getRelease() const {
    return release;
}

