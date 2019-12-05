//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_WATERSOURCEPARSER_H
#define TRIANGLEMODEL_WATERSOURCEPARSER_H


#include "../BondParsers/MasterBondParser.h"
#include "../../SystemComponents/WaterSources/Base/WaterSource.h"

class WaterSourceParser {
private:
protected:
    string name;
    vector<string> catchments;
    double capacity = NON_INITIALIZED;
    double treatment_capacity = ILLIMITED_TREATMENT_CAPACITY;
    int permitting_time = NON_INITIALIZED;
    vector<Bond *> bonds;
    MasterBondParser master_bond_parser;
    vector<double> construction_time;
    vector<double> allocated_treatment_fractions;
    vector<double> allocated_fractions;
    vector<int> utilities_with_allocations;
    bool existing_infrastructure = true;

public:
    const string tag_name;

    explicit WaterSourceParser(string tag_name);

    virtual ~WaterSourceParser();

    /**
     *
     * @param block list of parameter and their values for that system component.
     * @param n_realizations
     * @param n_weeks
     * @param line_no
     * @param ws_name_to_id map mapping names of water sources to corresponding IDs.
     * @param utility_name_to_id map mapping names of utilities to corresponding IDs.
     */
    virtual void parseVariables(vector<vector<string>> &block,
                                int n_realizations, int n_weeks, int line_no,
                                const map<string, int> &ws_name_to_id,
                                const map<string, int> &utility_name_to_id);

    /**
     *
     * @param id
     * @param block list of parameter and their values for that system component.
     * @param line_no
     * @param n_realizations
     * @param n_weeks
     * @param ws_name_to_id map mapping names of water sources to corresponding IDs.
     * @param utility_name_to_id map mapping names of utilities to corresponding IDs.
     * @return
     */
    virtual WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no,
                   int n_realizations, int n_weeks,
                   const map<string, int> &ws_name_to_id,
                   const map<string, int> &utility_name_to_id) = 0;

    /**
     *
     * @param line_no
     * @param block list of parameter and their values for that system component.
     */
    virtual void checkMissingOrExtraParams(int line_no,
                                           vector<vector<string>> &block);
};


#endif //TRIANGLEMODEL_WATERSOURCEPARSER_H
