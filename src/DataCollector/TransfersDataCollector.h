//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_TRANSFERSDATACOLLECTOR_H
#define TRIANGLEMODEL_TRANSFERSDATACOLLECTOR_H


#include <vector>
#include "Base/DataCollector.h"
#include "../DroughtMitigationInstruments/Transfers.h"

class TransfersDataCollector : public DataCollector {
private:
    vector<int> utilities_ids;
    vector<vector<double>> demand_offsets;
    Transfers *transfer_policy;

public:
    TransfersDataCollector(Transfers *transfer_policy, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;

};


#endif //TRIANGLEMODEL_TRANSFERSDATACOLLECTOR_H
