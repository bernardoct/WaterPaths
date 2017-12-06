//
// Created by bernardo on 1/22/17.
//

#ifndef TRIANGLEMODEL_WATERSOURCE_H
#define TRIANGLEMODEL_WATERSOURCE_H

#include <string>
#include "../../Catchment.h"
#include "../../../Utils/Constants.h"

using namespace std;
using namespace Constants;
const int BOND_INTEREST_PAYMENTS_PER_YEAR = 1;

class WaterSource {
protected:

    double available_volume = 0;
    double total_outflow = 0;
    double upstream_source_inflow = 0;
    double wastewater_inflow = 0;
    double upstream_catchment_inflow = 0;
    double total_demand = 0;
    double policy_added_demand = 0;
    double permitting_time;
    double construction_cost_of_capital;
    double upstream_min_env_inflow;
    double capacity;
    double *available_allocated_volumes = nullptr;
    double *allocated_capacities = nullptr;
    double *allocated_treatment_capacities = nullptr;
    double *allocated_treatment_fractions = nullptr;
    double *allocated_fractions = nullptr;
    vector<int> *utilities_with_allocations = nullptr;
    int wq_pool_id = NON_INITIALIZED;
    double total_allocated_fraction = NON_INITIALIZED;
    bool online;
    vector<Catchment *> catchments;
    double min_environmental_outflow;
    double evaporated_volume = 0;
    double total_treatment_capacity;
    int highest_alloc_id = NON_INITIALIZED;

    virtual void applyContinuity(int week, double upstream_source_inflow,
                                     double wastewater_inflow,
                                     vector<double> &demand_outflow) = 0;

    void bypass(int week, double total_upstream_inflow);

public:
    const int id;
    const char *name;
    const int source_type;
    const double construction_time;

    WaterSource(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            double treatment_capacity, const int source_type);

    WaterSource(const char *source_name, const int id, const vector<Catchment *> &catchments,
                    const double capacity, double treatment_capacity, const int source_type,
                    const vector<double> construction_time_range, double permitting_period,
                    double construction_cost_of_capital);

    WaterSource(const char *name, const int id, const vector<Catchment *> &catchments,
                    const double capacity, double treatment_capacity, const int source_type,
                    vector<double> *allocated_treatment_fractions, vector<double> *allocated_fractions,
                    vector<int> *utilities_with_allocations, const vector<double> construction_time_range,
                    double permitting_period, double construction_cost_of_capital);

    WaterSource(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            double treatment_capacity, const int source_type,
            vector<double> *allocated_treatment_fractions,
            vector<double> *allocated_fractions,
            vector<int> *utilities_with_allocations);

    WaterSource(const WaterSource &water_source);

    virtual ~WaterSource();

    WaterSource &operator=(const WaterSource &water_source);

    bool operator<(const WaterSource *other);

    bool operator>(const WaterSource *other);

    bool operator==(const WaterSource *other);

    void continuityWaterSource(int week, double upstream_source_inflow,
                                   double wastewater_inflow,
                                   vector<double> &demand_outflow);

    virtual void addTreatmentCapacity(
            const double added_treatment_capacity,
            double allocations_added_treatment_capacity,
            int utility_id);

    virtual double calculateNetPresentConstructionCost(int week, int utility_id, double discount_rate,
                                                           double *level_debt_service_payment, double bond_term,
                                                           double bond_interest_rate) const;

    virtual void removeWater(int allocation_id, double volume);

    virtual void addCapacity(double capacity);

    virtual void setOnline();

    virtual double getAvailableAllocatedVolume(int utility_id);

    double getMin_environmental_outflow() const;

    void setMin_environmental_outflow(double min_environmental_outflow);

    double getAvailable_volume() const;

    double getTotal_outflow() const;

    bool isOnline() const;

    virtual void setFull();

    void setOutflow_previous_week(double outflow_previous_week);

    virtual double getCapacity();

    double getUpstream_source_inflow() const;

    double getDemand() const;

    double getUpstreamCatchmentInflow() const;

    static bool compare(WaterSource *lhs, WaterSource *rhs);

    virtual void setRealization(unsigned long r, vector<vector<double>> *rdm_factors);

    virtual double getAllocatedCapacity(int utility_id);

    virtual double getAllocatedFraction(int utility_id);

    double getEvaporated_volume() const;

    virtual double getAllocatedTreatmentCapacity(int utility_id) const;

    double getTotal_treatment_capacity(int utility_id) const;

    void setAllocations(
            vector<int> *utilities_with_allocations,
            vector<double> *allocated_fractions,
            vector<double> *allocated_treatment_fractions);

    void resetAllocations(const vector<double> *new_allocated_fractions);

    void setAvailableAllocatedVolumes(
            double *available_allocated_volumes, double available_volume);

    double *getAvailable_allocated_volumes() const;

    vector<int> *getUtilities_with_allocations() const;

    double getWastewater_inflow() const;

    double getPermitting_period() const;

    void setPermitting_period(double permitting_period);

    double getConstruction_cost_of_capital() const;

    void setConstruction_cost_of_capital(double construction_cost_of_capital);


    virtual void addWater(int allocation_id, double volume);
};


#endif //TRIANGLEMODEL_WATERSOURCE_H
