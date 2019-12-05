//
// Created by Bernardo on 11/19/2019.
//

#include <cstring>
#include <algorithm>
#include "MasterSystemInputFileParser.h"
#include "WaterSourceParsers/ReuseParser.h"
#include "../Utils/Utils.h"
#include "WaterSourceParsers/ReservoirParser.h"
#include "WaterSourceParsers/AllocatedReservoirParser.h"
#include "WaterSourceParsers/ReservoirExpansionParser.h"
#include "Exceptions/MissingParameter.h"
#include "AuxDataParser.h"
#include "AuxParserFunctions.h"
#include "DroughtMitigationPolicyParsers/TransfersParser.h"
#include "DroughtMitigationPolicyParsers/RestrictionsParser.h"
#include "ReservoirControlRuleParsers/FixedFlowReservoirControlParser.h"
#include "ReservoirControlRuleParsers/InflowBasedReservoirControlParser.h"
#include "ReservoirControlRuleParsers/LakeMichaelReservoirControlParser.h"
#include "ReservoirControlRuleParsers/SeasonalReservoirControlParser.h"
#include "DroughtMitigationPolicyParsers/DroughtInsuranceParser.h"

MasterSystemInputFileParser::MasterSystemInputFileParser() {}

void MasterSystemInputFileParser::preloadAndCheckInputFile(string &input_file) {
    // read first line, blocks, tags, and tags' line numbers.
    parseFile(input_file);

    // Preload heady data so that it's not reloaded for every function
    // evaluation during and optimization exercise.
    auto it = find(tags.begin(), tags.end(), "[DATA TO LOAD]");
    int ix = distance(tags.begin(), it);
    parsePreloadedData(line_nos[ix], blocks[ix], tags[ix],
                       true, n_realizations);

    // loop through blocks to map of names utilities and water sources to their
    // to ids. Also checks for undefined tags and other errors.
    loopThroughTags(blocks, false);
    clearParsers();
}

void MasterSystemInputFileParser::createSystemObjects(double *vars) {

    if (!blocks.empty()) {
        vector<vector<vector<string>>> blocks_populated;
        if (vars != nullptr) {
            blocks_populated = replacePlaceHoldersByDVs(vars, blocks);
        } else {
            blocks_populated = blocks;
        }

        // loop through blocks again but now to create all objects.
        loopThroughTags(blocks_populated, true);

        // initialize standard RDM factors if they have not been loaded yet.
        if (rdm_dmp.empty()) {
            initializeStandardRDMFactors();
        }
    } else {
        throw invalid_argument("Data blocks must be populated before "
                               "createSystemObjects can be called.");
    }
}

/**
 * Fill in the gaps in the blocks left by the %%% and @ in the input file.
 * @param vars decision variables.
 * @param blocks data parsed from input file.
 * @return
 */
vector<vector<vector<string>>>
MasterSystemInputFileParser::replacePlaceHoldersByDVs(
        double *vars, vector<vector<vector<string>>> &blocks) {
    int count_var = 0;
    for (auto &block : blocks) {
        for (auto &line : block) {
            for (string &data : line)
            {
                // look for %%% and replaces them by next values in vars.
                unsigned long it;
                replaceNumericVarsIntoPlaceHolders(vars, count_var,data);

                // look for names beginning with @, reorder the names according
                // to values in vars, and removes all @s.
                it = data.find('@');
                if (it != string::npos) {
                    reorderCSVDataInBlockLine(vars, count_var, data);
                }
            }
        }
    }
}

/**
 * look for %%% and replaces them by next values in vars.
 * @param vars array with numeric values
 * @param count_var index of vars to be replace into line.
 * @param data piece of line
 * @return updated index of vars to be replace into line
 */
void MasterSystemInputFileParser::replaceNumericVarsIntoPlaceHolders(
        const double *vars, int &count_var, string &data) const {
    auto it = data.find("%%%");
    while (it != string::npos)
    {
        data.replace(it, 3, to_string(vars[count_var]));
        it = data.find("%%%");
        count_var++;
    }
}

/**
 * Checks with of the elements separated by commas in string data are preceded
 * by a @ and sorts them according to the values in vars. Any element in data
 * that is not preceded by @ will be left in the same position.
 * @param vars array with numeric values to be translated into positions
 * @param count_var index of vars to be used to sort data.
 * @param data piece of line
 */
void MasterSystemInputFileParser::reorderCSVDataInBlockLine(const double *vars,
                                                            int &count_var,
                                                            string &data) const {
    vector<string> to_be_ordered;
    vector<infraRank> order;
    Utils::tokenizeString(data, to_be_ordered, ',');
    vector<int> has_at;
    for (int k = 0; k < to_be_ordered.size(); ++k) {
        string sub_param = to_be_ordered[k];
        if (sub_param[0] == '@') {
            has_at.push_back(k);
            string name = sub_param.substr(1, sub_param.size());
            order.emplace_back(name, vars[count_var]);
            count_var++;
        }
    }
    sort(order.begin(), order.end(), by_xreal());
    for (unsigned long k = 0; k < has_at.size(); k++)
    {
        to_be_ordered[has_at[k]] = order[k].name;
    }

    data = "";
    for (auto &n : to_be_ordered) data += n + ',';
    data.pop_back();
}

void MasterSystemInputFileParser::initializeStandardRDMFactors() {
    // Simplification for one for each restriction.
    int n_rdms_dmp = (int) drought_mitigation_policy.size();
    // demand growth, bond interest, bond term, discount rate.
    int n_rdms_utility = 4;
    // evaporation multiplier, construction and permitting time multipliers.
    int n_rdms_ws = 1 + (int) water_sources.size() * 2;

    rdm_dmp = vector<vector<double>>(n_realizations,
                                     vector<double>(n_rdms_dmp, 1.));
    rdm_utilities = vector<vector<double>>(n_realizations,
                                           vector<double>(
                                                   n_rdms_utility, 1.));
    rdm_water_sources = vector<vector<double>>(n_realizations,
                                               vector<double>(n_rdms_ws,
                                                              1.));
}

MasterSystemInputFileParser::~MasterSystemInputFileParser() {
    clearParsers();
}

void MasterSystemInputFileParser::loopThroughTags(
        vector<vector<vector<string>>> blocks, bool create_objects) {
    for (unsigned long t = 0; t < tags.size(); ++t) {
        bool tag_read = false;
        // Check if tag is a water source and, if so, create corresponding water source.
        tag_read = (
                parseWaterSource(
                        line_nos[t], blocks[t], tags[t],
                        create_objects
                ) ? true : tag_read
        );
        tag_read = (
                parseUtility(
                        line_nos[t], blocks[t], tags[t],
                        create_objects
                ) ? true : tag_read
        );
        tag_read = (
                parseGraphsAndMatrices(
                        line_nos[t], blocks[t], tags[t],
                        create_objects
                ) ? true : tag_read
        );
        tag_read = (
                parseDroughtMitigationPolicies(
                        line_nos[t], blocks[t], tags[t],
                        create_objects
                ) ? true : tag_read
        );
        tag_read = (
                parseReservoirControlRules(
                        line_nos[t], blocks[t], tags[t],
                        create_objects
                ) ? true : tag_read
        );
        tag_read = (
                // preloaded data parser included here just to check the tag so
                // that exception below is not triggered.
                parsePreloadedData(line_nos[t], blocks[t], tags[t],
                                   true, n_realizations
                ) ? true : tag_read
        );

        if (!tag_read) {
            char error[128];
            sprintf(error, "Unrecognized tag %s on line %d",
                    tags[t].c_str(), line_nos[t]);
            throw invalid_argument(error);
        }
    }
}

void MasterSystemInputFileParser::parseFirstLine(const string &line) {
    vector<string> tok_line;
    Utils::tokenizeString(line, tok_line);

    if (tok_line.size() < 2) {
        throw invalid_argument("First line must contain at least the "
                               "numbers of realizations and weeks.");
    } else {
        try {
            n_realizations = stoi(tok_line[0]);
            n_weeks = stoi(tok_line[1]);
        } catch (...) {
            throw invalid_argument(
                    "First two items of the first line of input "
                    "file must contain only the numbers of "
                    "realizations and weeks.");
        }
    }

    if (tok_line.size() > 4) {
        rdm_utilities = Utils::parse2DCsvFile(tok_line[2],
                                              n_realizations);
        rdm_water_sources = Utils::parse2DCsvFile(tok_line[3],
                                                  n_realizations);
        rdm_dmp = Utils::parse2DCsvFile(tok_line[4], n_realizations);
    }

    if (tok_line.size() != 2 and tok_line.size() != 5)
        throw invalid_argument("First line must contain either just the number "
                               "of realizations and weeks, of both plus the "
                               "paths to utilities, water sources, and drought "
                               "mitigation policies rdm files.");
}

bool MasterSystemInputFileParser::parsePreloadedData(int line_no,
                                                     vector<vector<string>> &block,
                                                     const string &tag,
                                                     bool read_data,
                                                     int n_realizations) {
    if (tag == "[DATA TO LOAD]") {
        if (read_data) {
            for (auto line : block) {
                pre_loaded_data.insert(
                        {line[0],
                         Utils::parse2DCsvFile(line[1], n_realizations)});
            }
        }
        return true;
    }
    return false;
}

void MasterSystemInputFileParser::parseFile(string file_path) {
    ifstream inputFile(file_path);
    int line_no = 0; /// number of line being read.

    if (inputFile.is_open()) {
        string line;
        // Checks if file is not empty and gets number of realizations and weeks
        if (!getline(inputFile, line))
            throw invalid_argument("Empty input file.");
        line_no++;

        parseFirstLine(line);

        // Looks for first tag
        while (!getline(inputFile, line)) {
            line_no++;
            if (line[0] == '[') break;
        }

        parseBlocks(inputFile, line_no, line, blocks, line_nos, tags);
    } else {
        string error = "File " + file_path + " not found.";
        char error_char[error.size() + 1];
        strcpy(error_char, error.c_str());
        throw invalid_argument(error_char);
    }
}

bool MasterSystemInputFileParser::parseGraphsAndMatrices(int line_no,
                                                         vector<vector<string>> &block,
                                                         const string &tag,
                                                         bool create_objects) {

    if (tag == "[WATER SOURCES GRAPH]") {
        if (create_objects)
            water_sources_graph = AuxParserFunctions::parseGraph(block,
                                                                 ws_name_to_id,
                                                                 tag, line_no);
        return true;
    } else if (tag == "[UTILITIES GRAPH]") {
        if (create_objects)
            utilities_graph = AuxParserFunctions::parseGraph(block,
                                                             utility_name_to_id,
                                                             tag, line_no);
        return true;
    } else if (tag == "[WS TO UTILITY MATRIX]") {
        if (create_objects)
            reservoir_utility_connectivity_matrix =
                    AuxDataParser::parseReservoirUtilityConnectivityMatrix(
                            block, ws_name_to_id, utility_name_to_id, tag,
                            line_no);
        return true;
    } else if (tag == "[TABLE STORAGE SHIFT]") {
        if (create_objects)
            table_storage_shift = AuxDataParser::tableStorageShiftParser(block,
                                                                         ws_name_to_id,
                                                                         utility_name_to_id,
                                                                         tag,
                                                                         line_no);
        return true;
    }

    return false;
}

bool MasterSystemInputFileParser::parseUtility(int line_no,
                                               vector<vector<string>> &block,
                                               const string &tag,
                                               bool create_objects) {
    int utility_id = utility_parsers.size();

    if (tag == "[UTILITY]") {
        utility_parsers.push_back(new UtilityParser());

        if (create_objects) {
            utilities.push_back(utility_parsers.back()->generateUtility(
                    utility_id, block, line_no, n_realizations,
                    ws_name_to_id));
        } else {
            string name = findName(block, tag, line_no);
            utility_name_to_id.insert(pair<string, int>(name, utility_id));
        }

        return true;
    }

    return false;
}

bool MasterSystemInputFileParser::parseWaterSource(int line_no,
                                                   vector<vector<string>> &block,
                                                   const string &tag,
                                                   bool create_objects) {
    int water_source_id = water_source_parsers.size();

    if (tag == "[WATER REUSE]") {
        water_source_parsers.push_back(new ReuseParser());
    } else if (tag == "[RESERVOIR]") {
        water_source_parsers.push_back(
                new ReservoirParser(generate_rof_tables));
    } else if (tag == "[ALLOCATED RESERVOIR]") {
        water_source_parsers.push_back(
                new AllocatedReservoirParser(generate_rof_tables));
    } else if (tag == "[RESERVOIR EXPANSION]") {
        water_source_parsers.push_back(new ReservoirExpansionParser());
    }

    // Only create water source if a parser has been created, indicating the
    // block is a water source.
    if (water_source_id < water_source_parsers.size()) {
        if (create_objects) {
            water_sources.push_back(
                    water_source_parsers.back()->generateSource(
                            water_source_id, block, line_no, n_realizations,
                            n_weeks, ws_name_to_id, utility_name_to_id
                    )
            );
        } else {
            string name = findName(block, tag, line_no);
            ws_name_to_id.insert(pair<string, int>(name, water_source_id));
        }

        return true;
    }

    return false;
}

bool MasterSystemInputFileParser::parseReservoirControlRules(int line_no,
                                                             vector<vector<string>> &block,
                                                             const string &tag,
                                                             bool create_objects) {

    if (create_objects) {
        auto size_parsers = reservoir_control_rule_parse.size();
        if (tag == "[FIXED FLOW RESERVOIR CONTROL RULE]") {
            reservoir_control_rule_parse.push_back(
                    new FixedFlowReservoirControlParser());
        } else if (tag == "[INFLOW-BASED RESERVOIR CONTROL RULE]") {
            reservoir_control_rule_parse.push_back(
                    new InflowBasedReservoirControlParser());
        } else if (tag == "[SEASONAL RESERVOIR CONTROL RULE]") {
            reservoir_control_rule_parse.push_back(
                    new SeasonalReservoirControlParser());
        } else if (tag == "[LAKE MICHAEL RESERVOIR CONTROL RULE CUSTOM]") {
            reservoir_control_rule_parse.push_back(
                    new LakeMichaelReservoirControlParser());
        }

        if (size_parsers < reservoir_control_rule_parse.size()) {
            reservoir_control_rules.push_back(
                    reservoir_control_rule_parse.back()->generateReservoirControlRule(
                            block, line_no, n_realizations,
                            n_weeks, ws_name_to_id,
                            utility_name_to_id));
            return true;
        }

        return false;
    } else {
        return true;
    }
}

bool MasterSystemInputFileParser::parseDroughtMitigationPolicies(int line_no,
                                                                 vector<vector<string>> &block,
                                                                 const string &tag,
                                                                 bool create_objects) {
    int dmp_id = drought_mitigation_policy_parsers.size();

    if (tag == "[RESTRICTIONS POLICY]") {
        drought_mitigation_policy_parsers.push_back(new RestrictionsParser());
    } else if (tag == "[TRANSFERS POLICY]") {
        drought_mitigation_policy_parsers.push_back(new TransfersParser());
    } else if (tag == "[INSURANCE POLICY]") {
        drought_mitigation_policy_parsers.push_back(
                new DroughtInsuranceParser());
    }

    if (dmp_id < drought_mitigation_policy_parsers.size()) {
        if (create_objects) {
            drought_mitigation_policy.push_back(
                    drought_mitigation_policy_parsers.back()->generateSource(
                            dmp_id, block, line_no, n_realizations,
                            n_weeks,
                            ws_name_to_id, utility_name_to_id, utilities_graph,
                            water_sources_graph,
                            reservoir_utility_connectivity_matrix, utilities,
                            water_sources, drought_mitigation_policy,
                            reservoir_control_rules, rdm_utilities,
                            rdm_water_sources, rdm_dmp)
            );
        }

        return true;
    }

    return false;
}

int MasterSystemInputFileParser::parseBlocks(ifstream &inputFile, int l,
                                             string &line,
                                             vector<vector<vector<string>>> &blocks,
                                             vector<int> &line_nos,
                                             vector<string> &tags) {
    while (!inputFile.eof()) {
        if (line.back() == '\r') line.pop_back();

        l++;
        if (line[0] == '[') {
            tags.push_back(line);
            line_nos.push_back(l);
            blocks.push_back(readBlock(inputFile, l, line));
        } else {
            if (!getline(inputFile, line)) break;
        }
    }
    return l;
}

vector<vector<string>>
MasterSystemInputFileParser::readBlock(ifstream &inputFile, int &l,
                                       string &line) {
    vector<vector<string>> block;

    while (getline(inputFile, line) && line[0] != '[') {
        if (line.back() == '\r') line.pop_back();

        if (line[0] != '#' && !line.empty()) {
            vector<string> tokenized_line;
            Utils::tokenizeString(line, tokenized_line);
            block.push_back(tokenized_line);
        }
        l++;
    }

    return block;
}

void MasterSystemInputFileParser::clearParsers() {
    for (auto wsp : water_source_parsers) {
        delete wsp;
    }
    water_source_parsers.clear();
    for (auto ws : water_sources) {
        delete ws;
    }
    water_sources.clear();

    for (auto up : utility_parsers) {
        delete up;
    }
    utility_parsers.clear();
    for (auto u : utilities) {
        delete u;
    }
    utilities.clear();

    for (auto dmpp : drought_mitigation_policy_parsers) {
        delete dmpp;
    }
    drought_mitigation_policy_parsers.clear();
    for (auto dmp : drought_mitigation_policy) {
        delete dmp;
    }
    utilities.clear();
}

string MasterSystemInputFileParser::findName(vector<vector<string>> &block,
                                             const string &tag, int line_no) {
    for (auto &line : block) {
        if (line[0] == "name") {
            return line[1];
        }
    }

    throw MissingParameter("name", tag, line_no);
}

const vector<WaterSource *> &
MasterSystemInputFileParser::getWaterSources() const {
    return water_sources;
}

const vector<Utility *> &MasterSystemInputFileParser::getUtilities() const {
    return utilities;
}

const Graph &MasterSystemInputFileParser::getWaterSourcesGraph() const {
    return water_sources_graph;
}

const vector<DroughtMitigationPolicy *> &
MasterSystemInputFileParser::getDroughtMitigationPolicy() const {
    return drought_mitigation_policy;
}

const vector<MinEnvFlowControl *> &
MasterSystemInputFileParser::getReservoirControlRules() const {
    return reservoir_control_rules;
}

const vector<vector<int>> &
MasterSystemInputFileParser::getReservoirUtilityConnectivityMatrix() const {
    return reservoir_utility_connectivity_matrix;
}

const vector<vector<double>> &
MasterSystemInputFileParser::getTableStorageShift() const {
    return table_storage_shift;
}
