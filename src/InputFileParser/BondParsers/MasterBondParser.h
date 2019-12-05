//
// Created by Bernardo on 11/20/2019.
//

#ifndef TRIANGLEMODEL_MASTERBONDPARSER_H
#define TRIANGLEMODEL_MASTERBONDPARSER_H

#include "../../SystemComponents/Bonds/Base/Bond.h"
#include "LevelDebtServiceBondParser.h"

class MasterBondParser {
private:
    LevelDebtServiceBondParser level_debt_service_bond_parser;
    vector<Bond *> parsed_bonds;

public:
    ~MasterBondParser();

    Bond* generate_bond(vector<string> &bond_data);

};


#endif //TRIANGLEMODEL_MASTERBONDPARSER_H
