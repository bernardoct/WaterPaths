//
// Created by Bernardo on 11/30/2019.
//

#include "StorageMinEnvFlowControlParser.h"
#include "../AuxParserFunctions.h"
#include "../Exceptions/MissingParameter.h"
#include "../../Utils/Utils.h"
#include "../../Controls/StorageMinEnvFlowControl.h"



StorageMinEnvFlowControlParser::StorageMinEnvFlowControlParser()
        : ReservoirControlRuleParser("[STORAGE-BASED RESERVOIR CONTROL RULE]") {}

void StorageMinEnvFlowControlParser::parseVariables(
        vector <vector<string>> &block, int n_realizations,
        int n_weeks, int line_no,
        const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id) {
    ReservoirControlRuleParser::parseVariables(block, n_realizations,
                                               n_weeks, line_no, ws_name_to_id,
                                               utility_name_to_id);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector <string> line = block[i];
        if (line[0] == "storages") {
            Utils::tokenizeString(line[1], storages, ',');
            rows_read.push_back(i);
        } else if (line[0] == "releases") {
            Utils::tokenizeString(line[1], releases, ',');
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::checkForUnreadTags(line_no, block, tag);
}

MinEnvFlowControl *
StorageMinEnvFlowControlParser::generateReservoirControlRule(
        vector <vector<string>> &block, int line_no, int n_realizations,
        int n_weeks, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id) {
    parseVariables(block, n_realizations, n_weeks, line_no, ws_name_to_id,
                   utility_name_to_id);
    checkMissingOrExtraParams(line_no, block);
    return new StorageMinEnvFlowControl(water_source_id, storages, releases);
}

void StorageMinEnvFlowControlParser::checkMissingOrExtraParams(
        int line_no, vector <vector<string>> &block) {
    ReservoirControlRuleParser::checkMissingOrExtraParams(line_no, block);

    if (storages.empty())
        throw MissingParameter("storages", tag, line_no);
    else if (releases.empty()) {
        throw MissingParameter("releases", tag, line_no);
    }
}
