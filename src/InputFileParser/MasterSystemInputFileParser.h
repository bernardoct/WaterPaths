//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
#define TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H

#include <string>
#include <vector>
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "Base/WaterSourceParser.h"

using namespace std;

class MasterSystemInputFileParser {

    vector<WaterSourceParser *> water_source_parsers;
    vector<WaterSource *> water_sources;
    int simulation_time = NON_INITIALIZED;
public:
    ~MasterSystemInputFileParser();

    void parseFile(string file_path);

    const vector<WaterSource *>& getWaterSources() const;

    vector<vector<string>> readBlock(ifstream &inputFile, int &l, string &line);

    int
    parseBlocks(ifstream &inputFile, int l, string &line, vector<vector<vector<string>>> &blocks, vector<int> &line_nos,
                vector<string> &tags);

    bool createWaterSource(int l, const vector<int> &realizations_weeks, vector<vector<string>> &blocks,
                           const string &tag);
};


#endif //TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
