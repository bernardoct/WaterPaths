//
// Created by Bernardo on 11/27/2019.
//

#include "ReservoirExpansionParser.h"
#include "../../SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../AuxDataParser.h"


ReservoirExpansionParser::ReservoirExpansionParser() : WaterSourceParser(
        "[RESERVOIR EXPANSION") {}

void ReservoirExpansionParser::parseVariables(vector<vector<string>> &block,
                                              int n_realizations, int n_weeks) {
    WaterSourceParser::parseVariables(block, n_realizations, n_weeks);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> line = block[i];
        if (line[0] == "parent_reservoir") {
            parent_reservoir_ID = stoul(line[1]);
            rows_read.push_back(i);
        }
    }

    cleanBlock(block, rows_read);
}

void ReservoirExpansionParser::preProcessBlock(vector<vector<string>> &block,
                                               const string &tag, int l,
                                               const map<string, int> &ws_name_to_id) {
    AuxDataParser::replaceNameById(block, tag, l, "parent_reservoir", 1,
                                   ws_name_to_id);
}

WaterSource *
ReservoirExpansionParser::generateSource(int id, vector<vector<string>> &block,
                                         int line_no, int n_realizations,
                                         int n_weeks,
                                         const map<string, int> &ws_name_to_id,
                                         const map<string, int> &utility_name_to_id) {
    preProcessBlock(block, tag_name, line_no, ws_name_to_id);
    parseVariables(block, n_realizations, n_weeks);
    checkMissingOrExtraParams(line_no, block);

    return new ReservoirExpansion(name, id, parent_reservoir_ID, capacity,
                                  construction_time, permitting_time,
                                  *bonds[0]);
}

void ReservoirExpansionParser::checkMissingOrExtraParams(int line_no,
                                                         vector<vector<string>> &block) {
    WaterSourceParser::checkMissingOrExtraParams(line_no, block);
}
