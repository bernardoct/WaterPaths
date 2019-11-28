//
// Created by bernardo on 11/27/19.
//

#ifndef TRIANGLEMODEL_AUXDATAPARSER_H
#define TRIANGLEMODEL_AUXDATAPARSER_H


#include "../Utils/Graph/Graph.h"

class AuxDataParser {
public:
    static Graph parseGraph(vector<vector<string>> &block);
};


#endif //TRIANGLEMODEL_AUXDATAPARSER_H
