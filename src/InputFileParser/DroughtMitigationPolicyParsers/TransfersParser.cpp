//
// Created by Bernardo on 11/29/2019.
//

#include "TransfersParser.h"
#include "../../Utils/Utils.h"
#include "../Exceptions/MissingParameter.h"
#include "../../DroughtMitigationInstruments/Transfers.h"
#include "../AuxParserFunctions.h"

TransfersParser::TransfersParser() : DroughtMitigationPolicyParser(
        "[TRANSFERS POLICY]") {}


void TransfersParser::parseVariables(vector<vector<string>> &block,
                                     int n_realizations, int n_weeks,
                                     int line_no,
                                     Graph &utilities_graph,
                                     Graph &ws_graph,
                                     const map<string, int> &utility_name_to_id) {
    preProcessBlock(block, tag_name, line_no, utility_name_to_id);
    DroughtMitigationPolicyParser::parseVariables(block, n_realizations,
                                                  n_weeks, line_no,
                                                  utilities_graph, ws_graph,
                                                  utility_name_to_id);

    vector<unsigned long> rows_read(0);
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> line = block[i];
        if (line[0] == "source_utility_id") {
            source_utility_id = stoi(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "transfer_water_source_id") {
            transfer_water_source_id = stoi(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "source_treatment_buffer") {
            source_treatment_buffer = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "pipe_transfer_capacities") {
            Utils::tokenizeString(line[1], pipe_transfer_capacities, ',');
            rows_read.push_back(i);
        } else if (line[0] == "buyers_transfer_triggers") {
            Utils::tokenizeString(line[1], buyers_transfer_triggers, ',');
            rows_read.push_back(i);
//        TODO: UNCOMMENT THOSE WHEN THEY'RE IMPLEMENTED IN TRANSFERS CLASS.
//        } else if (line[0] == "pipe_owner") {
//            Utils::tokenizeString(line[1], pipe_owner, ',');
//            rows_read.push_back(i);
//        } else if (line[0] == "conveyance_costs") {
//            Utils::tokenizeString(line[1], conveyance_costs, ',');
//            rows_read.push_back(i);
        }
        this->utilities_graph = utilities_graph;
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

void TransfersParser::checkMissingOrExtraParams(int line_no,
                                                vector<vector<string>> &block) {
    DroughtMitigationPolicyParser::checkMissingOrExtraParams(line_no, block);

    if (source_utility_id == NON_INITIALIZED) {
        throw MissingParameter("source_utility_id", tag_name, line_no);
    } else if (transfer_water_source_id == NON_INITIALIZED) {
        throw MissingParameter("transfer_water_source_id", tag_name, line_no);
    } else if (source_treatment_buffer == NON_INITIALIZED) {
        throw MissingParameter("source_treatment_buffer", tag_name, line_no);
    } else if (pipe_transfer_capacities.empty()) {
        throw MissingParameter("pipe_transfer_capacities", tag_name, line_no);
    } else if (buyers_transfer_triggers.empty()) {
        throw MissingParameter("buyers_transfer_triggers", tag_name, line_no);
    }
}

DroughtMitigationPolicy *
TransfersParser::generateSource(int id, vector<vector<string>> &block,
                                int line_no, int n_realizations, int n_weeks,
                                const map<string, int> &ws_name_to_id,
                                const map<string, int> &utility_name_to_id,
                                Graph &utilities_graph, Graph &ws_graph,
                                const vector<vector<int>> &water_sources_to_utilities,
                                vector<Utility *> &utilities,
                                vector<WaterSource *> &water_source,
                                vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                                vector<MinEnvFlowControl *> min_env_flow_controls,
                                vector<vector<double>>& utilities_rdm,
                                vector<vector<double>>& water_sources_rdm,
                                vector<vector<double>>& policy_rdm) {
    parseVariables(block, n_realizations, n_weeks, line_no,
                   utilities_graph, ws_graph, utility_name_to_id);
    checkMissingOrExtraParams(line_no, block);

    return new Transfers(id, source_utility_id, transfer_water_source_id,
                         source_treatment_buffer, apply_to_utilities,
                         pipe_transfer_capacities, buyers_transfer_triggers,
                         utilities_graph, conveyance_costs, pipe_owner);
}

void TransfersParser::preProcessBlock(vector<vector<string>> &block,
                                      const string &tag_name, int line_no,
                                      const map<string, int> &utility_name_to_id) {
    AuxParserFunctions::replaceNameById(block, tag_name, line_no,
                                        "source_utility_id", 1,
                                        utility_name_to_id);
    AuxParserFunctions::replaceNameById(block, tag_name, line_no,
                                        "transfer_water_source_id", 1,
                                        utility_name_to_id);
}
