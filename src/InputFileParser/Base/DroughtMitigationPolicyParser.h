//
// Created by Bernardo on 11/29/2019.
//

#ifndef TRIANGLEMODEL_DROUGHTMITIGATIONPOLICYPARSER_H
#define TRIANGLEMODEL_DROUGHTMITIGATIONPOLICYPARSER_H

//#include <vector>
#include <string>
#include <map>
#include "../../DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h"

//using namespace std;

class DroughtMitigationPolicyParser {
protected:
    vector<int> apply_to_utilities;
    const string tag_name;

public:
    explicit DroughtMitigationPolicyParser(string tag_name);

    virtual ~DroughtMitigationPolicyParser();

    /**
     * parameters:
     * applies_to_utilities: to which utilities this policy will apply.
     *
     * @param block list of parameter and their values for that system component.
     * @param n_realizations
     * @param n_weeks
     * @param utilities_graph graph representing pipes connecting utilities.
     * @param ws_name_to_id map mapping names of water sources to corresponding IDs.
     */
    virtual void
    parseVariables(vector<vector<string>> &block,
                   int n_realizations, int n_weeks, int line_no,
                   Graph &utilities_graph,
                   Graph &ws_graph,
                   const map<string, int> &utility_name_to_id,
                   const map<string, int> &ws_name_to_id,
                   map<string, vector<vector<double>>> &pre_loaded_data);

    virtual void
    checkMissingOrExtraParams(int line_no, vector<vector<string>> &block);

    virtual DroughtMitigationPolicy *
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
                   map<string, vector<vector<double>>> &pre_loaded_data) = 0;
};


#endif //TRIANGLEMODEL_DROUGHTMITIGATIONPOLICYPARSER_H
