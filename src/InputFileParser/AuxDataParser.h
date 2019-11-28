//
// Created by bernardo on 11/27/19.
//

#ifndef TRIANGLEMODEL_AUXDATAPARSER_H
#define TRIANGLEMODEL_AUXDATAPARSER_H


#include "../Utils/Graph/Graph.h"

class AuxDataParser {
public:
    static Graph parseGraph(vector<vector<string>> &block,
            map<string, int> &name_to_id, const string &tag, int l);

    static vector<vector<int>> parseReservoirUtilityConnectivityMatrix(
            vector<vector<string>> &block, const map<string, int> &ws_name_to_id,
            const map<string, int> &utility_name_to_id, const string &tag, int l);

    static void replaceNameById(vector<vector<string>> &block, const string &tag_name, int line_no,
                                string param_name, int param_pos, map<string, int> name_to_id);
};


#endif //TRIANGLEMODEL_AUXDATAPARSER_H
