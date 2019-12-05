//
// Created by Bernardo on 11/29/2019.
//

#include "DroughtMitigationPolicyParser.h"
#include "../Base/WaterSourceParser.h"
#include "../../Utils/Utils.h"
#include "../Exceptions/MissingParameter.h"
#include "../AuxParserFunctions.h"

DroughtMitigationPolicyParser::DroughtMitigationPolicyParser(string tag_name)
        : tag_name(tag_name) {};

void
DroughtMitigationPolicyParser::parseVariables(vector<vector<string>> &block,
                                              int n_realizations, int n_weeks, int line_no,
                                              Graph &utilities_graph,
                                              Graph &ws_graph,
                                              const map<string, int> &utility_name_to_id) {
    AuxParserFunctions::replaceNameById(block, tag_name, line_no, "apply_to_utilities", 1, utility_name_to_id);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        const vector<string> line = block[i];
        if (line[0] == "apply_to_utilities") {
            Utils::tokenizeString(line[1], apply_to_utilities, ',');
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

void DroughtMitigationPolicyParser::checkMissingOrExtraParams(
        int line_no, vector<vector<string>> &block) {
    if (apply_to_utilities.empty()) {
        throw MissingParameter("apply_to_utilities", tag_name, line_no);
    }

    // Check for tags that were not read.
    AuxParserFunctions::checkForUnreadTags(line_no, block, tag_name);
}

DroughtMitigationPolicyParser::~DroughtMitigationPolicyParser() = default;
