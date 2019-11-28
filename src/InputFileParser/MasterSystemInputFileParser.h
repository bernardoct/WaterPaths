//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
#define TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H

#include <string>
#include <vector>
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "Base/WaterSourceParser.h"
#include "Utility/UtilityParser.h"
#include "../Utils/Graph/Graph.h"

using namespace std;

class MasterSystemInputFileParser {

    vector<WaterSourceParser *> water_source_parsers;
    vector<WaterSource *> water_sources;
    vector<UtilityParser *> utility_parsers;
    vector<Utility *> utilities;

    Graph water_sources_graph;
    Graph utilities_graph;

    int simulation_time = NON_INITIALIZED;

    map<string, int> name_to_id;
public:
    ~MasterSystemInputFileParser();

    void parseFile(string file_path);

    vector<vector<string>> readBlock(ifstream &inputFile, int &l, string &line);

    int
    parseBlocks(ifstream &inputFile, int l, string &line, vector<vector<vector<string>>> &blocks, vector<int> &line_nos,
                vector<string> &tags);

    bool createWaterSource(int l, const vector<int> &realizations_weeks, vector<vector<string>> &blocks,
                           const string &tag);

    void replaceNameById(vector<vector<string>> &block, const string &tag_name, int line_no, string param_name, int param_pos);

    const vector<WaterSource *>& getWaterSources() const;

    const vector<Utility *>& getUtilities() const;

    const Graph &getWaterSourcesGraph() const;

    bool createUtility(int l, const vector<int> &realizations_weeks, vector<vector<string>> &block,
                       const string &tag);

    bool
    createWaterSourceGraph(int l, const vector<int> &realizations_weeks, vector<vector<string>> &block,
                           const string &tag);
};


#endif //TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
