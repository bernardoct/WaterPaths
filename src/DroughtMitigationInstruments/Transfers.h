//
// Created by bernardo on 2/21/17.
//

#ifndef TRIANGLEMODEL_TRANSFERS_H
#define TRIANGLEMODEL_TRANSFERS_H

#include "DroughtMitigationPolicy.h"

class Transfers : public DroughtMitigationPolicy {
private:
    const int source_utility_id;
    Utility *source_utility;
    const double source_treatment_buffer;
    const vector<double> buyers_transfer_capacities;
    const vector<double> buyers_transfer_triggers;
    vector<Utility *> buying_utilities;

public:
    Transfers(const int id, const vector<int> &utilities_id, const int source_utility_id,
              const double source_treatment_buffer, const vector<double> &buyers_transfer_capacities,
              const vector<double> &buyers_transfer_triggers);

    void addSourceUtility(Utility *source_utility);

    virtual void applyPolicy(int week) override;

    virtual void addUtility(Utility *utility) override;

};


#endif //TRIANGLEMODEL_TRANSFERS_H
