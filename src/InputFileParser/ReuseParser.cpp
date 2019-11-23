//
// Created by Bernardo on 11/19/2019.
//

#include "ReuseParser.h"
#include "MasterSystemInputFileParser.h"


void ReuseParser::parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks) {
    WaterSourceParser::parseVariables(block, n_realizations, n_weeks);

    if (block.size() == 2) {
        constructor_type = EXISTING_REUSE_STATION;
    } else if (block.size() == 5) {
        constructor_type = NON_EXISTING_REUSE_STATION;
    } else {
        throw invalid_argument("Wrong number of arguments for water reuse.");
    }
}

WaterSource* ReuseParser::generateSource(vector<vector<string>> &block, int n_realizations, int n_weeks) {
    updateId();

    parseVariables(block, n_realizations, n_weeks);

    if (constructor_type == EXISTING_REUSE_STATION) {
        return new WaterReuse(name, getId(), capacity);
    } else if (constructor_type == NON_EXISTING_REUSE_STATION) {
        return new WaterReuse(name, getId(), treatment_capacity, construction_time,
                permitting_time, *bonds[0]);
    } else {
        throw invalid_argument("Wrong number of arguments for water reuse");
    }
}
