//
// Created by Bernardo on 11/29/2019.
//

#ifndef TRIANGLEMODEL_TRANSFERSPARSER_H
#define TRIANGLEMODEL_TRANSFERSPARSER_H


#include "../Base/DroughtMitigationPolicyParser.h"

class TransfersParser : public DroughtMitigationPolicyParser {
    int source_utility_id = NON_INITIALIZED;
    int transfer_water_source_id = NON_INITIALIZED;
    double source_treatment_buffer = NON_INITIALIZED;
    Graph utilities_graph;
    vector<double> pipe_transfer_capacities;
    vector<double> buyers_transfer_triggers;
    vector<int> pipe_owner;
    vector<double> conveyance_costs;

public:
    TransfersParser();

    /**
     * Parameters are:
     * applies_to_utilities: to which utilities this policy will apply.
     * source_utility_id: name of utility providing transfers
     * transfer_water_source_id: name of infrastructure providing transfer water. Source_utility_id must have access to it.
     * source_treatment_buffer: percentage of source utility's treatment capacity to be left unused.
     * pipe_transfer_capacities: pipe maximum flow conveyance in volume per time.
     * buyers_transfer_triggers: rof triggers prompting request from buyers.
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
                        const map<string, int> &utility_name_to_id) override;

    void checkMissingOrExtraParams(int line_no,
                                   vector<vector<string>> &block) override;

    DroughtMitigationPolicy *
    generateSource(int id, vector<vector<string>> &block, int line_no,
                   int n_realizations, int n_weeks,
                   const map<string, int> &ws_name_to_id,
                   const map<string, int> &utility_name_to_id, Graph &utilities_graph, Graph &ws_graph,
                   const vector<vector<int>> &water_sources_to_utilities,
                   vector<Utility *> &utilities,
                   vector<WaterSource *> &water_source,
                   vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                   vector<MinEnvFlowControl *> min_env_flow_controls,
                   vector<vector<double>>& utilities_rdm,
                   vector<vector<double>>& water_sources_rdm,
                   vector<vector<double>>& policy_rdm) override;

    void preProcessBlock(vector<vector<string>> &block, const string &tag_name,
                         int line_no, const map<string, int> &utility_name_to_id);
};

#endif //TRIANGLEMODEL_TRANSFERSPARSER_H
