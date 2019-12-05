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

struct infraRank {
    string name;
    double xreal;

    infraRank(string name, double xreal) {
        this->name = name;
        this->xreal = xreal;
    }
};

struct by_xreal {
    bool operator()(const infraRank &ir1, const infraRank &ir2) {
        return ir1.xreal < ir2.xreal;
    }
};

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

    vector<vector<int>> reservoir_utility_connectivity_matrix;
    vector<vector<double>> table_storage_shift;

    vector<vector<vector<string>>> blocks;
    vector<int> line_nos;
    vector<string> tags;

    Graph water_sources_graph;
    Graph utilities_graph;

    int n_realizations = NON_INITIALIZED;
    int n_weeks = NON_INITIALIZED;
    bool generate_rof_tables;

    map<string, int> ws_name_to_id;
    map<string, int> utility_name_to_id;
    map<string, vector<vector<double>>> pre_loaded_data;

    void parseFirstLine(const string &line);

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

    void loopThroughTags(vector<vector<vector<string>>> blocks, bool create_objects = true);

    void clearParsers();

    static string findName(vector<vector<string>> &block,
                    const string &tag, int line_no);

    vector<vector<vector<string>>> replacePlaceHoldersByDVs(double *vars, vector<vector<vector<string>>> &blocks);

public:

    MasterSystemInputFileParser();

    ~MasterSystemInputFileParser();

    void parseFile(string file_path);

    static vector<vector<string>> readBlock(ifstream &inputFile, int &l, string &line);

    int
    parseBlocks(ifstream &inputFile, int l, string &line,
                vector<vector<vector<string>>> &blocks, vector<int> &line_nos,
                vector<string> &tags);

    void createSystemObjects(double *vars);

    const vector<WaterSource *> &getWaterSources() const;

    const vector<Utility *> &getUtilities() const;

    const Graph &getWaterSourcesGraph() const;

    const vector<DroughtMitigationPolicy *> &getDroughtMitigationPolicy() const;

    const vector<MinEnvFlowControl *> &getReservoirControlRules() const;

    const vector<vector<int>> &getReservoirUtilityConnectivityMatrix() const;

    const vector<vector<double>> &getTableStorageShift() const;

    void initializeStandardRDMFactors();

    void
    reorderCSVDataInBlockLine(const double *vars, int &count_var,
                              string &data) const;

    void replaceNumericVarsIntoPlaceHolders(const double *vars, int &count_var,
                                            string &data) const;

    void preloadAndCheckInputFile(string &input_file);
};


#endif //TRIANGLEMODEL_MASTERSYSTEMINPUTFILEPARSER_H
