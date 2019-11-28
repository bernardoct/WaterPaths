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
#include "WaterSources/ReservoirExpansionParser.h"
#include "Exceptions/MissingParameter.h"
#include "AuxDataParser.h"

/**
 * Parses system described in input file.
 * @param file_path path to input file.
 */
void MasterSystemInputFileParser::parseFile(string file_path) {
    ifstream inputFile(file_path);
    int l = -1; /// number of line being read.

    if (inputFile.is_open()) {
        vector<int> realizations_weeks;
        string line;
        // Checks if file is not empty and gets number of realizations and weeks
        try {
            if (!getline(inputFile, line))
                throw invalid_argument("Empty input file.");
            Utils::tokenizeString(line, realizations_weeks);
        } catch (...) {
            throw invalid_argument("First line of input file must contain "
                                   "only number of realizations and weeks.");
        }

        // Looks for first tag
        while (!getline(inputFile, line)) if (line[0] == '[') break;

        vector<vector<vector<string>>> blocks;
        vector<int> line_nos;
        vector<string> tags;

        l = parseBlocks(inputFile, l, line, blocks, line_nos, tags);

        for (unsigned long t = 0; t < tags.size(); ++t) {
            bool tag_read = false;
            // Check if tag is a water source and, if so, create corresponding water source.
            tag_read = (createWaterSource(l, realizations_weeks, blocks[t],
                                          tags[t]) ? true : tag_read);
            tag_read = (createUtility(l, realizations_weeks, blocks[t], tags[t])
                        ? true : tag_read);
            tag_read = (createGraphsAndMatrices(l, realizations_weeks,
                                                blocks[t], tags[t]) ? true
                                                                    : tag_read);

            if (!tag_read) {
                char error[128];
                sprintf(error, "Unrecognized tag %s on line %d",
                        tags[t].c_str(), l);
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

bool MasterSystemInputFileParser::createGraphsAndMatrices(int l,
                                                          const vector<int> &realizations_weeks,
                                                          vector<vector<string>> &block,
                                                          const string &tag) {
    if (tag == "[WATER SOURCES GRAPH]") {
        water_sources_graph = AuxDataParser::parseGraph(block, ws_name_to_id,
                                                        tag, l);
        return true;
    } else if (tag == "[UTILITIES GRAPH]") {
        utilities_graph = AuxDataParser::parseGraph(block, utility_name_to_id,
                                                    tag, l);
        return true;
    } else if (tag == "[WS TO UTILITY MATRIX]") {
        reservoir_utility_connectivity_matrix =
                AuxDataParser::parseReservoirUtilityConnectivityMatrix(
                        block, ws_name_to_id, utility_name_to_id, tag, l);
        return true;
    }

    return false;
}

bool MasterSystemInputFileParser::createUtility(
        int l, const vector<int> &realizations_weeks,
        vector<vector<string>> &block, const string &tag) {
    if (tag == "[UTILITY]") {
        int utility_id = (int) utilities.size();

        utility_parsers.push_back(new UtilityParser());
        utilities.push_back(utility_parsers.back()->generateUtility(
                utility_id, block, l, realizations_weeks[0], ws_name_to_id));
        utility_name_to_id.insert(pair<string, int>(utilities.back()->name,
                                                    utilities.back()->id));
        return true;
    }

    return false;
}

bool MasterSystemInputFileParser::createWaterSource(
        int l, const vector<int> &realizations_weeks,
        vector<vector<string>> &block, const string &tag) {
    int ws_id = (int) water_sources.size();
    if (tag == "[WATER REUSE]") {
        water_source_parsers.push_back(new ReuseParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1], ws_name_to_id,
                        utility_name_to_id)
        );
    } else if (tag == "[RESERVOIR]") {
        water_source_parsers.push_back(new ReservoirParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1], ws_name_to_id,
                        utility_name_to_id)
        );
    } else if (tag == "[ALLOCATED RESERVOIR]") {
        water_source_parsers.push_back(new AllocatedReservoirParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1], ws_name_to_id,
                        utility_name_to_id)
        );
    } else if (tag == "[RESERVOIR EXPANSION]") {
        water_source_parsers.push_back(new ReservoirExpansionParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1], ws_name_to_id, utility_name_to_id
                )
        );
    }

    ws_name_to_id.insert(pair<string, int>(water_sources.back()->name,
                                           water_sources.back()->id));

    return ws_id < water_sources.size();
}

int MasterSystemInputFileParser::parseBlocks(ifstream &inputFile, int l,
                                             string &line,
                                             vector<vector<vector<string>>> &blocks,
                                             vector<int> &line_nos,
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

vector<vector<string>>
MasterSystemInputFileParser::readBlock(ifstream &inputFile, int &l,
                                       string &line) {
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

    for (auto up : utility_parsers) {
        delete up;
    }
    utility_parsers.clear();
    for (auto u : utilities) {
        delete u;
    }
    utilities.clear();
}

const vector<WaterSource *> &
MasterSystemInputFileParser::getWaterSources() const {
    return water_sources;
}

const vector<Utility *> &MasterSystemInputFileParser::getUtilities() const {
    return utilities;
}

const Graph &MasterSystemInputFileParser::getWaterSourcesGraph() const {
    return water_sources_graph;
}
