//
// Created by Bernardo on 11/19/2019.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include "MasterSystemInputFileParser.h"
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "ReuseParser.h"
#include "../Utils/Utils.h"


void MasterSystemInputFileParser::parseFile(string file_path) {
    ifstream inputFile(file_path);
    int l = -1;
    // Create one object for each type of parser.
    ReuseParser reuse_parser;

    if (inputFile.is_open()) {
        while (!inputFile.eof()) {
            string line_tag;
            string first_line;

            vector<int> realizations_weeks;
            try {
                if (!getline(inputFile, first_line)) break;
                stoi(first_line);
                Utils::tokenizeString(first_line, realizations_weeks);
            } catch (...) {
                throw invalid_argument("First line of input file must contain "
                                       "only number of realizations and weeks.");
            }


            if (!getline(inputFile, line_tag)) break;
            if (line_tag.back() == '\r') line_tag.pop_back();

            l++;
            if (line_tag[0] == '[') {
                vector<vector<string>> block;
                block = readBlock(inputFile, l);

                if (line_tag == "[WATER REUSE]") {
                    water_sources.push_back(reuse_parser.generateSource(block,
                            realizations_weeks[0], realizations_weeks[1]));
                }
            }
        }
    } else {
        string error = "File " + file_path + " not found.";
        char error_char[error.size() + 1];
        strcpy(error_char, error.c_str());
        throw invalid_argument(error_char);
    }
}

vector<vector<string>> MasterSystemInputFileParser::readBlock(ifstream &inputFile, int &l) const {
    vector<vector<string>> block;
    string line;

    while (getline(inputFile, line) && line[0] != '[') {
        if (line.back() == '\r') line.pop_back();

        if (line[0] != '#' && !line.empty()) {
            vector<string> tokenized_line;
            Utils::tokenizeString(line, tokenized_line);
            block.push_back(tokenized_line);
        }
        l++;
    }

    return block;
}

MasterSystemInputFileParser::~MasterSystemInputFileParser() {
    for (auto ws : water_sources) {
        delete ws;
    }
    water_sources.clear();
}

const vector<WaterSource *>& MasterSystemInputFileParser::getWaterSources() const{
    return water_sources;
}
