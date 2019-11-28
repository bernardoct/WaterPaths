//
// Created by bernardo on 11/27/19.
//

#include <algorithm>
#include "AuxDataParser.h"
#include "../Utils/Utils.h"


Graph AuxDataParser::parseGraph(vector<vector<string>> &block, map<string, int> &name_to_id, const string &tag, int l) {

    for (auto line : block) {
        replaceNameById(block, tag, l, line[0], 0, name_to_id);
    }

    vector<vector<int>> edges;
    vector<int> edges_list;
    for (auto line : block) {
        vector<int> edges_line;
        Utils::tokenizeString(line[0], edges_line, ',');
        if (edges_line.size() != 2)
            throw invalid_argument("All rows in graph block must have two entries (starting and ending points");

        edges.push_back(edges_line);
        edges_list.insert(edges_list.end(), edges_line.begin(), edges_line.end());
    }

    sort(edges_list.begin(), edges_list.end());
    edges_list.erase(unique(edges_list.begin(), edges_list.end()), edges_list.end());

    Graph g(edges_list.size());

    for (auto edges_line : edges) {
        g.addEdge(edges_line[0], edges_line[1]);
    }

    return g;
}

vector<vector<int>> AuxDataParser::parseReservoirUtilityConnectivityMatrix(
        vector<vector<string>> &block, const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id, const string &tag, int l) {

    for (auto line : block) {
        replaceNameById(block, tag, l, line[0], 1, ws_name_to_id);
        replaceNameById(block, tag, l, line[0], 0, utility_name_to_id);
    }

    vector<vector<int>> reservoir_utility_connectivity_matrix(block.size());
    for (auto &line : block) {
        vector<int> utilitys_sources;
        Utils::tokenizeString(line[1], utilitys_sources, ',');
        reservoir_utility_connectivity_matrix[stoi(line[0])] = utilitys_sources;
    }

    return reservoir_utility_connectivity_matrix;
}

void AuxDataParser::replaceNameById(vector<vector<string>> &block,
        const string &tag_name, int line_no, string param_name,
        int param_pos, map<string, int> name_to_id) {
    for (vector<string> &line : block) {
        if (line[0] == param_name) {
            vector<string> names;
            Utils::tokenizeString(line[param_pos], names, ',');
            string ids;

            for (auto & name : names) {
                try {
                    name = to_string(name_to_id[name]) + ",";
                    ids += name;
                } catch (...) {
                    char error[256];
                    sprintf(error, "Could not find %s of %s in line %d. "
                                   "All %s must be added in the input file above "
                                   "associated %ss.", name.c_str(), param_name.c_str(), line_no,
                            param_name.c_str(), tag_name.c_str());
                    throw invalid_argument(error);
                }
            }
            ids.pop_back();
            line[param_pos] = ids;
        }
    }
}