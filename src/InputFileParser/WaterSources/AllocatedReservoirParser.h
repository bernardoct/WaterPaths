//
// Created by Bernardo on 11/25/2019.
//

#ifndef TRIANGLEMODEL_ALLOCATEDRESERVOIRPARSER_H
#define TRIANGLEMODEL_ALLOCATEDRESERVOIRPARSER_H


#include "ReservoirParser.h"

class AllocatedReservoirParser : public ReservoirParser {
public:
    AllocatedReservoirParser();

    WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no, int n_realizations, int n_weeks) override;

    void checkMissingOrExtraParams(int line_no, vector<vector<string>> &block) override;
};


#endif //TRIANGLEMODEL_ALLOCATEDRESERVOIRPARSER_H
