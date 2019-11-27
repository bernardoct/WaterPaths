//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_REUSEPARSER_H
#define TRIANGLEMODEL_REUSEPARSER_H


#include "../Base/WaterSourceParser.h"
#include "../../SystemComponents/WaterSources/WaterReuse.h"

class ReuseParser : public WaterSourceParser {
private:
    const int EXISTING_REUSE_STATION = 0;
    const int NON_EXISTING_REUSE_STATION = 1;

    int constructor_type = NON_INITIALIZED;

public:
    explicit ReuseParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks) override;

    WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no, int n_realizations, int n_weeks) override;

    void checkMissingOrExtraParams(int line_no, vector<vector<string>> &block) override;
};


#endif //TRIANGLEMODEL_REUSEPARSER_H
