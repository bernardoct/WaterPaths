//
// Created by bernardoct on 8/25/17.
//

#ifndef TRIANGLEMODEL_UTILITIESDATACOLLECTOR_H
#define TRIANGLEMODEL_UTILITIESDATACOLLECTOR_H


#include "Base/DataCollector.h"
#include "../SystemComponents/Utility/Utility.h"

class UtilitiesDataCollector : public DataCollector {
private:
    vector<double> st_rof;
    vector<double> lt_rof;
    vector<double> lt_stor_rof;
    vector<double> lt_trmt_rof;
    vector<double> combined_storage;
    vector<double> unrestricted_demand;
    vector<double> restricted_demand;
    vector<double> contingency_fund_size;
    vector<double> gross_revenues;
    vector<double> contingency_fund_contribution;
    vector<double> debt_service_payments;
    vector<double> insurance_contract_cost;
    vector<double> insurance_payout;
    vector<double> drought_mitigation_cost;
    vector<double> capacity;
    vector<double> net_present_infrastructure_cost;
    vector<double> waste_water_discharge;
    vector<double> unfulfilled_demand;
    vector<double> net_stream_inflow;
    vector<double> total_treatment_capacity;
    vector<double> final_stored_volume;
    vector<vector<int>> pathways;
    const Utility *utility;

public:

    explicit UtilitiesDataCollector(const Utility *utility, unsigned long realization);

    UtilitiesDataCollector &operator=(const UtilitiesDataCollector &utility_data_collector);

    string printTabularString(int week) override;

    string printCompactString(int week) override;

    void collect_data() override;

    string printTabularStringHeaderLine1() override;

    string printTabularStringHeaderLine2() override;

    string printCompactStringHeader() override;

    const vector<double> &getCombined_storage() const;

    const vector<double> &getCapacity() const;

    const vector<double> &getGross_revenues() const;

    const vector<double> &getContingency_fund_contribution() const;

    const vector<double> &getDebt_service_payments() const;

    const vector<double> &getInsurance_contract_cost() const;

    const vector<double> &getDrought_mitigation_cost() const;

    const vector<double> &getContingency_fund_size() const;

    const vector<vector<int>> &getPathways() const;

    const vector<double> &getNet_present_infrastructure_cost() const;

    const vector<double> &getSt_rof() const;

    const vector<double> &getLt_rof() const;

    const vector<double> &getRestricted_demand() const;

    void checkForNans() const;
};


#endif //TRIANGLEMODEL_UTILITIESDATACOLLECTOR_H
