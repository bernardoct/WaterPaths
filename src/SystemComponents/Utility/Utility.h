//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include <memory>
#include "../WaterSources/Reservoir.h"
#include "../../Utils/Constants.h"
#include "../../Controls/WwtpDischargeRule.h"
#include "InfrastructureManager.h"
//#include "../Utils/Matrix3D.h"

class Utility {
private:
    vector<double> weekly_average_volumetric_price;
    vector<int> priority_draw_water_source;
    vector<int> non_priority_draw_water_source;
    vector<double> weekly_peaking_factor;
    double short_term_risk_of_failure = 0;
    double short_term_storage_risk_of_failure = 0;
    double short_term_treatment_risk_of_failure = 0;
    double long_term_actual_risk_of_failure = 0;
    double long_term_treatment_risk_of_failure = 0;
    double long_term_storage_risk_of_failure = 0;
    double total_storage_capacity = 0;
    double total_available_volume = 0;
    double total_stored_volume = 0;
    double total_treatment_capacity = 0;
    double waste_water_discharge = 0;
    double gross_revenue = 0;
    double unfulfilled_demand = 0;
    double net_stream_inflow = 0;
    bool used_for_realization = true;
    vector<WaterSource *> water_sources;
    WwtpDischargeRule wwtp_discharge_rule;
    vector<vector<double>>& demands_all_realizations;
    vector<double>& annual_demand_projections;
    vector<double> demand_series_realization;
    double *rdm_factors_realization;
    InfrastructureManager infrastructure_construction_manager;

    /// Drought mitigation
    double fund_contribution = 0;
    double contingency_fund_cap = 1;
    double demand_multiplier = 1;
    double demand_offset = 0;
    double restricted_price = NON_INITIALIZED;
    double offset_rate_per_volume = 0;
    double contingency_fund = 0;
    double drought_mitigation_cost = 0;
    double insurance_payout = 0;
    double insurance_purchase = 0;
    double restricted_demand = 0;
    double unrestricted_demand = 0;
    int n_sources = 0;
    double infra_discount_rate;
    double bond_term_multiplier;
    double bond_interest_rate_multiplier;
    double max_capacity = 0;

    /// Infrastructure cost
    double current_debt_payment = 0;
    double current_present_valued_debt_payment = 0;
    vector<vector<double>> debt_payment_streams;
    double infra_net_present_cost = 0;
    vector<Bond *> issued_bonds;

    /// Demand projection for ROF
    double future_demand_estimate = 0;
    double current_year_recorded_demand = 0;
    vector<double> annual_average_weekly_demand;
    int demand_projection_forecast_length = LOOK_AHEAD_YEARS_FOR_DEMAND_PROJECTION;
    int demand_projection_historical_period_to_use = LOOK_BACK_YEARS_FOR_DEMAND_REPROJECTION;
    int demand_projection_reprojection_frequency = FREQUENCY_OF_DEMAND_REPROJECTION_YEARS;

public:
    const int id;
    const int number_of_week_demands;
    const char *name;
    const double percent_contingency_fund_contribution;
    const double demand_buffer;

    Utility(
            const char *name, int id,
            vector<vector<double>>& demands_all_realizations,
            vector<double>& annual_demand_projections,
            int number_of_week_demands,
            const double percent_contingency_fund_contribution,
            const double contingency_fund_cap,
            const vector<vector<double>> &typesMonthlyDemandFraction,
            const vector<vector<double>> &typesMonthlyWaterPrice,
            WwtpDischargeRule wwtp_discharge_rule,
            double demand_buffer,
            int demand_projection_forecast_length,
            int demand_projection_historical_period_to_use,
            int demand_projection_reprojection_frequency);

    Utility(const char *name, int id, vector<vector<double>>& demands_all_realizations, vector<double>& annual_demand_projections,
                int number_of_week_demands, const double percent_contingency_fund_contribution,
                const double contingency_fund_cap,
                const vector<vector<double>> &typesMonthlyDemandFraction,
                const vector<vector<double>> &typesMonthlyWaterPrice, WwtpDischargeRule wwtp_discharge_rule,
                double demand_buffer, const vector<int> &rof_infra_construction_order,
                const vector<int> &demand_infra_construction_order,
                const vector<double> &infra_construction_triggers, double infra_discount_rate,
                const vector<vector<int>>& infra_if_built_remove, double bond_term, double bond_interest_rate,
                int demand_projection_forecast_length,
                int demand_projection_historical_period_to_use,
                int demand_projection_reprojection_frequency);

    Utility(const char *name, int id, vector<vector<double>>& demands_all_realizations, vector<double>& annual_demand_projections,
                int number_of_week_demands, const double percent_contingency_fund_contribution,
                const double contingency_fund_cap,
                const vector<vector<double>> &typesMonthlyDemandFraction,
                const vector<vector<double>> &typesMonthlyWaterPrice, WwtpDischargeRule wwtp_discharge_rule,
                double demand_buffer, const vector<int> &rof_infra_construction_order,
                const vector<int> &demand_infra_construction_order,
                const vector<double> &infra_construction_triggers, double infra_discount_rate, double bond_term,
                double bond_interest_rate,
                int demand_projection_forecast_length,
                int demand_projection_historical_period_to_use,
                int demand_projection_reprojection_frequency);

    Utility(Utility &utility);

    ~Utility();

    Utility &operator=(const Utility &utility);

    bool operator<(const Utility* other);

    bool operator>(const Utility* other);

    static bool compById(Utility *a, Utility *b);

    void setRisk_of_failure(double risk_of_failure);

    void updateTotalAvailableVolume();

    void calculateWastewater_releases(int week, double *discharges);

    double calculateCurrentToNextYearDemandDelta(int current_year);

    void addWaterSource(WaterSource *water_source);

    void splitDemands(
            int week, vector<vector<double>> &demands, bool
    apply_demand_buffer = false, bool apply_demand_projection = false);

    void checkErrorsAddWaterSourceOnline(WaterSource *water_source);

    void resetDroughtMitigationVariables();

    void issueBond(int new_infra_triggered, int week);

    void calculateWeeklyAverageWaterPrices(
            const vector<vector<double>> &typesMonthlyDemandFraction,
            const vector<vector<double>> &typesMonthlyWaterPrice);

    double waterPrice(int week);

    void forceInfrastructureConstruction(int week, vector<int> new_infra_triggered);

    int infrastructureConstructionHandler(double long_term_rof, int week);

    void priceCalculationErrorChecking(
            const vector<vector<double>> &typesMonthlyDemandFraction,
            const vector<vector<double>> &typesMonthlyWaterPrice);

    double getTotal_storage_capacity() const;

    double getRisk_of_failure() const;

    double getStorageToCapacityRatio() const;

    double getGrossRevenue() const;

    void setDemand_multiplier(double demand_multiplier);

    void setDemand_offset(double demand_offset, double offset_rate_per_volume);

    double getTotal_treatment_capacity() const;

    void updateContingencyFundAndDebtService(
            double unrestricted_demand, double demand_multiplier,
            double demand_offset, double unfulfilled_demand, int week);

    void setWaterSourceOnline(unsigned int source_id, int week);

    double updateCurrent_debt_payment(int week);

    double getContingency_fund() const;

    double getUnrestrictedDemand() const;

    double getRestrictedDemand() const;

    void setRestricted_price(double restricted_price);

    double getDemand_multiplier() const;

    double getUnrestrictedDemand(int week) const;

    double getInfrastructure_net_present_cost() const;

    double getCurrent_debt_payment() const;

    double getCurrent_contingency_fund_contribution() const;

    double getDrought_mitigation_cost() const;

    void addInsurancePayout(double payout_value);

    void clearWaterSources();

    void purchaseInsurance(double insurance_price);

    double getInsurance_payout() const;

    double getInsurance_purchase() const;

    const vector<int> &getRof_infrastructure_construction_order() const;

    void setRealization(unsigned long r, vector<double>& rdm_factors);

    const vector<int> getInfrastructure_built() const;

    void setNoFinaicalCalculations();

    double getLong_term_risk_of_failure() const;

    const vector<int> &getDemand_infra_construction_order() const;

    vector<double> calculateWeeklyPeakingFactor(vector<double> *demands);

    const vector<WaterSource *> &getWater_sources() const;

    double getWaste_water_discharge() const;

    double getTotal_available_volume() const;

    void resetTotal_storage_capacity();

    double getUnfulfilled_demand() const;

    double getNet_stream_inflow() const;

    double getTotal_stored_volume() const;

    const InfrastructureManager &getInfrastructure_construction_manager() const;

    double getDemand_offset() const;

    double getAvailableVolumeToCapacityRatio() const;

    double getUnrestrictedDemandToTreatmentCapacityRatio() const;

    void setLongTermRisk_of_failures(double storage_risk_of_failure, double treatment_risk_of_failure);

    double getLong_term_storage_risk_of_failure() const;

    double getLong_term_treatment_risk_of_failure() const;

    void calculateDemandEstimateFromProjection(int year, bool reproject_demand);

    double getCurrent_year_demand_record() const;

    double getFuture_demand_estimate() const;

    void setFuture_demand_estimate(double demand_estimate);

    void setCurrent_year_demand_record(double current_demand);

    vector<double> calculateAnnualAverageWeeklyDemand(vector<double> *demands);

    void updateTreatmentCapacity(double capacity_adjustment);

    void setTreatmentCapacity(double total_capacity);

    double updateCurrent_present_value_debt_payment(int week);

    double getCurrent_debt_payment_present_valued() const;

    void setRisk_of_failures(double storage_risk_of_failure, double treatment_risk_of_failure);

    double getStorageRisk_of_failure() const;

    double getTreatmentRisk_of_failure() const;
};


#endif //TRIANGLEMODEL_UTILITY_H
