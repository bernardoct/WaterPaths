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

    void parseVariables(vector<vector<string>> &block, int n_realizations,
                        int n_weeks, int line_no,
                        const map<string, int> &ws_name_to_id,
                        const map<string, int> &utility_name_to_id) override;

    WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no,
                   int n_realizations, int n_weeks,
                   const map<string, int> &ws_name_to_id,
                   const map<string, int> &utility_name_to_id) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector<vector<string>> &block) override;
};


#endif //TRIANGLEMODEL_REUSEPARSER_H
