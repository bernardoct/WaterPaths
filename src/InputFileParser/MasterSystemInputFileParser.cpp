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
            tag_read = (createUtility(l, realizations_weeks, blocks[t], tags[t]) ? true : tag_read);
            tag_read = (createWaterSourceGraph(l, realizations_weeks, blocks[t], tags[t]) ? true : tag_read);

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

bool MasterSystemInputFileParser::createWaterSourceGraph(int l, const vector<int> &realizations_weeks,
                                                         vector<vector<string>> &block,
                                                         const string &tag) {
    if (tag == "[WATER SOURCE GRAPH]") {
        for (auto line : block) {
            replaceNameById(block, tag, l, line[0], 0);
        }
        water_sources_graph = AuxDataParser::parseGraph(block);
        return true;
    }

    return false;
}

bool MasterSystemInputFileParser::createUtility(int l, const vector<int> &realizations_weeks,
                                                vector<vector<string>> &block,
                                                const string &tag) {
    if (tag == "[UTILITY]") {
        int utility_id = (int) utilities.size();

        replaceNameById(block, "[UTILITY]", l, "wwtp_discharge_rule", 2);
        replaceNameById(block, "[UTILITY]", l, "rof_infra_construction_order", 1);
        replaceNameById(block, "[UTILITY]", l, "demand_infra_construction_order", 1);

        utility_parsers.push_back(new UtilityParser());
        utilities.push_back(utility_parsers.back()->generateUtility(utility_id,
                                                                    block, l, realizations_weeks[0]));

        return true;
    }

    return false;
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
    } else if (tag == "[RESERVOIR]") {
        water_source_parsers.push_back(new ReservoirParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1])
        );
    } else if (tag == "[ALLOCATED RESERVOIR]") {
        water_source_parsers.push_back(new AllocatedReservoirParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1])
        );
    } else if (tag == "[RESERVOIR EXPANSION]") {
        replaceNameById(block, tag, l, "parent_reservoir", 1);
        water_source_parsers.push_back(new ReservoirExpansionParser());
        water_sources.push_back(
                water_source_parsers.back()->generateSource(
                        ws_id, block, l, realizations_weeks[0],
                        realizations_weeks[1]
                        )
        );
    }

    name_to_id.insert(pair<string, int>(water_sources.back()->name, water_sources.back()->id));

    return ws_id < water_sources.size();
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

void MasterSystemInputFileParser::replaceNameById(vector<vector<string>> &block,
        const string &tag_name, int line_no, string param_name, int param_pos) {
    for (vector<string> &line : block) {
        if (line[0] == param_name) {
            vector<string> names;
            Utils::tokenizeString(line[param_pos], names, ',');
            string ids;

            for (auto & name : names) {
                try {
                    name = to_string(name_to_id[name]) + ",";
                    ids += name;
                } catch (...) {
                    char error[256];
                    sprintf(error, "Could not find %s of %s in line %d. "
                                   "All %s must be added in the input file above "
                                   "associated %ss.", name.c_str(), param_name.c_str(), line_no,
                            param_name.c_str(), tag_name.c_str());
                    throw invalid_argument(error);
                }
            }
            ids.pop_back();
            line[param_pos] = ids;
        }
    }
}

const vector<WaterSource *>& MasterSystemInputFileParser::getWaterSources() const{
    return water_sources;
}

const vector<Utility *>& MasterSystemInputFileParser::getUtilities() const {
    return utilities;
}

const Graph &MasterSystemInputFileParser::getWaterSourcesGraph() const {
    return water_sources_graph;
}
