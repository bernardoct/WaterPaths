//
// Created by bernardo on 11/27/19.
//

#include <algorithm>
#include "AuxDataParser.h"
#include "../Utils/Utils.h"


Graph AuxDataParser::parseGraph(vector<vector<string>> &block) {

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
