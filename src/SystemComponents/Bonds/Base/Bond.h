//
// Created by bernardo on 4/12/18.
//

#ifndef TRIANGLEMODEL_BONDFINANCING_H
#define TRIANGLEMODEL_BONDFINANCING_H

#include <vector>
#include "../../../Utils/Constants.h"

using namespace std;
using namespace Constants;

class Bond {
private:
    bool issued = false;
protected:
    int week_issued;
    int begin_repayment_after_n_years = NON_INITIALIZED;
    double coupon_rate;
    double cost_of_capital;
    int n_payments;
public:
    const int type;
    const vector<int> pay_on_weeks;
    const int id;

    Bond(const int id, const double cost_of_capital, const int n_payments, vector<int> pay_on_weeks,
                  const int type, bool begin_repayment_at_issuance = false);

    Bond(const int id, const double cost_of_capital, const int n_payments, vector<int> pay_on_weeks,
         const double coupon_rate, const int type, bool begin_repayment_at_issuance = false);

    Bond();

    virtual ~Bond();

    Bond(const Bond&) = default;

    virtual double getDebtService(int week) = 0;

    virtual double getNetPresentValueAtIssuance(double discount_rate, int week) const = 0;

    virtual void
    issueBond(int week, int construction_time, double bond_term_multiplier, double bond_interest_rate_multiplier);

    virtual void setRealizationWaterSource(unsigned long r, vector<double> &rdm_factors);

    bool isIssued() const;

    void setIssued();

    double getCouponRate() const;

    double getCostOfCapital() const;

    int getNPayments() const;

};


#endif //TRIANGLEMODEL_BONDFINANCING_H
