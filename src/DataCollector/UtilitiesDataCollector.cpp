//
// Created by bernardoct on 8/25/17.
//

#include <iomanip>
#include "UtilitiesDataCollector.h"


UtilitiesDataCollector::UtilitiesDataCollector(const Utility *utility)
        : DataCollector(utility->id,
                        utility->name,
                        UTILITY,
                        11 * COLUMN_WIDTH),
          utility(utility) {}

string UtilitiesDataCollector::printTabularString(int week) {

    stringstream outStream;

    outStream << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << combined_storage[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << capacity[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << st_rof[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << lt_rof[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << restricted_demand[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << unrestricted_demand[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << waste_water_discharge[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << contingency_fund_size[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << insurance_payout[week]
              << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << insurance_contract_cost[week];

    return outStream.str();
}

string UtilitiesDataCollector::printCompactString(int week) {

    stringstream outStream;

    outStream << combined_storage[week]
              << ","
              << capacity[week]
              << ","
              << st_rof[week]
              << ","
              << lt_rof[week]
              << ","
              << restricted_demand[week]
              << ","
              << unrestricted_demand[week]
              << ","
              << unfulfilled_demand[week]
              << ","
              << waste_water_discharge[week]
              << ","
              << contingency_fund_size[week]
              << ","
              << insurance_payout[week]
              << ","
              << insurance_contract_cost[week]
              << ",";

    return outStream.str();
}

string UtilitiesDataCollector::printTabularStringHeaderLine1() {

    stringstream outStream;

    outStream << setw(2 * COLUMN_WIDTH) << "Stored"
              << setw(COLUMN_WIDTH) << " "
              << setw(COLUMN_WIDTH) << " "
              << setw(COLUMN_WIDTH) << " "
              << setw(COLUMN_WIDTH) << "Rest."
              << setw(COLUMN_WIDTH) << "Unrest."
              << setw(COLUMN_WIDTH) << "Unfulf."
              << setw(COLUMN_WIDTH) << "W. Water"
              << setw(COLUMN_WIDTH) << "Conting."
              << setw(COLUMN_WIDTH) << "Insurance"
              << setw(COLUMN_WIDTH) << "Insurance";

    return outStream.str();
}

string UtilitiesDataCollector::printTabularStringHeaderLine2() {

    stringstream outStream;

    outStream << setw(2 * COLUMN_WIDTH) << "Volume"
              << setw(COLUMN_WIDTH) << "Capacity"
              << setw(COLUMN_WIDTH) << "ST-ROF"
              << setw(COLUMN_WIDTH) << "LT-ROF"
              << setw(COLUMN_WIDTH) << "Demand"
              << setw(COLUMN_WIDTH) << "Demand"
              << setw(COLUMN_WIDTH) << "Demand"
              << setw(COLUMN_WIDTH) << "Discharge"
              << setw(COLUMN_WIDTH) << "Fund"
              << setw(COLUMN_WIDTH) << "Payout"
              << setw(COLUMN_WIDTH) << "Price";

    return outStream.str();
}

string UtilitiesDataCollector::printCompactStringHeader() {
    stringstream outStream;

    outStream << id << "st_vol" << ","
              << id << "capacity" << ","
              << id << "st_rof" << ","
              << id << "lt_rof" << ","
              << id << "rest_demand" << ","
              << id << "unrest_demand" << ","
              << id << "unfulf_demand" << ","
              << id << "wastewater" << ","
              << id << "cont_fund" << ","
              << id << "ins_pout" << ","
              << id << "ins_price" << ",";

    return outStream.str();
}

void UtilitiesDataCollector::collect_data() {
    vector<int> infra_built;

    combined_storage.push_back(utility->getTotal_stored_volume());
    lt_rof.push_back(utility->getLong_term_risk_of_failure());
    st_rof.push_back(utility->getRisk_of_failure());
    unrestricted_demand.push_back(utility->getUnrestrictedDemand());
    restricted_demand.push_back(utility->getRestrictedDemand());
    contingency_fund_size.push_back(utility->getContingency_fund());
    net_present_infrastructure_cost
            .push_back(utility->getInfrastructure_net_present_cost());
    gross_revenues.push_back(utility->getGrossRevenue());
    debt_service_payments.push_back(utility->getCurrent_debt_payment());
    contingency_fund_contribution
            .push_back(utility->getCurrent_contingency_fund_contribution());
    drought_mitigation_cost.push_back(utility->getDrought_mitigation_cost());
    insurance_contract_cost.push_back(utility->getInsurance_purchase());
    insurance_payout.push_back(utility->getInsurance_payout());
    capacity.push_back(utility->getTotal_storage_capacity());
    waste_water_discharge.push_back(utility->getWaste_water_discharge());
    unfulfilled_demand.push_back(utility->getUnfulfilled_demand());

    infra_built = utility->getInfrastructure_built();
    if (pathways.empty() && !infra_built.empty())
        pathways.push_back(infra_built);
    else
        if (!infra_built.empty() && infra_built[2] != pathways.back()[2])
            pathways.push_back(infra_built);
}

const vector<double> &UtilitiesDataCollector::getCombined_storage() const {
    return combined_storage;
}

const vector<double> &UtilitiesDataCollector::getCapacity() const {
    return capacity;
}

const vector<double> &UtilitiesDataCollector::getGross_revenues() const {
    return gross_revenues;
}

const vector<double> &
UtilitiesDataCollector::getContingency_fund_contribution() const {
    return contingency_fund_contribution;
}

const vector<double> &UtilitiesDataCollector::getDebt_service_payments() const {
    return debt_service_payments;
}

const vector<double> &
UtilitiesDataCollector::getInsurance_contract_cost() const {
    return insurance_contract_cost;
}

const vector<double> &
UtilitiesDataCollector::getDrought_mitigation_cost() const {
    return drought_mitigation_cost;
}

const vector<double> &UtilitiesDataCollector::getContingency_fund_size() const {
    return contingency_fund_size;
}

const vector<vector<int>> &UtilitiesDataCollector::getPathways() const {
    return pathways;
}

const vector<double> &
UtilitiesDataCollector::getNet_present_infrastructure_cost() const {
    return net_present_infrastructure_cost;
}
