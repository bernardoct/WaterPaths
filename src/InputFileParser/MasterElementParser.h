//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_MASTERELEMENTPARSER_H
#define TRIANGLEMODEL_MASTERELEMENTPARSER_H

#include <string>
#include <vector>
#include "../SystemComponents/WaterSources/Base/WaterSource.h"

using namespace std;

class MasterElementParser {

    vector<WaterSource *> water_sources;
public:
    ~MasterElementParser();

    void parseFile(string file_path);

};


#endif //TRIANGLEMODEL_MASTERELEMENTPARSER_H
