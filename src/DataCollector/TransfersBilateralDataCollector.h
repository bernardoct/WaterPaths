//
// Created by Bernardo on 5/10/2020.
//

#ifndef WATERPATHS_TRANSFERSBILATERALDATACOLLECTOR_H
#define WATERPATHS_TRANSFERSBILATERALDATACOLLECTOR_H

#include "Base/DataCollector.h"
#include "../DroughtMitigationInstruments/TransfersBilateral.h"

class TransfersBilateralDataCollector : public DataCollector {
private:
    TransfersBilateral *transfer_policy;
    vector<vector<double>> demand_offsets;
    vector<int> utilities_ids;

public:
    TransfersBilateralDataCollector(TransfersBilateral *transfer_policy,
                                    unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //WATERPATHS_TRANSFERSBILATERALDATACOLLECTOR_H
