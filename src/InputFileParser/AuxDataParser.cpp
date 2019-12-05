//
// Created by bernardo on 11/27/19.
//

#include <algorithm>
#include "AuxDataParser.h"
#include "../Utils/Utils.h"
#include "AuxParserFunctions.h"


vector<vector<int>> AuxDataParser::parseReservoirUtilityConnectivityMatrix(
        vector<vector<string>> &block, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id, const string &tag, int l) {

    for (auto line : block) {
        AuxParserFunctions::replaceNameById(block, tag, l, line[0], 1,
                                            ws_name_to_id);
        AuxParserFunctions::replaceNameById(block, tag, l, line[0], 0,
                                            utility_name_to_id);
    }

    vector<vector<int>> reservoir_utility_connectivity_matrix(block.size());
    for (auto &line : block) {
        vector<int> utilitys_sources;
        Utils::tokenizeString(line[1], utilitys_sources, ',');
        reservoir_utility_connectivity_matrix[stoi(line[0])] = utilitys_sources;
    }

    return reservoir_utility_connectivity_matrix;
}

vector<vector<double>>
AuxDataParser::tableStorageShiftParser(vector<vector<string>> &block,
                                       const map<string, int> &ws_name_to_id,
                                       const map<string, int> &utility_name_to_id,
                                       const string &tag, int l) {

    for (vector<string> &line : block) {
        try {
            line[0] = to_string(utility_name_to_id.at(line[0]));
            line[1] = to_string(ws_name_to_id.at(line[1]));
        } catch (const std::out_of_range &e) {
            char error[500];
            sprintf(error,
                    "Wrong name in %s in line %d. Are either %s or %s mispelled "
                    "in either in storage shift table or in utility/water "
                    "source?", tag.c_str(), l, line[0].c_str(), line[1].c_str()
            );
            throw invalid_argument(error);
        }
    }

    try {
        vector<vector<double>> table_storage_shift(
                AuxParserFunctions::getMax(utility_name_to_id).second + 1,
                vector<double>(
                        AuxParserFunctions::getMax(ws_name_to_id).second + 1,
                        0));
        for (auto line : block) {
            vector<int> v = {stoi(line[0]), stoi(line[1])};
            table_storage_shift[v[0]][v[1]] = stod(line[2]);
        }

        return table_storage_shift;
    } catch (...) {
        char error[128];
        sprintf(error, "Something is wrong with %s in line %d", tag.c_str(), l);
        throw invalid_argument(error);
    }
}