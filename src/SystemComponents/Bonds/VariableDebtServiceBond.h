//
// Created by dgorelic on 11/4/2019.
//

#ifndef TRIANGLEMODEL_VARIABLEDEBTSERVICEBOND_H
#define TRIANGLEMODEL_VARIABLEDEBTSERVICEBOND_H


#include "Base/Bond.h"

class VariableDebtServiceBond : public Bond {
private:
    double variable_debt_service_payment;
    double initial_debt_fraction;
    int water_source_id;
    int n_payments_made = 0;

public:
    VariableDebtServiceBond(const int id, const int water_source_id,
                            const double total_unallocated_cost_of_capital,
                            const double initial_fraction_of_allocated_debt,
                            const int n_payments,
                            const double coupon_rate,
                            vector<int> pay_on_weeks,
                            bool begin_repayment_at_issuance = false);

    ~VariableDebtServiceBond() override;

    double getDebtService(int week) override;

    double getPresentValueDebtService(int week, double discount_rate) override;

    double getNetPresentValueAtIssuance(double yearly_discount_rate, int week) const override;

    void issueBond(int week, int construction_time, double bond_term_multiplier,
                   double bond_interest_rate_multiplier) override;

    void setDebtService(double updated_allocated_fraction_of_annual_debt_service) override;

    int getWaterSourceID() override;
};


#endif //TRIANGLEMODEL_VARIABLEDEBTSERVICEBOND_H
