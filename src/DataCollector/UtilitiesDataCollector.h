//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_UTILITIESDATACOLLECTOR_H
#define TRIANGLEMODEL_UTILITIESDATACOLLECTOR_H


#include "Base/DataCollector.h"
#include "../SystemComponents/Utility.h"

class UtilitiesDataCollector : public DataCollector {
private:
    vector<float> st_rof;
    vector<float> lt_rof;
    vector<float> combined_storage;
    vector<float> unrestricted_demand;
    vector<float> restricted_demand;
    vector<float> contingency_fund_size;
    vector<float> gross_revenues;
    vector<float> contingency_fund_contribution;
    vector<float> debt_service_payments;
    vector<float> insurance_contract_cost;
    vector<float> insurance_payout;
    vector<float> drought_mitigation_cost;
    vector<float> capacity;
    vector<vector<int>> pathways;
    vector<float> net_present_infrastructure_cost;
    vector<float> waste_water_discharge;
    vector<float> unfulfilled_demand;
    const Utility *utility;

public:

    explicit UtilitiesDataCollector(const Utility *utility);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    void collect_data() override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    const vector<float> &getCombined_storage() const;

    const vector<float> &getCapacity() const;

    const vector<float> &getGross_revenues() const;

    const vector<float> &getContingency_fund_contribution() const;

    const vector<float> &getDebt_service_payments() const;

    const vector<float> &getInsurance_contract_cost() const;

    const vector<float> &getDrought_mitigation_cost() const;

    const vector<float> &getContingency_fund_size() const;

    const vector<vector<int>> &getPathways() const;

    const vector<float> &getNet_present_infrastructure_cost() const;

};


#endif //TRIANGLEMODEL_UTILITIESDATACOLLECTOR_H
