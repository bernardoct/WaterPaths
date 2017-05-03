//
// Created by bernardoct on 5/1/17.
//

#ifndef TRIANGLEMODEL_INSURANCESTORAGETOROF_H
#define TRIANGLEMODEL_INSURANCESTORAGETOROF_H


#include "DroughtMitigationPolicy.h"
#include "../Utils/Matrix3D.h"

class InsuranceStorageToROF : public DroughtMitigationPolicy {
private:
    double insurance_price;

public:
    InsuranceStorageToROF(const int id, const int type, const double rof_trigger, const double insurance_premium,
                          const double fixed_payouts);

    const double rof_trigger;
    const double insurance_premium;
    const double fixed_payouts;
    Matrix3D<double> *storage_rof_table;

};


#endif //TRIANGLEMODEL_INSURANCESTORAGETOROF_H
