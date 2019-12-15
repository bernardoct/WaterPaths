//
// Created by Bernardo on 11/29/2019.
//

#include <stdexcept>
#include <algorithm>
#include "AuxParserFunctions.h"
#include "../Utils/Utils.h"


void AuxParserFunctions::checkForUnreadTags(int line_no,
                                            const vector<vector<string>> &block,
                                            const string &tag_name) {
    if (!block.empty()) {
        string extra_params;
        for (auto line : block) {
            extra_params += line[0] + " ";
        }
        char error[5000];
        sprintf(error, "Parameter(s) %s do not belong to %s (line %d). This may"
                       " also be a typo.",
                extra_params.c_str(), tag_name.c_str(), line_no);
        throw invalid_argument(error);
    }
}

void AuxParserFunctions::cleanBlock(vector<vector<string>> &block,
                                    vector<unsigned long> &rows_read) {
    sort(rows_read.begin(), rows_read.end());
    reverse(rows_read.begin(), rows_read.end());
    for (unsigned long &i : rows_read) {
        block.erase(block.begin() + i);
    }
}


Graph AuxParserFunctions::parseGraph(vector<vector<string>> &block,
                                     map<string, int> &name_to_id,
                                     const string &tag, int line_no) {

    for (auto line : block) {
        replaceNameById(block, tag, line_no, line[0], 0, name_to_id);
    }

    vector<vector<int>> edges;
    vector<int> edges_list;
    for (auto line : block) {
        vector<int> edges_line;
        Utils::tokenizeString(line[0], edges_line, ',');
        if (edges_line.size() != 2)
            throw invalid_argument(
                    "All rows in graph block must have two entries (starting and ending points");

        edges.push_back(edges_line);
        edges_list.insert(edges_list.end(), edges_line.begin(),
                          edges_line.end());
    }

    sort(edges_list.begin(), edges_list.end());
    edges_list.erase(unique(edges_list.begin(), edges_list.end()),
                     edges_list.end());

    Graph g(edges_list.size());

    for (auto edges_line : edges) {
        g.addEdge(edges_line[0], edges_line[1]);
    }

    return g;
}

void AuxParserFunctions::replaceNameById(vector<vector<string>> &block,
                                         const string &tag_name, int line_no,
                                         string param_name,
                                         int param_pos,
                                         map<string, int> name_to_id) {
    for (vector<string> &line : block) {
        if (line[0] == param_name) {
            vector<string> names;
            Utils::tokenizeString(line[param_pos], names, ',');
            string ids;

            for (auto &name : names) {
                try {
                    name = to_string(name_to_id.at(name)) + ",";
                    ids += name;
                } catch (...) {
                    char error[256];
                    sprintf(error, "Could not find %s in parameter %s of tag %s"
                                   " line %d. All %s must be added in the input"
                                   " file above associated %ss.", name.c_str(),
                            param_name.c_str(), tag_name.c_str(), line_no,
                            param_name.c_str(), tag_name.c_str());
                    throw invalid_argument(error);
                }
            }
            ids.pop_back();
            line[param_pos] = ids;
        }
    }
}
