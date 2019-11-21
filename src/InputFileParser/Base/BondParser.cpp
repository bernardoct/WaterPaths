//
// Created by Bernardo on 11/20/2019.
//

#include <algorithm>
#include "BondParser.h"
#include "../MasterSystemInputFileParser.h"
#include "../../Utils/Utils.h"


int BondParser::id = -1;

void BondParser::updateId() {
    ++id;
}

int BondParser::getId() {
    return id;
}

void BondParser::parseVariables(vector<string> &bond_data) {
    cost_of_capital = stod(bond_data[2]);
    n_payments = stoi(bond_data[3]);
    coupon_rate = stod(bond_data[4]);
    pay_on_weeks = Utils::tokenizeStringInt(bond_data[5], ',');
    if (bond_data.size() < 7) {
        throw invalid_argument("Bond must either have the first payment deferred or immediate.");
    } else if (bond_data[6] == "deferred") {
        begin_repayment_at_issuance = false;
    } else if (bond_data[6] == "immediate") {
        begin_repayment_at_issuance = true;
    } else {
        throw invalid_argument("Bond must either have the first payment deferred or immediate.");
    }
}

Bond *BondParser::generate_bond(vector<string> &bond_data) {
    updateId();
    return nullptr;
}
