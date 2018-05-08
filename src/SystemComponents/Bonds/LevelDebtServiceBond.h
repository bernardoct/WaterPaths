//
// Created by bernardo on 4/12/18.
//

#ifndef TRIANGLEMODEL_LEVELDEBTSERVICEBOND_H
#define TRIANGLEMODEL_LEVELDEBTSERVICEBOND_H


#include "Base/Bond.h"

class LevelDebtServiceBond : public Bond {
private:
    double level_debt_service_payment;
    const int begin_repayment_after_n_years;
    int n_payments_made = 0;

public:
    LevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
                         const double coupon_rate, vector<int> pay_on_weeks);

    LevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
                         const double coupon_rate, vector<int> pay_on_weeks, const int starts_paying_after_n_years);

    double getDebtService(int week) override;

    double getNetPresentValueAtIssuance(double yearly_discount_rate, int week) const override;

    void issueBond(int week, double bond_term_multiplier, double bond_interest_rate_multiplier) override;

};


#endif //TRIANGLEMODEL_LEVELDEBTSERVICEBOND_H
