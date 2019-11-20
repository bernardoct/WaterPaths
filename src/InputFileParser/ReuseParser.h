//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_REUSEPARSER_H
#define TRIANGLEMODEL_REUSEPARSER_H


#include "Base/WaterSourceParser.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"

class ReuseParser : public WaterSourceParser {
private:
    const int EXISTING_REUSE_STATION = 0;
    const int NON_EXISTING_REUSE_STATION = 1;

    string name;
    double capacity;
    int constructor_type;

public:
    void parseVariables(vector<vector<string>> &block);

    WaterSource* generateSource(vector<vector<string>> &block);
};


#endif //TRIANGLEMODEL_REUSEPARSER_H
