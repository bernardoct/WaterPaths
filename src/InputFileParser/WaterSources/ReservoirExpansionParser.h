//
// Created by Bernardo on 11/27/2019.
//

#ifndef TRIANGLEMODEL_RESERVOIREXPANSIONPARSER_H
#define TRIANGLEMODEL_RESERVOIREXPANSIONPARSER_H


#include "../Base/WaterSourceParser.h"

class ReservoirExpansionParser : public WaterSourceParser {

    unsigned long parent_reservoir_ID;

public:
    explicit ReservoirExpansionParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks) override;

    WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no, int n_realizations, int n_weeks) override;

    void checkMissingOrExtraParams(int line_no, vector<vector<string>> &block) override;

};


#endif //TRIANGLEMODEL_RESERVOIREXPANSIONPARSER_H
