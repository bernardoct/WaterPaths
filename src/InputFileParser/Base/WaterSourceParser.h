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
    bool existing_infrastructure = false;

    void cleanBlock(vector<vector<string>> &block, vector<unsigned long> &rows_read) const;

public:
    const string tag_name;

    explicit WaterSourceParser(string tag_name);

    virtual ~WaterSourceParser();

    virtual void parseVariables(vector<vector<string>> &block, int n_realizations, int n_weeks);

    virtual WaterSource *
    generateSource(int id, vector<vector<string>> &block, int line_no, int n_realizations, int n_weeks) =0;

    virtual void checkMissingOrExtraParams(int line_no, vector<vector<string>> &block);
};


#endif //TRIANGLEMODEL_WATERSOURCEPARSER_H
