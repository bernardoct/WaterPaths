//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include "WaterSources/Reservoir.h"
#include "../Utils/Constants.h"
#include "../Controls/WwtpDischargeRule.h"
//#include "../Utils/Matrix3D.h"


class Utility {
private:

    const int NUMBER_OF_MONTHS = 12;
    const double WEEKS_IN_MONTH = WEEKS_IN_YEAR / (double) NUMBER_OF_MONTHS;
    double fund_contribution;
    double *demand_series = NULL;
    double *weekly_average_volumetric_price;
    vector<int> priority_draw_water_source;
    vector<int> non_priority_draw_water_source;
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
    double waste_water_discharge = 0;
    double gross_revenue = 0;
    bool under_construction = false;
    int construction_end_date = 0;
//    map<int, WaterSource *> water_sources;
    vector<WaterSource *> water_sources;
    vector<unsigned int> infrastructure_construction_order;
    vector<vector<double>> debt_payment_streams;
    vector<int> infrastructure_built_last_week;
    vector<vector<double>> *demands_all_realizations;
    WwtpDischargeRule wwtp_discharge_rule;

public:
    const int id;
    const int number_of_week_demands;
    const char *name;
    const double percent_contingency_fund_contribution;
    const double infrastructure_discount_rate;

    Utility(
            char *name, int id,
            vector<vector<double>> *demands_all_realizations,
            int number_of_week_demands,
            const double percent_contingency_fund_contribution,
            const vector<vector<double>> *typesMonthlyDemandFraction,
            const vector<vector<double>> *typesMonthlyWaterPrice,
            WwtpDischargeRule wwtp_discharge_rule);

    Utility(
            char *name, int id,
            vector<vector<double>> *demands_all_realizations,
            int number_of_week_demands,
            const double percent_contingency_fund_contribution,
            const vector<vector<double>> *typesMonthlyDemandFraction,
            const vector<vector<double>> *typesMonthlyWaterPrice,
            WwtpDischargeRule wwtp_discharge_rule,
            vector<unsigned int> infrastructure_construction_order,
            double infrastructure_discount_rate);

    Utility(Utility &utility);

    ~Utility();

    Utility &operator=(const Utility &utility);

    bool operator<(const Utility* other);

    bool operator>(const Utility* other);

    void setRisk_of_failure(double risk_of_failure);

    double getRisk_of_failure() const;

    void updateTotalStoredVolume();

    void calculateWastewater_releases(int week, double *discharges);

    void addWaterSource(WaterSource *water_source);

    double getStorageToCapacityRatio() const;

    void splitDemands(int week, vector<vector<double>> *demands);

    double getTotal_storage_capacity() const;

    void setDemand_multiplier(double demand_multiplier);

    void setDemand_offset(double demand_offset, double offset_rate_per_volume);

    double getTotal_treatment_capacity() const;

    void updateContingencyFund(
            double unrestricted_demand, double demand_multiplier,
            double demand_offset, int week);

    double getContingency_fund() const;

    double getUnrestrictedDemand() const;

    double getRestrictedDemand() const;

    void beginConstruction(int week, int infra_id);

    void setWaterSourceOnline(unsigned int source_id);

    void infrastructureConstructionHandler(double long_term_rof, int week);

    double getDemand_multiplier() const;

    double getUnrestrictedDemand(int week) const;

    double getInfrastructure_net_present_cost() const;

    double getCurrent_debt_payment() const;

    double updateCurrent_debt_payment(int week);

    double getCurrent_contingency_fund_contribution() const;

    double getDrought_mitigation_cost() const;

    void addInsurancePayout(double payout_value);

    void clearWaterSources();

    void purchaseInsurance(double insurance_price);

    double getInsurance_payout() const;

    double getInsurance_purchase() const;

    void resetDataColletionVariables();

    const vector<unsigned int> &getInfrastructure_construction_order() const;

    void setRelization(unsigned long r);

    const vector<int> getInfrastructure_built() const;

    void calculateWeeklyAverageWaterPrices(
            const vector<vector<double>> *typesMonthlyDemandFraction,
            const vector<vector<double>> *typesMonthlyWaterPrice);

    const double waterPrice(int week);

    double getGrossRevenue() const;

    static bool compById(Utility *a, Utility *b);
};


#endif //TRIANGLEMODEL_UTILITY_H
