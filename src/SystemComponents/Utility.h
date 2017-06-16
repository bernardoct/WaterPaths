//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include "WaterSources/Reservoir.h"
#include "../Utils/Constants.h"
//#include "../Utils/Matrix3D.h"


class Utility {
private:
    double *demand_series = new double[0];
    double risk_of_failure;
    double total_storage_capacity = 0;
    double total_stored_volume = 0;
    double total_treatment_capacity;
    double demand_multiplier = 1;
    double demand_offset = 0;
    double offset_rate_per_volume = 0;
    double contingency_fund = 0;
    double drought_mitigation_cost = 0;
    double insurance_payout = 0;
    double insurance_purchase = 0;
    double restricted_demand = 0;
    double unrestricted_demand = 0;
    double infrastructure_net_present_cost = 0;
    double current_debt_payment = 0;
    bool underConstruction = false;
    int construction_start_date;
    map<int, WaterSource *> water_sources;
    vector<int> infrastructure_construction_order;
    vector<vector<double>> debt_payment_streams;
    vector<vector<int>> infrastructure_built;
    vector<vector<double>> *demands_all_realizations;

    void setWaterSourceOnline(int source_id);

public:
    const int id;
    const double water_price_per_volume;
    const int number_of_week_demands;
    const char *name;
    const double percent_contingency_fund_contribution;
    const double infrastructure_discount_rate;

    Utility(char *name, int id, vector<vector<double>> *demands_all_realizations, int number_of_week_demands,
            const double percent_contingency_fund_contribution, const double water_price_per_volume);

    Utility(char *name, int id, vector<vector<double>> *demands_all_realizations, int number_of_week_demands,
            const double percent_contingency_fund_contribution, const double water_price_per_volume,
            const vector<int> infrastructure_build_order, double infrastructure_discount_rate);

    Utility(Utility &utility);

    ~Utility();

    Utility &operator=(const Utility &utility);

    bool operator<(const Utility* other);

    bool operator>(const Utility* other);

    void setRisk_of_failure(double risk_of_failure);

    double getRisk_of_failure() const;

    void updateTotalStoredVolume();

    const map<int, WaterSource *> &getWaterSources() const;

    void addWaterSource(WaterSource *water_source);

    double getStorageToCapacityRatio() const;

    void splitDemands(int week, double *water_sources_draws);

    double getTotal_storage_capacity() const;

    void setDemand_multiplier(double demand_multiplier);

    void setDemand_offset(double demand_offset, double offset_rate_per_volume);

    double getTotal_treatment_capacity() const;

    double getTotal_available_volume() const;

    void updateContingencyFund(double unrestricted_demand, double demand_multiplier, double demand_offset);

    double getContingency_fund() const;

    double getUnrestrictedDemand() const;

    double getRestrictedDemand() const;

    void beginConstruction(int week);

    void infrastructureConstructionHandler(double long_term_rof, int week);

    double getDemand_multiplier() const;

    double getUnrestrictedDemand(int week) const;

    double getInfrastructure_net_present_cost() const;

    double getCurrent_debt_payment() const;

    double updateCurrent_debt_payment(int week, vector<vector<double>> debt_payment_streams);

    double getCurrent_contingency_fund_contribution() const;

    double getDrought_mitigation_cost() const;

    void addInsurancePayout(double payout_value);

    void clearWaterSources();

    void purchaseInsurance(double insurance_price);

    double getInsurance_payout() const;

    double getInsurance_purchase() const;

    void resetDataColletionVariables();

    const vector<int> &getInfrastructure_construction_order() const;

    void setRelization(unsigned long r);
};


#endif //TRIANGLEMODEL_UTILITY_H
