//
// Created by bernardo on 2/21/17.
//

#ifndef TRIANGLEMODEL_TRANSFERS_H
#define TRIANGLEMODEL_TRANSFERS_H

#include "DroughtMitigationPolicy.h"

class Transfers : public DroughtMitigationPolicy {
private:
    const int source_utility_id;
    const double source_treatment_buffer;
    const vector<vector<int>> buyers_ids;
    vector<double> buyers_transfer_capacities;
    vector<double> buyers_transfer_triggers;
    Utility *source_utility;
    map<int, Utility *> buying_utilities;
    unsigned long highest_utility_id = 0;

public:

    Transfers(const int id, const int source_utility_id,
              const double source_treatment_buffer, const vector<vector<int>> &buyers_ids,
              const vector<vector<double>> &buyers_transfer_capacities,
              const vector<vector<double>> &buyers_transfer_triggers);

    virtual void applyPolicy(int week) override;

    virtual void addUtility(Utility *utility) override;

};


#endif //TRIANGLEMODEL_TRANSFERS_H
