//
// Created by Bernardo on 11/30/2019.
//

#ifndef TRIANGLEMODEL_INFLOWBASEDRESERVOIRCONTROLPARSER_H
#define TRIANGLEMODEL_INFLOWBASEDRESERVOIRCONTROLPARSER_H


#include "../Base/ReservoirControlRuleParser.h"

class InflowBasedReservoirControlParser : public ReservoirControlRuleParser {
protected:
    vector<double> inflows;
    vector<double> releases;
public:
    explicit InflowBasedReservoirControlParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations,
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

    const vector<double> &getInflows() const;

    const vector<double> &getReleases() const;
};


#endif //TRIANGLEMODEL_INFLOWBASEDRESERVOIRCONTROLPARSER_H
