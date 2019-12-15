//
// Created by Bernardo on 12/2/2019.
//

#include "LakeMichaelReservoirControlParser.h"
#include "../../Utils/Utils.h"
#include "../AuxParserFunctions.h"
#include "../Exceptions/MissingParameter.h"
#include "../../Controls/Custom/JordanLakeMinEnvFlowControl.h"

LakeMichaelReservoirControlParser::LakeMichaelReservoirControlParser()
        : ReservoirControlRuleParser(
        "[LAKE MICHAEL RESERVOIR CONTROL RULE CUSTOM]") {}

void
LakeMichaelReservoirControlParser::parseVariables(vector<vector<string>> &block,
                                                  int n_realizations,
                                                  int n_weeks, int line_no,
                                                  const map<string, int> &ws_name_to_id,
                                                  const map<string, int> &utility_name_to_id,
                                                  map<string, vector<vector<double>>> &pre_loaded_data) {
    ReservoirControlRuleParser::parseVariables(block, n_realizations, n_weeks,
                                               line_no, ws_name_to_id,
                                               utility_name_to_id,
                                               pre_loaded_data);

    vector<unsigned long> rows_read;
    for (unsigned long i = 0; i < block.size(); ++i) {
        vector<string> &line = block[i];
        if (line[0] == "wq_lims") {
            vector<double> lims;
            Utils::tokenizeString(line[1], lims, ',');
            rel02 = lims[0];
            rel04 = lims[1];
            rel06_up = lims[2];
            rows_read.push_back(i);
        } else if (line[0] == "lillington_lims") {
            vector<double> lims;
            Utils::tokenizeString(line[1], lims, ',');
            min_rel_llt06 = lims[0];
            min_rel_llt08 = lims[1];
            min_rel_llt08_up = lims[2];
            rows_read.push_back(i);
        } else if (line[0] == "lake_michael_supply_capacity") {
            lake_michael_supply_capacity = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "lake_michael_wq_capacity") {
            lake_michael_wq_capacity = stod(line[1]);
            rows_read.push_back(i);
        } else if (line[0] == "lillington_gage_catchment") {
            lillington_gage_catchment = Catchment(pre_loaded_data.at(line[1]),
                                                  n_weeks +
                                                  (int) (Constants::WEEKS_IN_YEAR *
                                                         Constants::NUMBER_REALIZATIONS_ROF));
            rows_read.push_back(i);
        }
    }

    AuxParserFunctions::cleanBlock(block, rows_read);
}

MinEnvFlowControl *
LakeMichaelReservoirControlParser::generateReservoirControlRule(
        vector<vector<string>> &block,
        int line_no,
        int n_realizations,
        int n_weeks,
        const map<string, int> &ws_name_to_id,
        const map<string, int> &utility_name_to_id,
        map<string, vector<vector<double>>> &pre_loaded_data) {
    parseVariables(block, n_realizations, n_weeks, line_no, ws_name_to_id,
                   utility_name_to_id, pre_loaded_data);
    checkMissingOrExtraParams(line_no, block);

    return new JordanLakeMinEnvFlowControl(
            water_source_id, lillington_gage_catchment, rel02, rel04, rel06_up,
            min_rel_llt06, min_rel_llt08, min_rel_llt08_up,
            lake_michael_wq_capacity, lake_michael_supply_capacity);
}

void LakeMichaelReservoirControlParser::checkMissingOrExtraParams(int line_no,
                                                                  vector<vector<string>> &block) {
    ReservoirControlRuleParser::checkMissingOrExtraParams(line_no, block);

    if (rel02 == NON_INITIALIZED)
        throw MissingParameter("rel02", tag, line_no);
    if (rel04 == NON_INITIALIZED)
        throw MissingParameter("rel04", tag, line_no);
    if (rel06_up == NON_INITIALIZED)
        throw MissingParameter("rel06_up", tag, line_no);
    if (min_rel_llt06 == NON_INITIALIZED)
        throw MissingParameter("min_rel_llt06", tag, line_no);
    if (min_rel_llt08 == NON_INITIALIZED)
        throw MissingParameter("min_rel_llt08", tag, line_no);
    if (min_rel_llt08_up == NON_INITIALIZED)
        throw MissingParameter("min_rel_llt08_up", tag, line_no);
    if (lake_michael_supply_capacity == NON_INITIALIZED)
        throw MissingParameter("lake_michael_supply_capacity", tag, line_no);
    if (lake_michael_wq_capacity == NON_INITIALIZED)
        throw MissingParameter("lake_michael_wq_capacity", tag, line_no);
    if (lillington_gage_catchment.getSeriesLength() == NON_INITIALIZED)
        throw MissingParameter("lillington_gage_catchment", tag, line_no);
}
