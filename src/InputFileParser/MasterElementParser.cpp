//
// Created by Bernardo on 11/19/2019.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include "MasterElementParser.h"
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "ReuseParser.h"
#include "../Utils/Utils.h"


void MasterElementParser::parseFile(string file_path) {
    ifstream inputFile(file_path);
    int l = -1;
    ReuseParser reuse_parser;


    if (inputFile.is_open()) {
        while (!inputFile.eof()) {
            vector<vector<string>> block(0);
            l++;
            string s;
            if (!getline(inputFile, s)) break;

            if (s[0] == '[') {
                if (l > 0) {
                    if (s.compare("[WATER REUSE]")) {
                        water_sources.push_back(reuse_parser.generateSource(block));
                    }
                }
                block = vector<vector<string>>();
            } else {
                if (s[0] != '#') {
                    block.push_back(Utils::tokenizeString(s));
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

MasterElementParser::~MasterElementParser() {
    for (auto ws : water_sources) {
        delete ws;
    }
    water_sources.clear();
}
