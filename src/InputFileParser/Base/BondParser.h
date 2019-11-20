//
// Created by Bernardo on 11/20/2019.
//

#ifndef TRIANGLEMODEL_BONDPARSER_H
#define TRIANGLEMODEL_BONDPARSER_H

#include <vector>
#include "../../Utils/Constants.h"
#include "../../SystemComponents/Bonds/Base/Bond.h"

using namespace std;
using namespace Constants;

class BondParser {
private:
    static int id;
protected:
    double cost_of_capital;
    double coupon_rate;
    int n_payments;
    int begin_repayment_after_n_years = NON_INITIALIZED;
    vector<int> pay_on_weeks;
    bool begin_repayment_at_issuance;

    static void updateId();

public:

    static int getId();

    virtual void parseVariables(vector<string> &bond_data);

    /**
     * Gets tokenized contents of bond line and converts into a bond. All bonds must be fully specified in one line.
     * MUST ADD POINTER TO CREATED BOND TO VECTOR parsed_bonds SO THAT THE FORMER IS PROPERLY DELETED AT EXIT.
     *
     * @param bond_data The first element must be the word "bond", the second the bond type, followed by the cost of
     * capital, number of payments, coupon rate, on what weeks to make payments (integers between 0 and 52 separated by
     * commas without spaces), and optionally by whether or not the payments are deferred to after the project begins
     * operations.
     * @return A bond object.
     */
    virtual Bond* generate_bond(vector<string> &bond_data);
};


#endif //TRIANGLEMODEL_BONDPARSER_H
