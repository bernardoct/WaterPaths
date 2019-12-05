//
// Created by Bernardo on 11/29/2019.
//

#ifndef TRIANGLEMODEL_FIXEDFLOWRESERVOIRCONTROLPARSER_H
#define TRIANGLEMODEL_FIXEDFLOWRESERVOIRCONTROLPARSER_H


#include "../Base/ReservoirControlRuleParser.h"
#include <map>

class FixedFlowReservoirControlParser : public ReservoirControlRuleParser {
protected:
    double release = NON_INITIALIZED;
public:
    explicit FixedFlowReservoirControlParser();

    void parseVariables(vector <vector<string>> &block, int n_realizations,
                        int n_weeks, int line_no,
                        const map<string, int> &ws_name_to_id,
                        const map<string, int> &utility_name_to_id) override;

    MinEnvFlowControl *
    generateReservoirControlRule(vector <vector<string>> &block,
                                 int line_no, int n_realizations, int n_weeks,
                                 const map<string, int> &ws_name_to_id,
                                 const map<string, int> &utility_name_to_id) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector <vector<string>> &block) override;

    double getRelease() const;
};


#endif //TRIANGLEMODEL_FIXEDFLOWRESERVOIRCONTROLPARSER_H
