//
// Created by Bernardo on 11/30/2019.
//

#ifndef TRIANGLEMODEL_SEASONALRESERVOIRCONTROLPARSER_H
#define TRIANGLEMODEL_SEASONALRESERVOIRCONTROLPARSER_H


#include "../Base/ReservoirControlRuleParser.h"

class SeasonalReservoirControlParser : public ReservoirControlRuleParser {
protected:
    vector<int> week_thresholds;
    vector<double> releases;

public:

    explicit SeasonalReservoirControlParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations,
                        int n_weeks, int line_no,
                        const map<string, int> &ws_name_to_id,
                        const map<string, int> &utility_name_to_id,
                        map<string, vector<vector<double>>> &pre_loaded_data) override;

    MinEnvFlowControl *
    generateReservoirControlRule(vector<vector<string>> &block,
                                 int line_no, int n_realizations, int n_weeks,
                                 const map<string, int> &ws_name_to_id,
                                 const map<string, int> &utility_name_to_id,
                                 map<string, vector<vector<double>>> &pre_loaded_data) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector<vector<string>> &block) override;
};


#endif //TRIANGLEMODEL_SEASONALRESERVOIRCONTROLPARSER_H
