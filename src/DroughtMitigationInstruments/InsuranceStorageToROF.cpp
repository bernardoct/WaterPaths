//
// Created by bernardoct on 5/1/17.
//

#include "InsuranceStorageToROF.h"

InsuranceStorageToROF::InsuranceStorageToROF(const int id, const int type, const double rof_trigger,
                                             const double insurance_premium, const double fixed_payouts)
        : DroughtMitigationPolicy(id, type), rof_trigger(rof_trigger), insurance_premium(insurance_premium),
          fixed_payouts(fixed_payouts) {}

