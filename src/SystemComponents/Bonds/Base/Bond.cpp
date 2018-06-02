//
// Created by bernardo on 4/12/18.
//

#include <iostream>
#include "Bond.h"

Bond::Bond(const int id, const double cost_of_capital, const int n_payments,
           vector<int> pay_on_weeks, const int type, bool begin_repayment_at_issuance) :
        id(id), cost_of_capital(cost_of_capital), n_payments(n_payments), pay_on_weeks(pay_on_weeks), type(type) {

    /// If bond is to start being paid for at issuance, set repayment delay to 0. Otherwise it will be set at issuance.
    if (begin_repayment_at_issuance) {
        begin_repayment_after_n_years = 0;
    }

    if (std::isnan(cost_of_capital) || cost_of_capital < 0) {
        string error = "Invalid construction cost of capital for bond " + to_string(id);
        __throw_invalid_argument(error.c_str());
    }

}

Bond::Bond(const int id, const double cost_of_capital, const int n_payments, vector<int> pay_on_weeks,
           const double coupon_rate, const int type, bool begin_repayment_at_issuance) :
        id(id), cost_of_capital(cost_of_capital), n_payments(n_payments), pay_on_weeks(pay_on_weeks),
        coupon_rate(coupon_rate), type(type) {

    /// If bond is to start being paid for at issuance, set repayment delay to 0. Otherwise it will be set at issuance.
    if (begin_repayment_at_issuance) {
        begin_repayment_after_n_years = 0;
    }

    if (std::isnan(cost_of_capital) || cost_of_capital < 0) {
        string error = "Invalid construction cost of capital for bond " + to_string(id);
        __throw_invalid_argument(error.c_str());
    }

}

Bond::Bond() : id(NON_INITIALIZED), n_payments(NON_INITIALIZED), cost_of_capital(NON_INITIALIZED),
                                 type(NON_INITIALIZED) {}

Bond::~Bond() = default;

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

void
Bond::issueBond(int week, int construction_time, double bond_term_multiplier, double bond_interest_rate_multiplier) {

    /// Set date to begin repayment
    if (begin_repayment_after_n_years == NON_INITIALIZED) {
//        begin_repayment_after_n_years = (int) ((week + construction_time) / WEEKS_IN_YEAR) + 1;
        begin_repayment_after_n_years = (int) ((construction_time) / WEEKS_IN_YEAR) + 1;
    }

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
