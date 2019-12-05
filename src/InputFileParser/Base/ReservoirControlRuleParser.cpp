//
// Created by Bernardo on 11/29/2019.
//

#include "ReservoirControlRuleParser.h"
#include "../../Utils/Utils.h"
#include "../AuxParserFunctions.h"
#include "../Exceptions/MissingParameter.h"

ReservoirControlRuleParser::ReservoirControlRuleParser(string tag) : tag(tag) {}

void ReservoirControlRuleParser::parseVariables(vector <vector<string>> &block,
                                                int n_realizations,
                                                int n_weeks, int line_no,
                                                const map<string, int> &ws_name_to_id,
                                                const map<string, int> &utility_name_to_id) {
    AuxParserFunctions::replaceNameById(block, tag, line_no,
                                        "water_source_id", 1,
                                        ws_name_to_id);
    AuxParserFunctions::replaceNameById(block, tag, line_no,
                                        "aux_water_sources_id", 1,
                                        ws_name_to_id);
    AuxParserFunctions::replaceNameById(block, tag, line_no,
                                        "aux_utilities_ids", 1,
                                        utility_name_to_id);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector <string> line = block[i];
        if (line[0] == "water_source_id") {
            water_source_id = stoi(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "aux_water_sources_id") {
            Utils::tokenizeString(line[1], aux_water_sources_id, ',');
            rows_read.push_back(i);
        } else if (line[0] == "aux_utilities_ids") {
            Utils::tokenizeString(line[1], aux_utilities_ids, ',');
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

void ReservoirControlRuleParser::checkMissingOrExtraParams(int line_no,
                                                           vector <vector<string>> &block) {

    if (water_source_id == NON_INITIALIZED)
        throw MissingParameter("water_source_id", tag, line_no);

    AuxParserFunctions::checkForUnreadTags(line_no, block, tag);
}