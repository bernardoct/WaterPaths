//
// Created by Bernardo on 11/20/2019.
//

#include "MasterBondParser.h"
#include "LevelDebtServiceBondParser.h"



MasterBondParser::~MasterBondParser() {
    for (auto bond : parsed_bonds) {
        delete bond;
    }
    parsed_bonds.clear();
}

Bond *MasterBondParser::generate_bond(vector<string> &bond_data) {
    if (bond_data[1] == "level") {
        parsed_bonds.push_back(level_debt_service_bond_parser.generate_bond(bond_data));
    } else {
        throw invalid_argument("Bond type not recognized.");
    }

    return parsed_bonds.back();
}
