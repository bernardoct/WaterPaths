//
// Created by Bernardo on 11/19/2019.
//

#ifndef TRIANGLEMODEL_WATERSOURCEPARSER_H
#define TRIANGLEMODEL_WATERSOURCEPARSER_H


#include "ElementParser.h"
#include "../Bonds/MasterBondParser.h"

class WaterSourceParser : public ElementParser {
private:
    static int id;
protected:
    string name;
    vector<string> catchments;
    double capacity = NON_INITIALIZED;
    double treatment_capacity = NON_INITIALIZED;
    int permitting_time;
    vector<string> bond_info;
    vector<Bond *> bonds;
    MasterBondParser master_bond_parser;
    vector<double> construction_time;

    static void updateId();

public:
    static int getId();

    virtual void parseVariables(vector<vector<string>> &block);

    virtual WaterSource* generateSource(vector<vector<string>> &block)=0;
};


#endif //TRIANGLEMODEL_WATERSOURCEPARSER_H
