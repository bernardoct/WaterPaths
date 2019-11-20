//
// Created by Bernardo on 11/20/2019.
//

#ifndef TRIANGLEMODEL_MASTERBONDPARSER_H
#define TRIANGLEMODEL_MASTERBONDPARSER_H

#include "../../SystemComponents/Bonds/Base/Bond.h"

class MasterBondParser {
private:
    vector<Bond *> parsed_bonds;

public:
    ~MasterBondParser();

    Bond* generate_bond(vector<string> &bond_data);

};


#endif //TRIANGLEMODEL_MASTERBONDPARSER_H
