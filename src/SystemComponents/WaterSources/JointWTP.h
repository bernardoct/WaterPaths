//
// Created by David on 4/9/2018.
//

#ifndef TRIANGLEMODEL_JOINTWTP_H
#define TRIANGLEMODEL_JOINTWTP_H


#include "Base/WaterSource.h"

class JointWTP : public WaterSource {
private:

    double total_added_capacity;
    double total_capital_cost;
    double external_allocation;
    int contract_type;

    bool initial_capacity_added = false;

    vector<int> *third_party_ids;
    vector<double> *third_party_sales_rates;

    vector<double> *fixed_utility_allocations;

    vector<double> *external_demands;
    vector<vector<double>> *annual_demands;
    vector<double> *utility_peaking_factors;
    vector<double> *previous_year_allocated_capacities = nullptr;

    vector<vector<double>> *utility_allocation_fractions = nullptr;
public:

    const unsigned int parent_reservoir_ID;

    /// FOR CONTRACT TYPE 0
    JointWTP(const char *name, const int id,
             const int contract_type,
                // 0: fixed capacity, 1: uniform rate, 2: adjustible allocations, 3: add third-party contracts
             const int parent_reservoir_ID,
             vector<int> participating_utilities,
                // assumes parent reservoir already has allocations for these utilities
             const double total_treatment_capacity,
             const double total_capital_cost,
             vector <double> fixed_allocations,
             double fixed_external_allocation,
             const vector<double> &construction_time_range,
             double permitting_period);

    /// FOR CONTRACT TYPES 1 AND 2
    JointWTP(const char *name, const int id,
             const int contract_type,
                // 0: fixed capacity, 1: uniform rate, 2: adjustible allocations, 3: add third-party contracts
             const int parent_reservoir_ID,
             vector<int> participating_utilities,
                // assumes parent reservoir already has allocations for these utilities
             const double total_treatment_capacity,
             const double total_capital_cost,
             const vector<double> &construction_time_range,
             double permitting_period,
             vector<double> demand_peaking_factor,
                // indices for each utility attached to the source, and an extra for external reservations
             vector<vector<double>> future_annual_utility_demands,
             vector<double> external_annual_demands);

    /// FOR CONTRACT TYPE 3 with type 0 base (type 3)
    JointWTP(const char *name, const int id,
             const int contract_type,
            // 0: fixed capacity, 1: uniform rate, 2: adjustible allocations,
            // 3: add third-party contracts, fixed allocs, 4: 3rd party contracts, adjustible allocs
             const int parent_reservoir_ID,
             vector<int> participating_utilities,
            // assumes parent reservoir already has allocations for these utilities
             const double total_treatment_capacity,
             const double total_capital_cost,
             vector <double> fixed_allocations,
             double fixed_external_allocation,
             const vector<double> &construction_time_range,
             double permitting_period,
             vector<int> third_party_utilities,
             vector<double> third_party_sales_volumetric_rate);

    /// FOR CONTRACT TYPE 3 with type 1 or 2 base (type 4)
    JointWTP(const char *name, const int id,
             const int contract_type,
            // 0: fixed capacity, 1: uniform rate, 2: adjustible allocations,
            // 3: add third-party contracts, fixed allocs, 4: 3rd party contracts, adjustible allocs
             const int parent_reservoir_ID,
             vector<int> participating_utilities,
                // assumes parent reservoir already has allocations for these utilities
             const double total_treatment_capacity,
             const double total_capital_cost,
             const vector<double> &construction_time_range,
             double permitting_period,
             vector<double> demand_peaking_factor,
             vector<vector<double>> future_annual_utility_demands,
             vector<double> external_annual_demands,
             vector<int> third_party_utilities,
             vector<double> third_party_sales_volumetric_rate);

    JointWTP(const JointWTP &joint_water_treatment_plant);

    JointWTP &operator=(const JointWTP &joint_water_treatment_plant);

    ~JointWTP() override;

    void applyContinuity(int week, double upstream_source_inflow,
                         double wastewater_discharge,
                         vector<double> &demand_outflow) override;

    int getContractType();

    vector<double> calculateAdjustableAllocationConstructionCosts(int utility_id, double bond_length);

    double calculateFixedAllocationCosts(int utility_id);

    double calculateNetPresentConstructionCost(
            int week, int utility_id, double discount_rate,
            vector<double>& debt_service_payments, double bond_term,
            double bond_interest_rate) const override;

    double implementFixedTreatmentCapacity(int utility_id);

    double getAdjustableTreatmentCapacity(int utility_id, int year);

    void updateTreatmentAllocations(int week) override;

    double getAddedTotalTreatmentCapacity();

    void recordAllocationAdjustment(double added_treatment_capacity, int utility_id);

};


#endif //TRIANGLEMODEL_JOINTWTP_H
