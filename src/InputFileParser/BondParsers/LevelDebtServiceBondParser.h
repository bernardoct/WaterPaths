//
// Created by Bernardo on 11/20/2019.
//

#ifndef TRIANGLEMODEL_LEVELDEBTSERVICEBONDPARSER_H
#define TRIANGLEMODEL_LEVELDEBTSERVICEBONDPARSER_H

#include <vector>
#include <string>
#include "../../SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../Base/BondParser.h"

using namespace std;

class LevelDebtServiceBondParser : public BondParser {
private:

public:

    Bond* generateBond(const vector<string> &bond_data) override;
};


#endif //TRIANGLEMODEL_LEVELDEBTSERVICEBONDPARSER_H
