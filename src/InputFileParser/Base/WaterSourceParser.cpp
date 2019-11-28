//
// Created by Bernardo on 11/19/2019.
//

#include <algorithm>
#include "WaterSourceParser.h"
#include "../../Utils/Utils.h"
#include "../Exceptions/MissingParameter.h"
#include "../Exceptions/InconsistentMutuallyImplicativeParameters.h"


WaterSourceParser::WaterSourceParser(string tag_name) : tag_name(tag_name) {}

void WaterSourceParser::parseVariables(vector<vector<string>> &block,
                                        int n_realizations, int n_weeks) {
    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i){
        vector<string> line = block[i];
        if (line[0] == "name") {
            name = line[1];
            rows_read.push_back(i);
        } else if (line[0] == "capacity") {
            capacity = stof(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "bond") {
            bonds.push_back(master_bond_parser.generate_bond(line));
            rows_read.push_back(i);
            existing_infrastructure = false;
        } else if (line[0] == "ctime") {
            if (line.size() != 3)
                throw invalid_argument("Lines \"construction_time\" must have "
                                       "three elements, the tag and upper and "
                                       "lower limits.");
            construction_time = {stod(line[1]), stod(line[2])};
            rows_read.push_back(i);
            existing_infrastructure = false;
        } else if (line[0] == "ptime") {
            permitting_time = stoi(line[1]);
            rows_read.push_back(i);
            existing_infrastructure = false;
        } else if (line[0] == "treatment_capacity") {
            treatment_capacity = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "allocated_treatment_fractions") {
            Utils::tokenizeString(line[1], allocated_treatment_fractions, ',');
            rows_read.push_back(i);
        } else if (line[0] == "allocated_fractions") {
            Utils::tokenizeString(line[1], allocated_fractions, ',');
            rows_read.push_back(i);
        } else if (line[0] == "utilities_with_allocations") {
            Utils::tokenizeString(line[1], utilities_with_allocations, ',');
            rows_read.push_back(i);
        }
    }

    cleanBlock(block, rows_read);
}

WaterSourceParser::~WaterSourceParser() {
//    for (Bond *b : bonds) {
//        delete b;
//    }
    bonds.clear();
}

void WaterSourceParser::checkMissingOrExtraParams(int line_no, vector<vector<string>> &block) {
    if (name.empty()) {
        throw MissingParameter("name", tag_name, line_no);
    } else if (capacity == NON_INITIALIZED) {
        throw MissingParameter("capacity", tag_name, line_no);
    }

    if (!((bonds.empty() && permitting_time == NON_INITIALIZED &&
                    construction_time.empty()) ||
          (!bonds.empty() && permitting_time != NON_INITIALIZED &&
          !construction_time.empty()))) {
        throw InconsistentMutuallyImplicativeParameters(
                "bonds, permitting_time, construction_time",
                tag_name, line_no
                );
    }

    if (!((utilities_with_allocations.empty() && allocated_fractions.empty() &&
         allocated_treatment_fractions.empty()) ||
        (!utilities_with_allocations.empty() && !allocated_fractions.empty() &&
         !allocated_treatment_fractions.empty()))) {
        throw InconsistentMutuallyImplicativeParameters(
                "utilities_with_allocations, allocated_fractions, "
                "allocated_treatment_fractions", tag_name, line_no
                );
    }

    // Check for tags that were not read.
    checkForUnreadTags(line_no, block, tag_name);
}

void WaterSourceParser::checkForUnreadTags(int line_no, const vector<vector<string>> &block, const string& tag_name) {
    if (!block.empty()) {
        string extra_params;
        for (auto line : block) {
            extra_params += line[0] + " ";
        }
        char error[5000];
        sprintf(error, "Parameters %s do not belong to a %s (line %d)",
                extra_params.c_str(), tag_name.c_str(), line_no);
        throw invalid_argument(error);
    }
}

void WaterSourceParser::cleanBlock(vector<vector<string>> &block, vector<unsigned long> &rows_read) {
    sort(rows_read.begin(), rows_read.end());
    reverse(rows_read.begin(), rows_read.end());
    for (unsigned long &i : rows_read) {
        block.erase(block.begin() + i);
    }
}
