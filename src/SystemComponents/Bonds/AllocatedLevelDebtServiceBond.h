//
// Created by David on 4/23/2018.
//

#ifndef TRIANGLEMODEL_ALLOCATEDLEVELDEBTSERVICEBOND_H
#define TRIANGLEMODEL_ALLOCATEDLEVELDEBTSERVICEBOND_H


#include "Base/Bond.h"

class AllocatedLevelDebtServiceBond : public Bond {
private:
    double level_debt_service_payment;
    const int begin_repayment_after_n_years;
    int n_payments_made = 0;

public:
    AllocatedLevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
    const double coupon_rate, vector<int> pay_on_weeks, const int water_source_id);

    AllocatedLevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
    const double coupon_rate, vector<int> pay_on_weeks,
                                  const int water_source_id, const int starts_paying_after_n_years);

    double getDebtService(int week) override;

    double getNetPresentValueAtIssuance(double yearly_discount_rate, int week) const override;

    void issueBond(int week, double bond_term_multiplier, double bond_interest_rate_multiplier) override;

    void setAllocatedDebtService(int week, double debt_obligation_fraction) override;

    double setRetroactiveDebtServicePayment(int week, double allocation_adjustment_capacity_fraction) override;
};


#endif //TRIANGLEMODEL_ALLOCATEDLEVELDEBTSERVICEBOND_H
