//
// Created by Bernardo on 12/2/2019.
//

#ifndef TRIANGLEMODEL_LAKEMICHAELRESERVOIRCONTROLPARSER_H
#define TRIANGLEMODEL_LAKEMICHAELRESERVOIRCONTROLPARSER_H


#include "../Base/ReservoirControlRuleParser.h"

class LakeMichaelReservoirControlParser : public ReservoirControlRuleParser {
private:
    Catchment lillington_gage_catchment;
    vector<vector<double>> lillington_gage_streamflows;
    double rel02 = NON_INITIALIZED;
    double rel04 = NON_INITIALIZED;
    double rel06_up = NON_INITIALIZED;
    double min_rel_llt06 = NON_INITIALIZED;
    double min_rel_llt08 = NON_INITIALIZED;
    double min_rel_llt08_up = NON_INITIALIZED;
    double lake_michael_supply_capacity = NON_INITIALIZED;
    double lake_michael_wq_capacity = NON_INITIALIZED;
public:
    explicit LakeMichaelReservoirControlParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations,
                        int n_weeks, int line_no,
                        const map<string, int> &ws_name_to_id,
                        const map<string, int> &utility_name_to_id) override;

    MinEnvFlowControl *
    generateReservoirControlRule(vector<vector<string>> &block,
                                 int line_no, int n_realizations, int n_weeks,
                                 const map<string, int> &ws_name_to_id,
                                 const map<string, int> &utility_name_to_id) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector<vector<string>> &block) override;

};


#endif //TRIANGLEMODEL_LAKEMICHAELRESERVOIRCONTROLPARSER_H
