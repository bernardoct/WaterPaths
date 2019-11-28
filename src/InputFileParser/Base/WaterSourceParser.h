//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_WATERSOURCEPARSER_H
#define TRIANGLEMODEL_WATERSOURCEPARSER_H


#include "ElementParser.h"
#include "../Bonds/MasterBondParser.h"

class WaterSourceParser : public ElementParser {
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

    virtual void parseVariables(vector<vector<string>> &block,
                                int n_realizations, int n_weeks);

    virtual WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no,
                   int n_realizations, int n_weeks,
                   const map<string, int> &ws_name_to_id,
                   const map<string, int> &utility_name_to_id) = 0;

    virtual void checkMissingOrExtraParams(int line_no,
                                           vector<vector<string>> &block);

    static void checkForUnreadTags(int line_no,
                                   const vector<vector<string>> &block,
                                   const string &tag_name);

    static void cleanBlock(vector<vector<string>> &block,
                           vector<unsigned long> &rows_read);
};


#endif //TRIANGLEMODEL_WATERSOURCEPARSER_H
