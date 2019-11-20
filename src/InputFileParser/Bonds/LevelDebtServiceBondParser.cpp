//
// Created by Bernardo on 11/20/2019.
//

#include <algorithm>
#include "LevelDebtServiceBondParser.h"
#include "../MasterSystemInputFileParser.h"


void LevelDebtServiceBondParser::parseVariables(vector<string> &bond_data) {
    BondParser::parseVariables(bond_data);
}

Bond* LevelDebtServiceBondParser::generate_bond(vector<string> &bond_data) {
    BondParser::updateId();
    parseVariables(bond_data);

    Bond* bonds = new LevelDebtServiceBond(getId(), cost_of_capital, n_payments, coupon_rate,
            pay_on_weeks, begin_repayment_at_issuance);

    return bonds;
}
