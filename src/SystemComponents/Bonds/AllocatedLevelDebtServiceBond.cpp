//
// Created by David on 4/23/2018.
//

#include "AllocatedLevelDebtServiceBond.h"
#include <cmath>
#include <algorithm>
#include "../../Utils/Utils.h"

AllocatedLevelDebtServiceBond::AllocatedLevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
                                           const double coupon_rate, vector<int> pay_on_weeks, const int water_source_id) :
        Bond(id, cost_of_capital, n_payments, pay_on_weeks, coupon_rate, ALLOCATED_LEVEL_DEBT_SERVICE) {
    Bond::water_source_id = water_source_id;
}

AllocatedLevelDebtServiceBond::AllocatedLevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
                                                             const double coupon_rate, vector<int> pay_on_weeks,
                                                             const int water_source_id, bool begin_repayment_at_issuance) :
        Bond(id, cost_of_capital, n_payments, pay_on_weeks, coupon_rate, ALLOCATED_LEVEL_DEBT_SERVICE, begin_repayment_at_issuance) {
    Bond::water_source_id = water_source_id;
}

/**
 * Calculates debt service payment for a give week.
 * @param week
 * @return
 */
double AllocatedLevelDebtServiceBond::getDebtService(int week) {
    /// If there are still payments to be made, repayment has begun, and this is a payment week, issue payment.
    if (n_payments_made < n_payments &&
        week > week_issued + begin_repayment_after_n_years * WEEKS_IN_YEAR - 1 &&
        std::find(pay_on_weeks.begin(), pay_on_weeks.end(), Utils::weekOfTheYear(week)) != pay_on_weeks.end()) {

        n_payments_made++;
        return level_debt_service_payment;
    } else {
        return 0.;
    }
}


double AllocatedLevelDebtServiceBond::getNetPresentValueAtIssuance(double yearly_discount_rate, int week) const {
    double npv_at_first_payment_date = level_debt_service_payment *
                                       (1. - pow(1. + (yearly_discount_rate / pay_on_weeks.size()), -n_payments)) /
                                       (yearly_discount_rate / pay_on_weeks.size());

    double npv = npv_at_first_payment_date / pow(1. + yearly_discount_rate, begin_repayment_after_n_years);

    return npv;
}

void AllocatedLevelDebtServiceBond::issueBond(int week, int construction_time, double bond_term_multiplier, double bond_interest_rate_multiplier) {
    Bond::issueBond(week, construction_time, bond_term_multiplier, bond_interest_rate_multiplier);

    /// Level debt service payment value
    level_debt_service_payment = cost_of_capital * (coupon_rate * pow(1. + coupon_rate, n_payments)) /
                                 (pow(1. + coupon_rate, n_payments) - 1.);
}


/**
 * Allocate cost responsibility to bond based on utility allocation in project
 * ASSUMES THAT THE PROJECT COST PASSED IN BOND CONSTRUCTOR IS COST OF ENTIRE PROJECT
 */
void AllocatedLevelDebtServiceBond::setAllocatedDebtService(int week, double debt_obligation_fraction) {
    /// If there are still payments to be made, repayment has begun, and this is a payment week, calculate debt service.
    if (n_payments_made < n_payments &&
        week > week_issued + begin_repayment_after_n_years * WEEKS_IN_YEAR - 1 &&
        std::find(pay_on_weeks.begin(), pay_on_weeks.end(), Utils::weekOfTheYear(week)) != pay_on_weeks.end()) {

        /// set temporary debt service as if payment covering entire debt responsibility was due
        double temp_level_debt_service_payment = cost_of_capital * (coupon_rate * pow(1. + coupon_rate, n_payments)) /
                                                 (pow(1. + coupon_rate, n_payments) - 1.);

        /// calculate allocated debt service based on utility use
        level_debt_service_payment = temp_level_debt_service_payment * debt_obligation_fraction;
    }
}

/**
 * Allocate cost responsibility to bond based on utility allocation in project
 * ASSUMES THAT THE PROJECT COST PASSED IN BOND CONSTRUCTOR IS COST OF ENTIRE PROJECT
 */
double AllocatedLevelDebtServiceBond::setRetroactiveDebtServicePayment(int week,
                                                                       double allocation_adjustment_capacity_fraction) {
    double retroactive_payment = 0.0;

    /// If repayment has begun, and this is a payment week, calculate retroactive debt service.
    /// THESE CAN CONTINUE AFTER REPAYMENT PERIOD ENDS BECAUSE THEY ARE NOT PAYMENTS ON THE BOND
    /// BUT RETROACTIVE REIMBURSEMENTS BETWEEN UTILITIES
    if (week > week_issued + begin_repayment_after_n_years * WEEKS_IN_YEAR - 1 &&
        std::find(pay_on_weeks.begin(), pay_on_weeks.end(), Utils::weekOfTheYear(week)) != pay_on_weeks.end()) {

        /// set temporary debt service as if payment covering entire debt responsibility was due
        double temp_level_debt_service_payment = cost_of_capital * (coupon_rate * pow(1. + coupon_rate, n_payments)) /
                                                 (pow(1. + coupon_rate, n_payments) - 1.);

        /// Sum total payments made to this point
        double temp_total_repaid_plus_interest = temp_level_debt_service_payment * n_payments_made;

        /// Determine retroactive payment to or from utility holding bond
        retroactive_payment = temp_total_repaid_plus_interest * allocation_adjustment_capacity_fraction;
    }

    return retroactive_payment;
}

double AllocatedLevelDebtServiceBond::printDebtService(int week) {
    if (n_payments_made < n_payments &&
        week > week_issued + begin_repayment_after_n_years * WEEKS_IN_YEAR - 1 &&
        std::find(pay_on_weeks.begin(), pay_on_weeks.end(), Utils::weekOfTheYear(week)) != pay_on_weeks.end())
        return level_debt_service_payment;
    else
        return 0.;

}
