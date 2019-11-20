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
            if (!getline(inputFile, line_tag)) break;
            if (line_tag.back() == '\r') line_tag.pop_back();

            l++;
            if (line_tag[0] == '[') {
                vector<vector<string>> block;
                block = readBlock(inputFile, l);

                if (line_tag == "[WATER REUSE]") {
                    water_sources.push_back(reuse_parser.generateSource(block));
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
    string  line;

    while (getline(inputFile, line) && line[0] != '[') {
        if (line.back() == '\r') line.pop_back();

        if (line[0] != '#' && !line.empty()) {
            block.push_back(Utils::tokenizeString(line));
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
