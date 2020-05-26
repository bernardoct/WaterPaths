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
    vector<double> infra_construction_triggers;
    vector<double> utility_owned_wtp_capacities;
    vector<vector<int>> infra_if_built_remove;
    vector<vector<int>> construction_pre_requisites;
    vector<vector<int>> water_source_to_wtp;
    vector<vector<double>> typesMonthlyDemandFraction;
    vector<vector<double>> typesMonthlyWaterPrice;
    vector<vector<double>> *demands_all_realizations = nullptr;
    string name;
    WwtpDischargeRule wwtp_discharge_rule;

public:

    const string tag_name = "[UTILITY]";

    explicit UtilityParser();

    ~UtilityParser();

    void parseVariables(vector<vector<string>> &block, int n_realizations, map<string, vector<vector<double>>> &pre_loaded_data);

    Utility *
    generateUtility(int id, vector<vector<string>> &block,
                    int line_no, int n_realizations,
                    const map<string, int> &ws_name_to_id, map<string, vector<vector<double>>> &pre_loaded_data);

    void checkMissingOrExtraParams(vector<vector<string>> &block, int line_no);

    void preProcessBlock(vector<vector<string>> &block, int l,
                         const map<string, int> &ws_name_to_id);
};


#endif //TRIANGLEMODEL_UTILITYPARSER_H
