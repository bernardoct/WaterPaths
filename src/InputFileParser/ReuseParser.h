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

    double capacity = NON_INITIALIZED;
    int constructor_type = NON_INITIALIZED;

public:
    void parseVariables(vector<vector<string>> &block) override;

    WaterSource* generateSource(vector<vector<string>> &block) override;
};


#endif //TRIANGLEMODEL_REUSEPARSER_H
