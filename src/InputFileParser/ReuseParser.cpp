//
// Created by Bernardo on 11/19/2019.
//

#include "ReuseParser.h"
#include "MasterElementParser.h"


void ReuseParser::parseVariables(vector<vector<string>> &block) {
    WaterSourceParser::parseVariables(block);

    if (block.size() == 3) {
        constructor_type = EXISTING_REUSE_STATION;
    } else if (block.size() == 6) {
        constructor_type = NON_EXISTING_REUSE_STATION;
    } else {
        throw invalid_argument("Wrong number of arguments for water reuse.");
    }
}

WaterSource* ReuseParser::generateSource(vector<vector<string>> &block) {
    updateId();

    parseVariables(block);

    if (constructor_type == EXISTING_REUSE_STATION) {
        return new WaterReuse(name.c_str(), getId(), capacity);
    } else if (constructor_type == NON_EXISTING_REUSE_STATION) {
        return new WaterReuse(name.c_str(), getId(), treatment_capacity, construction_time,
                permitting_time, *bonds[0]);
    } else {
        throw invalid_argument("Wrong number of arguments for water reuse");
    }
}
