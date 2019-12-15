//
// Created by Bernardo on 11/29/2019.
//

#ifndef TRIANGLEMODEL_RESTRICTIONSPARSER_H
#define TRIANGLEMODEL_RESTRICTIONSPARSER_H


#include "../Base/DroughtMitigationPolicyParser.h"

class RestrictionsParser : public DroughtMitigationPolicyParser {

public:
    RestrictionsParser();

private:
    vector<double> stage_multipliers;
    vector<double> stage_triggers;
    vector<vector<double>> typesMonthlyDemandFraction;
    vector<vector<double>> typesMonthlyWaterPrice;
    vector<vector<double>> priceMultipliers;

public:

    /**
     * Parameters are:
     * applies_to_utilities: to which utilities this policy will apply.
     * stage_multipliers: percentage by which demand is decreased at each stage.
     * stage_triggers: rof triggers for each restriction stage.
     * typesMonthlyDemandFraction (optional): fractions of demand pertaining to each tier.
     * typesMonthlyWaterPrice (optional): water price for each demand tier.
     * priceMultipliers (optional): price multiplier for each demand tier under each restriction stage.
     *
     * For further documentation see documentation of WaterSourcesParser::parseVariables.
     * @param block
     * @param n_realizations
     * @param n_weeks
     * @param utilities_graph
     * @param ws_graph
     */
    void parseVariables(vector<vector<string>> &block,
                        int n_realizations, int n_weeks, int line_no,
                        Graph &utilities_graph,
                        Graph &ws_graph,
                        const map<string, int> &utility_name_to_id,
                        const map<string, int> &ws_name_to_id,
                        map<string, vector<vector<double>>> &pre_loaded_data) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector<vector<string>> &block) override;

    DroughtMitigationPolicy *
    generateSource(int id, vector<vector<string>> &block, int line_no,
                   int n_realizations, int n_weeks,
                   const map<string, int> &ws_name_to_id,
                   const map<string, int> &utility_name_to_id,
                   Graph &utilities_graph, Graph &ws_graph,
                   const vector<vector<int>> &water_sources_to_utilities,
                   vector<Utility *> &utilities,
                   vector<WaterSource *> &water_source,
                   vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                   vector<MinEnvFlowControl *> min_env_flow_controls,
                   vector<vector<double>> &utilities_rdm,
                   vector<vector<double>> &water_sources_rdm,
                   vector<vector<double>> &policy_rdm,
                   map<string, vector<vector<double>>> &pre_loaded_data) override;

};


#endif //TRIANGLEMODEL_RESTRICTIONSPARSER_H
