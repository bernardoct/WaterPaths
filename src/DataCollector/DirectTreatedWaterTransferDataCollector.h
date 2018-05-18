//
// Created by David on 5/17/2018.
//

#ifndef TRIANGLEMODEL_DIRECTTREATEDWATERTRANSFERDATACOLLECTOR_H
#define TRIANGLEMODEL_DIRECTTREATEDWATERTRANSFERDATACOLLECTOR_H


#include "Base/DataCollector.h"
#include "../DroughtMitigationInstruments/DirectTreatedWaterTransfer.h"

class DirectTreatedWaterTransferDataCollector : public DataCollector {
    DirectTreatedWaterTransfer *dtwt_policy;
    
    int buying_utility;
    vector<int> possible_seller_utilities;
    vector<double> weekly_initial_requested_transfer_volume;
    vector<vector<double>> weekly_available_treatment_capacity_to_sell;
    vector<vector<double>> weekly_available_supply_capacity_to_sell;
    vector<vector<double>> weekly_quantities_sold;

public:
    DirectTreatedWaterTransferDataCollector(DirectTreatedWaterTransfer *dtwt, unsigned long realization);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    void collect_data() override;
};


#endif //TRIANGLEMODEL_DIRECTTREATEDWATERTRANSFERDATACOLLECTOR_H
