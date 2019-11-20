//
// Created by Bernardo on 11/19/2019.
//

#include "WaterSourceParser.h"

int WaterSourceParser::id = 0;

void WaterSourceParser::updateId() {
    ++id;
}

int WaterSourceParser::getId() {
    return id;
}

void WaterSourceParser::parseVariables(vector<vector<string>> &block) {
    for (vector<string> &line : block) {
        if (line[0] == "name") {
            name = line[1];
        } else if (line[0] == "capacity") {
            capacity = stof(line[1]);
        } else if (line[0] == "bond") {
            bonds.push_back(master_bond_parser.generate_bond(line));
        } else if (line[0] == "construction_time") {
            if (line.size() != 3)
                throw invalid_argument("Lines \"construction_time\" must have three elements, the tag and upper and "
                                       "lower limits.");
            construction_time = {stod(line[0]), stod(line[1])};
        } else if (line[0] == "permitting_time") {
            permitting_time = stod(line[1]);
        }
    }
}
