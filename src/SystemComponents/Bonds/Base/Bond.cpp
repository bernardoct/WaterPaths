//
// Created by bernardo on 4/12/18.
//

#include "Bond.h"

Bond::Bond(const int id, const double cost_of_capital, const int n_payments,
           vector<int> pay_on_weeks, const int type,
           bool begin_repayment_at_issuance) :
        id(id), cost_of_capital(cost_of_capital), n_payments(n_payments),
        pay_on_weeks(pay_on_weeks), type(type) {

    /// If bond is to start being paid for at issuance, set repayment delay
    /// to 0. Otherwise it will be set at issuance.
    if (begin_repayment_at_issuance) {
        begin_repayment_after_n_years = 0;
    }

    if (std::isnan(cost_of_capital) || cost_of_capital < 0) {
        string error = "Invalid construction cost of capital for bond "
                       + to_string(id);
        throw invalid_argument(error.c_str());
    }
}

Bond::Bond(const int id, const double cost_of_capital, const int n_payments,
           vector<int> pay_on_weeks, const double coupon_rate, const int type,
           bool begin_repayment_at_issuance) :
        id(id), cost_of_capital(cost_of_capital), n_payments(n_payments),
        pay_on_weeks(pay_on_weeks),
        coupon_rate(coupon_rate), type(type) {

    /// If bond is to start being paid for at issuance, set repayment delay
    /// to 0. Otherwise it will be set at issuance.
    if (begin_repayment_at_issuance) {
        begin_repayment_after_n_years = 0;
    }

    if (std::isnan(cost_of_capital) || cost_of_capital < 0) {
        string error = "Invalid construction cost of capital for bond "
                       + to_string(id);
        throw invalid_argument(error.c_str());
    }
}

Bond::Bond() : id(NON_INITIALIZED), n_payments(NON_INITIALIZED),
               cost_of_capital(NON_INITIALIZED), type(NON_INITIALIZED) {}

Bond::~Bond() = default;

/**
 * Only cost of capital RDM multiplier is applied here because in a joint
 * project, it is assumed any cost overruns would be split among the
 * utilities. Interest and bond term, on the other hand, are utility
 * dependent are set at issuance, in function issueBond.
 * @param r
 * @param rdm_factors
 */
void Bond::setRealizationWaterSource(unsigned long r,
                                     vector<double> &rdm_factors) {
    cost_of_capital *= rdm_factors[0];
}

void Bond::issueBond(int week, int construction_time,
                     double bond_term_multiplier,
                     double bond_interest_rate_multiplier) {

    /// Set date to begin repayment
    if (begin_repayment_after_n_years == NON_INITIALIZED) {
        begin_repayment_after_n_years =
                (int) (construction_time / WEEKS_IN_YEAR) + 1;
    }

    week_issued = week;
    n_payments *= bond_term_multiplier;
    coupon_rate *= bond_interest_rate_multiplier;
    setIssued();
}

bool Bond::isIssued() const {
    return issued;
}

void Bond::setIssued() {
    Bond::issued = true;
}

void Bond::setDebtService(double updated_allocated_fraction_of_annual_debt_service) {
    throw logic_error("Error in Bond::setDebtService, "
                      "this should only be called when using "
                      "a VariableDebtServiceBond and in that "
                      "case the call would be overriden and "
                      "taken to that child class of Bond class.");
}

int Bond::getWaterSourceID() {
    return id;
}

double Bond::getCostOfCapital() {
    return cost_of_capital;
}

void Bond::adjustCostOfCapital(double reduction) {
    cost_of_capital -= reduction;
    if (cost_of_capital < 0)
        __throw_logic_error("Error in Bond::adjustCostOfCapital, adjusted cost_of_capital is negative");
}