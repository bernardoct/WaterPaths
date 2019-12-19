//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
#define TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H

#include <string>
#include <vector>
#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "Base/WaterSourceParser.h"
#include "UtilityParser/UtilityParser.h"
#include "../Utils/Graph/Graph.h"
#include "Base/DroughtMitigationPolicyParser.h"
#include "WaterSourceParsers/ReservoirParser.h"
#include "WaterSourceParsers/AllocatedReservoirParser.h"
#include "WaterSourceParsers/ReuseParser.h"
#include "WaterSourceParsers/ReservoirExpansionParser.h"
#include "DroughtMitigationPolicyParsers/RestrictionsParser.h"
#include "DroughtMitigationPolicyParsers/TransfersParser.h"
#include "Base/ReservoirControlRuleParser.h"

using namespace std;

class MasterSystemInputFileParser {

    vector<WaterSourceParser *> water_source_parsers;
    vector<WaterSource *> water_sources;
    vector<UtilityParser *> utility_parsers;
    vector<Utility *> utilities;
    vector<DroughtMitigationPolicyParser *> drought_mitigation_policy_parsers;
    vector<DroughtMitigationPolicy *> drought_mitigation_policy;
    vector<ReservoirControlRuleParser *> reservoir_control_rule_parse;
    vector<MinEnvFlowControl *> reservoir_control_rules;

    vector<vector<double>> rdm_utilities, rdm_water_sources, rdm_dmp;
    vector<vector<double>> dec_vars_bounds;
    vector<double> objs_epsilons;

    vector<vector<int>> reservoir_utility_connectivity_matrix;
    vector<vector<double>> table_storage_shift;

    Graph water_sources_graph;
    Graph utilities_graph;

    int n_realizations = NON_INITIALIZED;
    int n_weeks = NON_INITIALIZED;
    bool generate_rof_tables = false;
    bool optimize = false;
    map<string, vector<vector<double>>> pre_loaded_data;

    int rdm_no = NON_INITIALIZED;
    int n_threads = NON_INITIALIZED;
    int n_bootstrap_samples = NON_INITIALIZED;
    int bootstrap_sample_size = NON_INITIALIZED;
    int n_function_evals = NON_INITIALIZED;
    int runtime_output_interval = NON_INITIALIZED;

    string rof_tables_dir;
    string solutions_file;
    int use_rof_tables = DO_NOT_EXPORT_OR_IMPORT_ROF_TABLES; /// can be "no," "export," and "import."
    bool print_time_series = false;
    vector<unsigned long> realizations_to_run;
    vector<unsigned long> solutions_to_run;
    vector<vector<double>> solutions_decvars;

    vector<vector<vector<string>>> blocks;
    vector<int> line_nos;
    vector<string> tags;

    map<string, int> ws_name_to_id;
    map<string, int> utility_name_to_id;

    bool parseUtility(int line_no,
                      vector<vector<string>> &block,
                      const string &tag,
                      bool create_objects);

    bool
    parseGraphsAndMatrices(int line_no,
                           vector<vector<string>> &block,
                           const string &tag,
                           bool create_objects);

    bool parseDroughtMitigationPolicies(int line_no,
                                        vector<vector<string>> &block,
                                        const string &tag,
                                        bool create_objects);

    bool parseWaterSource(int line_no,
                          vector<vector<string>> &block,
                          const string &tag,
                          bool create_objects);

    bool
    parseReservoirControlRules(int line_no,
                               vector<vector<string>> &block,
                               const string &tag,
                               bool create_objects);

    bool parsePreloadedData(int line_no, vector<vector<string>> &block,
                            const string &tag, bool read_data,
                            int n_realizations);

    void loopThroughTags(vector<vector<vector<string>>> blocks,
                         bool create_objects = true);

    static string findName(vector<vector<string>> &block,
                           const string &tag, int line_no);

    void parseFile(string file_path);

public:

    MasterSystemInputFileParser();

    ~MasterSystemInputFileParser();

    void clearParsers();

    static vector<vector<string>>
    readBlock(ifstream &inputFile, int &l, string &line);

    int
    parseBlocks(ifstream &inputFile, int l, string &line,
                vector<vector<vector<string>>> &blocks, vector<int> &line_nos,
                vector<string> &tags);

    void createSystemObjects(double *vars);

    void initializeStandardRDMFactors();

/**
 * Checks with of the elements separated by commas in string data are preceded
 * by a @ and sorts them according to the values in vars. Any element in data
 * that is not preceded by @ will be left in the same position.
 * @param vars array with numeric values to be translated into positions
 * @param count_var index of vars to be used to sort data.
 * @param data piece of line
 */
    static void
    reorderCSVDataInBlockLine(const double *vars, int &count_var,
                              string &data);

/**
 * look for %%% and replaces them by next values in vars.
 * @param vars array with numeric values
 * @param count_var index of vars to be replace into line.
 * @param data piece of line
 * @return updated index of vars to be replace into line
 */
    static void replaceNumericVarsIntoPlaceHolders(const double *vars,
            int &count_var, string &data);

/**
 * Fill in the gaps in the blocks left by the %%% and @ in the input file.
 * @param vars decision variables.
 * @param blocks data parsed from input file.
 * @return
 */
    static void replacePlaceHoldersByDVs(double *vars,
                                         vector<vector<vector<string>>> &blocks);

    void preloadAndCheckInputFile(string &input_file);

    bool
    parseRunParams(int line_no, vector<vector<string>> &block,
                   const string &tag, bool read_data);

    bool
    parseDecVarsBoundsAndObjEpsilons(int line_no, vector<vector<string>> &block,
                                     const string &tag, bool create_objects);

    const vector<WaterSource *> &getWaterSources() const;

    const vector<Utility *> &getUtilities() const;

    const Graph &getWaterSourcesGraph() const;

    const vector<DroughtMitigationPolicy *> &getDroughtMitigationPolicy() const;

    const vector<MinEnvFlowControl *> &getReservoirControlRules() const;

    const vector<vector<int>> &getReservoirUtilityConnectivityMatrix() const;

    const vector<vector<double>> &getTableStorageShift() const;

    const string &getRofTablesDir() const;

    int getUseRofTables() const;

    int getNThreads() const;

    int getRdmNo() const;

    bool isPrintTimeSeries() const;

    const vector<unsigned long> &getRealizationsToRun() const;

    int getNWeeks() const;

    const vector<vector<double>> &getRdmUtilities() const;

    const vector<vector<double>> &getRdmWaterSources() const;

    const vector<vector<double>> &getRdmDmp() const;

    int getNRealizations() const;

    int getNBootstrapSamples() const;

    int getBootstrapSampleSize() const;

    const string &getSolutionsFile() const;

    const vector<unsigned long> &getSolutionsToRun() const;

    const vector<vector<double>> &getSolutionsDecvars() const;

    bool isOptimize() const;

    int getNFunctionEvals() const;

    int getRuntimeOutputInterval() const;

    const vector<vector<double>> &getDecVarsBounds() const;

    const vector<double> &getObjsEpsilons() const;
};


#endif //TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
