//
// Created by Bernardo on 11/19/2019.
//

#include <cstring>
#include <algorithm>
#include <numeric>
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
#include "Exceptions/InconsistentMutuallyImplicativeParameters.h"
#include "../DataCollector/MasterDataCollector.h"

MasterSystemInputFileParser::MasterSystemInputFileParser() = default;

MasterSystemInputFileParser::~MasterSystemInputFileParser() {
    clearParsers();
}

void MasterSystemInputFileParser::preloadAndCheckInputFile(string &input_file) {

    // read first line, blocks, tags, and tags' line numbers.
    parseFile(input_file);

    // Preload heady data so that it's not reloaded for every function
    // evaluation during and optimization exercise.
    auto it = find(tags.begin(), tags.end(), "[DATA TO LOAD]");
    if (it == tags.end()) {
        char error[500];
        sprintf(error, "Input file %s is missing tag [DATA TO LOAD]",
                input_file.c_str());
        throw invalid_argument(error);
    }

    // loop through blocks to map of names utilities and water sources to their
    // to ids. Also checks for undefined tags and other errors.
    loopThroughTags(blocks, false);
    clearParsers();
}

void MasterSystemInputFileParser::createSystemObjects(double *vars) {
    if (!blocks.empty()) {
	auto blocks_sol = blocks;
        if (vars != nullptr) {
            replacePlaceHoldersByDVs(vars, blocks_sol);
        }

//	for (auto &bb : blocks_sol) {
//	    for (auto &b : bb) {
//	        for (string s : b) {
//		    printf("%s ", s.c_str());
//		}
//		printf("\n");
//	    }
//	    printf("\n");
//	}

        // loop through blocks again but now to create all objects.
        loopThroughTags(blocks_sol, true);

        // initialize standard RDM factors if they have not been loaded yet.
        if (rdm_dmp.empty()) {
            initializeStandardRDMFactors();
        }
    } else {
        throw invalid_argument("Data blocks must be populated before "
                               "createSystemObjects can be called.");
    }
}

void
MasterSystemInputFileParser::replacePlaceHoldersByDVs(
        double *vars, vector<vector<vector<string>>> &blocks) {
    int count_var = 0;
    for (auto &block : blocks) {
        for (auto &line : block) {
            for (string &data : line) {
                // look for %%% and replaces them by next values in vars.
                replaceNumericVarsIntoPlaceHolders(vars, count_var, data);

                // look for names beginning with @, reorder the names according
                // to values in vars, and removes all @s.
                unsigned long it = data.find('@');
                if (it != string::npos) {
                    reorderCSVDataInBlockLine(vars, count_var, data);
                }
//		printf("%s ", data.c_str());
            }
//	    printf("\n");
        }
//	printf("\n");
    }
}

void MasterSystemInputFileParser::replaceNumericVarsIntoPlaceHolders(
        const double *vars, int &count_var, string &data) {
    auto it = data.find("%%%");
    while (it != string::npos) {
        data.replace(it, 3, to_string(vars[count_var]));
        it = data.find("%%%");
        count_var++;
    }
}

void MasterSystemInputFileParser::reorderCSVDataInBlockLine(const double *vars,
                                                            int &count_var,
                                                            string &data) {
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
    for (unsigned long k = 0; k < has_at.size(); k++) {
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
                                   !create_objects, n_realizations
                ) ? true : tag_read
        );
        tag_read = (
                // preloaded data parser included here just to check the tag so
                // that exception below is not triggered.
                parseRunParams(line_nos[t], blocks[t], tags[t],
                               !create_objects
                ) ? true : tag_read
        );
        tag_read = (
                parseDecVarsBoundsAndObjEpsilons(line_nos[t], blocks[t],
                                                 tags[t], !create_objects
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

bool MasterSystemInputFileParser::parseRunParams(int line_no,
                                                 vector<vector<string>> &block,
                                                 const string &tag,
                                                 bool read_data) {
    if (tag == "[RUN PARAMETERS]") {
        if (read_data) {
            vector<unsigned long> rows_read(0);
            for (unsigned long i = 0; i < block.size(); ++i) {
                vector<string> line = block[i];
                if (line[0] == "n_realizations") {
                    n_realizations = stoi(line[1]);
                    realizations_to_run = vector<unsigned long>(n_realizations);
                    iota(realizations_to_run.begin(), realizations_to_run.end(),
                         0);
                    rows_read.push_back(i);
                } else if (line[0] == "n_weeks") {
                    n_weeks = stoi(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "rdm_utilities") {
                    rdm_utilities = Utils::parse2DCsvFile(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "rdm_water_sources") {
                    rdm_water_sources = Utils::parse2DCsvFile(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "rdm_dmps") {
                    rdm_dmp = Utils::parse2DCsvFile(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "rdm_no") {
                    rdm_no = stoi(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "n_threads") {
                    n_threads = stoi(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "rof_tables_dir") {
                    rof_tables_dir = line[1];
                    rows_read.push_back(i);
                } else if (line[0] == "use_rof_tables") {
                    if (line[1] == "generate")
                        use_rof_tables = EXPORT_ROF_TABLES;
                    else if (line[1] == "import")
                        use_rof_tables = IMPORT_ROF_TABLES;
                    else if (line[1] == "no")
                        use_rof_tables = DO_NOT_EXPORT_OR_IMPORT_ROF_TABLES;
                    else {
                        char error[500];
                        sprintf(error,
                                "Parameter \"use_rof_tables\" in tag %s, "
                                "line %d can only take the values of "
                                "\"generate\", \"import\", or \"no\".",
                                tag.c_str(), line_no);
                        throw invalid_argument(error);
                    }
                    rows_read.push_back(i);
                } else if (line[0] == "print_time_series") {
                    print_time_series = true;
                    rows_read.push_back(i);
                } else if (line[0] == "realizations_to_run") {
                    rof_tables_dir = line[1];
                    rows_read.push_back(i);
                } else if (line[0] == "realizations_to_run") {
                    Utils::tokenizeString(line[1], realizations_to_run, ',');
                    n_realizations = *max_element(realizations_to_run.begin(),
                                                  realizations_to_run.end());
                    rows_read.push_back(i);
                } else if (line[0] == "solutions_file") {
                    solutions_file = line[1];
                    rows_read.push_back(i);
                } else if (line[0] == "n_bootstrap_samples") {
                    n_bootstrap_samples = stoi(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "bootstrap_sample_size") {
                    bootstrap_sample_size = stoi(line[1]);
                    rows_read.push_back(i);
                } else if (line[0] == "solutions_to_run") {
                    if (!solutions_to_run.empty()) {
                        throw invalid_argument(
                                "Parameters solutions_to_run and "
                                "solutions_to_run_range cannot be passed "
                                "together.");
                    }
                    Utils::tokenizeString(line[1], solutions_to_run, ',');
                    rows_read.push_back(i);
                } else if (line[0] == "solutions_to_run_range") {
                    if (!solutions_to_run.empty()) {
                        throw invalid_argument(
                                "Parameters solutions_to_run and "
                                "solutions_to_run_range cannot be passed "
                                "together.");
                    }
                    vector<unsigned long> range;
                    Utils::tokenizeString(line[1], range, ',');
                    if (range[0] > range[1]) {
                        throw invalid_argument(
                                "First number in solutions_to_run_range needs "
                                "to be smaller than the second.");
                    }
                    solutions_to_run = vector<unsigned long>(range[1]);
                    iota(solutions_to_run.begin(), solutions_to_run.end(),
                         0);
                    rows_read.push_back(i);
                } else if (line[0] == "seed") {
                    int seed = stoi(line[1]);
                    WaterSource::setSeed(seed);
                    MasterDataCollector::setSeed(seed);
		    this->seed = seed;
                    rows_read.push_back(i);
                } else if (line[0] == "optimize") {
                    optimize = true;
                    n_function_evals = stoi(line[1]);
                    runtime_output_interval = stoi(line[2]);
		    rows_read.push_back(i);
                }
            }

            if (!solutions_file.empty()) {
                if (solutions_to_run.empty()) {
                    throw invalid_argument("If solutions file was passed, "
                                           "either solutions_to_run_range or "
                                           "solutions_to_run need to be passed "
                                           "too.");
                } else {
                    solutions_decvars = Utils::parse2DCsvFile(solutions_file,
                                                              NON_INITIALIZED,
                                                              solutions_to_run);
                }
            }

            bool empty_rdm = (rdm_dmp.empty() && rdm_water_sources.empty() &&
                              rdm_utilities.empty());
            bool full_rdm = (!rdm_dmp.empty() && !rdm_water_sources.empty() &&
                             !rdm_utilities.empty());
            if (!(empty_rdm || full_rdm)) {
                throw InconsistentMutuallyImplicativeParameters(
                        "rdm_dmp, rdm_water_sources, and rdm_utilities", tag,
                        line_no);
            }
            if ((n_bootstrap_samples == NON_INITIALIZED &&
                 bootstrap_sample_size != NON_INITIALIZED) ||
                (n_bootstrap_samples != NON_INITIALIZED &&
                 bootstrap_sample_size == NON_INITIALIZED)) {
                throw InconsistentMutuallyImplicativeParameters(
                        "n_bootstrap_samples and bootstrap_sample_size", tag,
                        line_no);
            }
            AuxParserFunctions::cleanBlock(block, rows_read);
            AuxParserFunctions::checkForUnreadTags(line_no, block, tag);
        }
        return true;
    }
    return false;
}

const vector<vector<double>> &
MasterSystemInputFileParser::getSolutionsDecvars() const {
    return solutions_decvars;
}

//TODO WRITE TEST FOR FUNCTION BELOW.
bool MasterSystemInputFileParser::parsePreloadedData(int line_no,
                                                     vector<vector<string>> &block,
                                                     const string &tag,
                                                     bool read_data,
                                                     int n_realizations) {
    if (tag == "[DATA TO LOAD]") {
        if (read_data) {
            for (auto line : block) {
                try {
                    string alias = line.at(0);
                    if (alias[0] == '*') {
                        alias.erase(0, 1);
                        pre_loaded_data.insert(
                                {alias,
                                 Utils::parse2DCsvFile(line.at(1))});
                    } else {
                        pre_loaded_data.insert(
                                {alias,
                                 Utils::parse2DCsvFile(line.at(1),
                                                       n_realizations)});
                    }
                } catch (out_of_range &e) {
                    e.what();
                    throw invalid_argument("Data to be loaded must be specified"
                                           " as a alias followed by the file "
                                           "path. Add a * before the alias of "
                                           "files that are not ensembles of "
                                           "time series of demand, inflows or "
                                           "evaporation rates.");
                }
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

        // Looks for first tag
        if (line[0] != '[') {
            while (!getline(inputFile, line)) {
                line_no++;
                if (line[0] == '[') break;
            }
        }

        parseBlocks(inputFile, line_no, line, blocks, line_nos, tags);

        // Move "[RUN PARAMETERS]" tag to the top.
        for (int i = 0; i < blocks.size(); ++i) {
            if (tags[i] == "[RUN PARAMETERS]" && i > 0) {
                iter_swap(blocks.begin() + i, blocks.begin());
                iter_swap(tags.begin() + i, tags.begin());
                iter_swap(line_nos.begin() + i, line_nos.begin());
                break;
            }
        }
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
                    ws_name_to_id, pre_loaded_data));
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
                new ReservoirParser(use_rof_tables == EXPORT_ROF_TABLES));
    } else if (tag == "[ALLOCATED RESERVOIR]") {
        water_source_parsers.push_back(
                new AllocatedReservoirParser(
                        use_rof_tables == EXPORT_ROF_TABLES));
    } else if (tag == "[RESERVOIR EXPANSION]") {
        water_source_parsers.push_back(new ReservoirExpansionParser());
    } else if (tag == "[INTAKE]") {
        throw invalid_argument("Intake parser, tag [INTAKE], not yet "
                               "implemented.");
    } else if (tag == "[QUARRY]") {
        throw invalid_argument("Quarry parser, tag [QUARRY], not yet "
                               "implemented.");
    } else if (tag == "[REALLOCATION]") {
        throw invalid_argument("Reallocation parser, tag [REALLOCATION], not "
                               "yet implemented.");
    }

    // Only create water source if a parser has been created, indicating the
    // block is a water source.
    if (water_source_id < water_source_parsers.size()) {
        if (create_objects) {
            water_sources.push_back(
                    water_source_parsers.back()->generateSource(
                            water_source_id, block, line_no, n_realizations,
                            n_weeks, ws_name_to_id, utility_name_to_id,
                            pre_loaded_data
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

bool MasterSystemInputFileParser::parseDecVarsBoundsAndObjEpsilons(
        int line_no, vector<vector<string>> &block, const string &tag,
        bool create_objects) {
#ifdef PARALLEL
    if (create_objects) {
        if (tag == "[DECISION VARIABLE BOUNDS]") {
	    n_dec_vars = block.size();
            dec_vars_bounds.resize(n_dec_vars);
            for (auto &line : block) {
                vector<double> bounds;
                try {
                    Utils::tokenizeString(line[1], bounds, ',');
                    dec_vars_bounds[stod(line[0])] = bounds;
                } catch (out_of_range &e) {
                    char error[500];
                    sprintf(error, "It looks like you skipped a decision "
                                   "variable in tag %s, line %d. Did you skip "
                                   "decision variable number 0?", tag.c_str(),
                            line_no);
                    throw invalid_argument(error);
                } 

                if (bounds.size() != 2 || bounds[0] > bounds[1]) {
                    char error[300];
                    sprintf(error, "Decision variable bounds in line %d, tag %s"
                                   " is not comprised of variable id, lower, and"
                                   " upper bound.", line_no, tag.c_str());
                    throw invalid_argument(error);
                }
            }
            return true;
        } else if (tag == "[OBJECTIVES EPSILONS]") {
            Utils::tokenizeString(block[0][0], objs_epsilons, ',');
            return true;
        }

        return false;
    } else {
        return true;
    }
#else
    if (tag == "[DECISION VARIABLE BOUNDS]" && tag == "[OBJECTIVES EPSILONS]") {
        string error = "Tags \"[DECISION VARIABLE BOUNDS]\" and \"[OBJECTIVES "
                       "EPSILONS]\" can only be passed to a version of "
                       "WaterPaths compiled with a multiobjective algorithm.";
        throw invalid_argument(error.c_str());
    }
#endif

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
                            utility_name_to_id, pre_loaded_data));
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
                            rdm_water_sources, rdm_dmp, pre_loaded_data)
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

int MasterSystemInputFileParser::getRdmNo() const {
    return rdm_no;
}

int MasterSystemInputFileParser::getNThreads() const {
    return n_threads;
}

const string &MasterSystemInputFileParser::getRofTablesDir() const {
    return rof_tables_dir;
}

int MasterSystemInputFileParser::getUseRofTables() const {
    return use_rof_tables;
}

bool MasterSystemInputFileParser::isPrintTimeSeries() const {
    return print_time_series;
}

const vector<unsigned long> &
MasterSystemInputFileParser::getRealizationsToRun() const {
    return realizations_to_run;
}

int MasterSystemInputFileParser::getNWeeks() const {
    return n_weeks;
}

const vector<vector<double>> &
MasterSystemInputFileParser::getRdmUtilities() const {
    return rdm_utilities;
}

const vector<vector<double>> &
MasterSystemInputFileParser::getRdmWaterSources() const {
    return rdm_water_sources;
}

const vector<vector<double>> &MasterSystemInputFileParser::getRdmDmp() const {
    return rdm_dmp;
}

int MasterSystemInputFileParser::getNRealizations() const {
    return n_realizations;
}

int MasterSystemInputFileParser::getNBootstrapSamples() const {
    return n_bootstrap_samples;
}

int MasterSystemInputFileParser::getBootstrapSampleSize() const {
    return bootstrap_sample_size;
}

const string &MasterSystemInputFileParser::getSolutionsFile() const {
    return solutions_file;
}

const vector<unsigned long> &
MasterSystemInputFileParser::getSolutionsToRun() const {
    return solutions_to_run;
}

bool MasterSystemInputFileParser::isOptimize() const {
    return optimize;
}

int MasterSystemInputFileParser::getNFunctionEvals() const {
    return n_function_evals;
}

int MasterSystemInputFileParser::getRuntimeOutputInterval() const {
    return runtime_output_interval;
}

const vector<vector<double>> &
MasterSystemInputFileParser::getDecVarsBounds() const {
    return dec_vars_bounds;
}

const vector<double> &MasterSystemInputFileParser::getObjsEpsilons() const {
    return objs_epsilons;
}

unsigned long MasterSystemInputFileParser::getNDecVars() const {
    return n_dec_vars;
}

unsigned long MasterSystemInputFileParser::getNObjectives() const {
    return N_OBJECTIVES;
}

int MasterSystemInputFileParser::getSeed() const {
    return seed;
}
