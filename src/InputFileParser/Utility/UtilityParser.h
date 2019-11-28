//
// Created by bernardo on 11/27/19.
//

#ifndef TRIANGLEMODEL_UTILITYPARSER_H
#define TRIANGLEMODEL_UTILITYPARSER_H

#include <vector>
#include <string>
#include "../../SystemComponents/Utility/Utility.h"

class UtilityParser {
protected:
    double percent_contingency_fund_contribution = NON_INITIALIZED;
    double bond_interest_rate = NON_INITIALIZED;
    double bond_term = NON_INITIALIZED;
    double demand_buffer = NON_INITIALIZED;
    double infra_discount_rate = NON_INITIALIZED;
    int number_of_week_demands = NON_INITIALIZED;
    vector<int> demand_infra_construction_order;
    vector<int> rof_infra_construction_order;
    vector<vector<int>> infra_if_built_remove;
    vector<double> infra_construction_triggers;
    vector<vector<double>> typesMonthlyDemandFraction;
    vector<vector<double>> typesMonthlyWaterPrice;
    vector<vector<double>> demands_all_realizations;
    string name;
    WwtpDischargeRule wwtp_discharge_rule;

public:

    const string tag_name = "[UTILITY]";

    explicit UtilityParser();

    ~UtilityParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations);

    Utility *
    generateUtility(int id, vector<vector<string>> &block, int line_no, int n_realizations);

    void checkMissingOrExtraParams(vector<vector<string>> &block, int line_no);
};


#endif //TRIANGLEMODEL_UTILITYPARSER_H
