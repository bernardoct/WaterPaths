//
// Created by Bernardo on 11/19/2019.
//

#include <iostream>
#include <cstring>
#include "MasterSystemInputFileParser.h"
#include "WaterSources/ReuseParser.h"
#include "../Utils/Utils.h"
#include "WaterSources/ReservoirParser.h"
#include "WaterSources/AllocatedReservoirParser.h"


void MasterSystemInputFileParser::parseFile(string file_path) {
    ifstream inputFile(file_path);
    int l = -1;
    // Create one object for each type of parser.

    if (inputFile.is_open()) {
        vector<int> realizations_weeks;
        string line;
        try {
            if (!getline(inputFile, line))
                throw invalid_argument("Empty input file.");
            Utils::tokenizeString(line, realizations_weeks);
        } catch (...) {
            throw invalid_argument("First line of input file must contain "
                                   "only number of realizations and weeks.");
        }

        while (!getline(inputFile, line)) if (line[0] == '[') break;

        vector<vector<vector<string>>> blocks;
        vector<int> line_nos;
        vector<string> tags;

        l = parseBlocks(inputFile, l, line, blocks, line_nos, tags);

        for (unsigned long t = 0; t < tags.size(); ++t) {
            bool tag_read = false;
            // Check if tag is a water source and, if so, create corresponding water source.
            tag_read = (createWaterSource(l, realizations_weeks, blocks[t], tags[t]) ? true : tag_read);

            if (!tag_read) {
                char error[128];
                sprintf(error, "Unrecognized tag %s on line %d", tags[t].c_str(), l);
                throw invalid_argument(error);
            }
        }
    } else {
        string error = "File " + file_path + " not found.";
        char error_char[error.size() + 1];
        strcpy(error_char, error.c_str());
        throw invalid_argument(error_char);
    }
}

bool MasterSystemInputFileParser::createWaterSource(int l, const vector<int> &realizations_weeks,
                                                    vector<vector<string>> &block,
                                                    const string &tag) {
    int ws_id = (int) water_sources.size();
    if (tag == "[WATER REUSE]") {
        water_source_parsers.push_back(new ReuseParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1])
        );
        return true;
    } else if (tag == "[RESERVOIR]") {
        water_source_parsers.push_back(new ReservoirParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1])
        );
        return true;
    } else if (tag == "[ALLOCATED RESERVOIR]") {
        water_source_parsers.push_back(new AllocatedReservoirParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1])
        );
        return true;
    }

    return false;
}

int MasterSystemInputFileParser::parseBlocks(ifstream &inputFile, int l, string &line,
                                             vector<vector<vector<string>>> &blocks, vector<int> &line_nos,
                                             vector<string> &tags) {
    while (!inputFile.eof()) {
        if (line.back() == '\r') line.pop_back();

        l++;
        if (line[0] == '[') {
            tags.push_back(line);
            blocks.push_back(readBlock(inputFile, l, line));
            line_nos.push_back(l);
        } else {
            if (!getline(inputFile, line)) break;
        }
    }
    return l;
}

vector<vector<string>> MasterSystemInputFileParser::readBlock(ifstream &inputFile, int &l, string &line) {
    vector<vector<string>> block;

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
    for (auto wsp : water_source_parsers) {
        delete wsp;
    }
    water_source_parsers.clear();
    for (auto ws : water_sources) {
        delete ws;
    }
    water_sources.clear();
}

const vector<WaterSource *>& MasterSystemInputFileParser::getWaterSources() const{
    return water_sources;
}
