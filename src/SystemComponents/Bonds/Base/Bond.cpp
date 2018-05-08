//
// Created by bernardo on 4/12/18.
//

#include <iostream>
#include "Bond.h"

Bond::Bond(const int id, const double cost_of_capital, const int n_payments,
           vector<int> pay_on_weeks, const int type) :
        id(id), cost_of_capital(cost_of_capital), n_payments(n_payments),
        pay_on_weeks(pay_on_weeks), type(type) {

    if (std::isnan(cost_of_capital) || cost_of_capital < 0) {
        string error = "Invalid construction cost of capital for bond " + to_string(id);
        __throw_invalid_argument(error.c_str());
    }
}

Bond::Bond(const int id, const double cost_of_capital, const int n_payments,
           vector<int> pay_on_weeks, const double coupon_rate, const int type) :
        id(id), cost_of_capital(cost_of_capital), n_payments(n_payments), pay_on_weeks(pay_on_weeks),
        coupon_rate(coupon_rate), type(type) {

    if (std::isnan(cost_of_capital) || cost_of_capital < 0) {
        string error = "Invalid construction cost of capital for bond " + to_string(id);
        __throw_invalid_argument(error.c_str());
    }
}

Bond::Bond() : id(NON_INITIALIZED), n_payments(NON_INITIALIZED), cost_of_capital(NON_INITIALIZED),
                                 type(NON_INITIALIZED) {}

/**
 * Only cost of capital RDM multiplier is applied here because in a joint project, it is assumed any cost overruns
 * would be split among the utilities. Interest and bond term, on the other hand, are utility dependent are set at
 * issuance, in function issueBond.
 * @param r
 * @param rdm_factors
 */
void Bond::setRealizationWaterSource(unsigned long r, vector<double> &rdm_factors) {
    cost_of_capital *= rdm_factors[0];
}

void Bond::issueBond(int week, double bond_term_multiplier, double bond_interest_rate_multiplier) {
    week_issued = week;
    n_payments *= bond_term_multiplier;
    coupon_rate *= bond_interest_rate_multiplier;
}

/**
 * Allocate cost responsibility to bond based on utility allocation in project
 * ASSUMES THAT THE PROJECT COST PASSED IN BOND CONSTRUCTOR IS COST OF ENTIRE PROJECT
 */
void Bond::setAllocatedDebtService(int week, double debt_obligation_fraction) {
    cout << "Need to direct to a virtual function in sub-class of Bond" << endl;
}

double Bond::setRetroactiveDebtServicePayment(int week, double allocation_adjustment_capacity_fraction) {
    return 0;
}
