//
// Created by Bernardo on 11/30/2019.
//

#ifndef TRIANGLEMODEL_STORAGEMINENVFLOWCONTROLPARSER_H
#define TRIANGLEMODEL_STORAGEMINENVFLOWCONTROLPARSER_H


#include "../Base/ReservoirControlRuleParser.h"

class StorageMinEnvFlowControlParser : ReservoirControlRuleParser {
protected:
    vector<double> storages;
    vector<double> releases;

public:

    explicit StorageMinEnvFlowControlParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations,
                        int n_weeks, int line_no,
                        const map<string, int> &ws_name_to_id,
                        const map<string, int> &utility_name_to_id,
                        map<string, vector<vector<double>>> &pre_loaded_data) override;

    MinEnvFlowControl *
    generateReservoirControlRule(vector <vector<string>> &block,
                                 int line_no, int n_realizations, int n_weeks,
                                 const map<string, int> &ws_name_to_id,
                                 const map<string, int> &utility_name_to_id,
                                 map<string, vector<vector<double>>> &pre_loaded_data) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector <vector<string>> &block) override;
};


#endif //TRIANGLEMODEL_STORAGEMINENVFLOWCONTROLPARSER_H
