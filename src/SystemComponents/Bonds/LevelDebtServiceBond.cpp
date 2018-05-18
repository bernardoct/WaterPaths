//
// Created by bernardo on 4/12/18.
//

#include <cmath>
#include <algorithm>
#include "LevelDebtServiceBond.h"
#include "../../Utils/Utils.h"

LevelDebtServiceBond::LevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
                                           const double coupon_rate, vector<int> pay_on_weeks,
                                           bool begin_repayment_at_issuance) :
        Bond(id, cost_of_capital, n_payments, pay_on_weeks, coupon_rate, LEVEL_DEBT_SERVICE, begin_repayment_at_issuance) {
}

LevelDebtServiceBond::~LevelDebtServiceBond() = default;

/**
 * Calculates debt service payment for a give week.
 * @param week
 * @return
 */
double LevelDebtServiceBond::getDebtService(int week) {
    /// If there are still payments to be made, repayment has begun, and this is a payment week, issue payment.
    if (n_payments_made < n_payments &&
            week > week_issued + begin_repayment_after_n_years * WEEKS_IN_YEAR - 1&&
            std::find(pay_on_weeks.begin(), pay_on_weeks.end(), Utils::weekOfTheYear(week)) != pay_on_weeks.end()) {

        n_payments_made++;
        return level_debt_service_payment;
    } else {
        return 0.;
    }
}


double LevelDebtServiceBond::getNetPresentValueAtIssuance(double yearly_discount_rate, int week) const {
    double npv_at_first_payment_date = level_debt_service_payment *
                                       (1. - pow(1. + (yearly_discount_rate / pay_on_weeks.size()), -n_payments)) /
                                       (yearly_discount_rate / pay_on_weeks.size());

    double npv = npv_at_first_payment_date / pow(1. + yearly_discount_rate, begin_repayment_after_n_years);

    return npv;
}

void LevelDebtServiceBond::issueBond(int week, int construction_time, double bond_term_multiplier,
                                     double bond_interest_rate_multiplier) {
    Bond::issueBond(week, construction_time, bond_term_multiplier, bond_interest_rate_multiplier);

    /// Level debt service payment value
    level_debt_service_payment = cost_of_capital * (coupon_rate * pow(1. + coupon_rate, n_payments)) /
                                 (pow(1. + coupon_rate, n_payments) - 1.);
}

double LevelDebtServiceBond::printDebtService(int week) {
    return level_debt_service_payment;
}

