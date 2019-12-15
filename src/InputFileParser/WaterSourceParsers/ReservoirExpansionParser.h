//
// Created by Bernardo on 11/27/2019.
//

#ifndef TRIANGLEMODEL_RESERVOIREXPANSIONPARSER_H
#define TRIANGLEMODEL_RESERVOIREXPANSIONPARSER_H


#include "../Base/WaterSourceParser.h"

class ReservoirExpansionParser : public WaterSourceParser {

    unsigned long parent_reservoir_ID = NON_INITIALIZED;

public:
    explicit ReservoirExpansionParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations,
                        int n_weeks, int line_no,
                        const map<string, int> &ws_name_to_id,
                        const map<string, int> &utility_name_to_id, map<string, vector<vector<double>>> &pre_loaded_data) override;

    WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no,
                   int n_realizations, int n_weeks,
                   const map<string, int> &ws_name_to_id,
                   const map<string, int> &utility_name_to_id, map<string, vector<vector<double>>> &pre_loaded_data) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector<vector<string>> &block) override;

    void
    preProcessBlock(vector<vector<string>> &block, const string &tag, int l,
                    const map<string, int> &ws_name_to_id);
};


#endif //TRIANGLEMODEL_RESERVOIREXPANSIONPARSER_H
