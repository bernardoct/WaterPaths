//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
#define TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H

#include <string>
#include <vector>
#include "../SystemComponents/WaterSources/Base/WaterSource.h"

using namespace std;

class MasterSystemInputFileParser {

    vector<WaterSource *> water_sources;
    int simulation_time = NON_INITIALIZED;
public:
    ~MasterSystemInputFileParser();

    void parseFile(string file_path);

    const vector<WaterSource *>& getWaterSources() const;

    vector<vector<string>> readBlock(ifstream &inputFile, int &l) const;
};


#endif //TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
