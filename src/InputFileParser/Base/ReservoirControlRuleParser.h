//
// Created by Bernardo on 11/29/2019.
//

#ifndef TRIANGLEMODEL_RESERVOIRCONTROLRULEPARSER_H
#define TRIANGLEMODEL_RESERVOIRCONTROLRULEPARSER_H

//#include <vector>
#include <string>
#include "../../Controls/Base/MinEnvFlowControl.h"

using namespace std;

class ReservoirControlRuleParser {
protected:
    int water_source_id = NON_INITIALIZED;
    vector<int> aux_water_sources_id;
    vector<int> aux_utilities_ids;
public:
    const string tag;

    ReservoirControlRuleParser(string tag);

    virtual void parseVariables(vector<vector<string>> &block,
                                int n_realizations, int n_weeks, int line_no,
                                const map<string, int> &ws_name_to_id,
                                const map<string, int> &utility_name_to_id,
                                map<string, vector<vector<double>>> &pre_loaded_data);

    virtual MinEnvFlowControl *
    generateReservoirControlRule(vector<vector<string>> &block,
                                 int line_no,
                                 int n_realizations, int n_weeks,
                                 const map<string, int> &ws_name_to_id,
                                 const map<string, int> &utility_name_to_id,
                                 map<string, vector<vector<double>>> &pre_loaded_data) = 0;

    virtual void checkMissingOrExtraParams(int line_no,
                                           vector<vector<string>> &block);

};


#endif //TRIANGLEMODEL_RESERVOIRCONTROLRULEPARSER_H
