//
// Created by David on 4/9/2018.
//

#ifndef TRIANGLEMODEL_JOINTWTP_H
#define TRIANGLEMODEL_JOINTWTP_H


#include "Base/WaterSource.h"

class JointWTP : public WaterSource {
private:

    int build_year = 0;
    unsigned long EXTERNAL_SOURCE_ID = -1;
    unsigned long PROJECTED_YEARS = -1;
    unsigned long N_UTILITIES_INCLUDING_EXTERNAL = -1;
    unsigned long N_INTERNAL_UTILITIES = -1;

    double external_allocation;
    int contract_type;

    int observed_demand_weeks_to_use = 0;

    bool initial_capacity_added = false;

    vector<int> third_party_ids;
    vector<double> third_party_sales_rates;

    vector<double> external_demands;
    vector<vector<double>> annual_demands;
    vector<double> utility_peaking_factors;
    vector<double> previous_period_allocated_capacities;

    vector<vector<double>> utility_allocation_fractions;

    double current_external_allocation_fraction = 0.0;

public:

    const unsigned int parent_reservoir_ID;

    /// FOR CONTRACT TYPE 0
    JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
             vector<int> *utils_with_allocations,
             vector<int> participating_utilities, const double added_treatment_capacity, vector<Bond *> &bonds,
             vector<double> &fixed_allocations, double fixed_external_allocation,
             const vector<double> &construction_time_range, double permitting_period);

    /// FOR CONTRACT TYPES 1 AND 2
    /// Type 1
    JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
             vector<int> *utils_with_allocations,
             vector<int> participating_utilities, const double added_treatment_capacity, vector<Bond *> &bonds,
             const vector<double> &construction_time_range, double permitting_period,
             vector<double> demand_peaking_factor, vector<vector<double>> future_annual_utility_demands,
             vector<double> external_annual_demands);

    /// Type 2
    JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
             vector<int> *utils_with_allocations,
             vector<int> participating_utilities, const double added_treatment_capacity, vector<Bond *> &bonds,
             const vector<double> &construction_time_range, double permitting_period,
             vector<double> demand_peaking_factor, vector<vector<double>> future_annual_utility_demands,
             vector<double> external_annual_demands, const int past_demand_weeks_to_use);

    /// FOR CONTRACT TYPE 3 with type 0 base (type 3)
    JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
             vector<int> *utils_with_allocations,
             vector<int> participating_utilities, const double added_treatment_capacity, vector<Bond *> &bonds,
             const vector<double> &construction_time_range, double permitting_period,
             vector<double> demand_peaking_factor, vector<vector<double>> future_annual_utility_demands,
             vector<double> external_annual_demands, vector<int> third_party_utilities,
             vector<double> third_party_sales_volumetric_rate);

    /// FOR CONTRACT TYPE 3 with type 1 or 2 base (type 4)
    JointWTP(const char *name, const int id, const int contract_type, const int parent_reservoir_ID,
             vector<int> *utils_with_allocations,
             vector<int> participating_utilities, const double added_treatment_capacity, vector<Bond *> &bonds,
             vector<double> &fixed_allocations, double fixed_external_allocation,
             const vector<double> &construction_time_range, double permitting_period, vector<int> third_party_utilities,
             vector<double> third_party_sales_volumetric_rate);

    JointWTP(const JointWTP &joint_water_treatment_plant);

    JointWTP &operator=(const JointWTP &joint_water_treatment_plant);

    ~JointWTP() override;

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    int getContractType();

    double implementFixedTreatmentCapacity(int utility_id);

    double getAdjustableTreatmentCapacity(int utility_id, int year);

    void updateTreatmentAllocations(int week, vector<vector<vector<double>>> weekly_demands,
                                    vector<vector<vector<double>>> weekly_supply_based_demands) override;

    double getAddedTotalTreatmentCapacity();

    void recordAllocationAdjustment(double added_treatment_capacity, int utility_id);

    void setBuildYear(int year);

    double calculateJointWTPDebtServiceFraction(int utility_id, int week) override;

    int getParentSourceID() override;

    double getAllocatedTreatmentCapacity(int utility_id) const override;

    double getAllocationAdjustment(const int utility_id) override;

    void setAllocationAdjustment(const int utility_id, const double allocation_adjustment) override;

    double getAllocatedTreatmentFraction(int utility_id) const override;

    void setProjectedAllocationFractions(vector<vector<double>> &utility_allocation_fractions,
                                         vector<vector<double>> &annual_demands,
                                         vector<double> &external_demands,
                                         vector<double> &utility_peaking_factors);

    void setAllocatedTreatmentCapacity(const int year, const int utility_id) override;

    void recordParentReservoirTreatmentCapacity(const int utility_id, const double capacity_value) override;

    double getExternalDemand(const int year);

    double getExternalTreatmentAllocation();

    void setExternalAllocatedTreatmentCapacity(const int year) override;

    vector<double> getPreviousPeriodAllocatedCapacities() const override;
};


#endif //TRIANGLEMODEL_JOINTWTP_H
