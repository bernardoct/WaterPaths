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

    float available_volume = 0;
    float total_outflow = 0;
    float upstream_source_inflow = 0;
    float wastewater_inflow = 0;
    float upstream_catchment_inflow = 0;
    float total_demand = 0;
    float policy_added_demand = 0;
    float upstream_min_env_inflow;
    float capacity;
    float *available_allocated_volumes = nullptr;
    float *allocated_capacities = nullptr;
    float *allocated_treatment_capacities = nullptr;
    float *allocated_treatment_fractions = nullptr;
    float *allocated_fractions = nullptr;
    vector<int> *utilities_with_allocations = nullptr;
    int wq_pool_id = NON_INITIALIZED;
    float total_allocated_fraction = NON_INITIALIZED;
    bool online;
    vector<Catchment *> catchments;
    float min_environmental_outflow;
    float evaporated_volume = 0;
    float total_treatment_capacity;
    int highest_alloc_id = NON_INITIALIZED;

    virtual void applyContinuity(int week, float upstream_source_inflow,
                                     float wastewater_inflow,
                                     vector<float> &demand_outflow) = 0;

    void bypass(int week, float total_upstream_inflow);

public:
    const int id;
    const char *name;
    const int source_type;
    const float construction_cost_of_capital;
    const float construction_time;
    const float permitting_period;
    const float bond_term;
    const float bond_interest_rate;

    WaterSource(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            float treatment_capacity, const int source_type);

    WaterSource(const char *source_name, const int id,
                    const vector<Catchment *> &catchments, const float capacity,
                    float treatment_capacity, const int source_type,
                    const vector<float> construction_time_range,
                    float permitting_period, float construction_cost_of_capital,
                    float bond_term, float bond_interest_rate);

    WaterSource(const char *name, const int id,
                    const vector<Catchment *> &catchments, const float capacity,
                    float treatment_capacity, const int source_type,
                    vector<float> *allocated_treatment_fractions,
                    vector<float> *allocated_fractions,
                    vector<int> *utilities_with_allocations,
                    const vector<float> construction_time_range,
                    float permitting_period, float construction_cost_of_capital,
                    float bond_term, float bond_interest_rate);

    WaterSource(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            float treatment_capacity, const int source_type,
            vector<float> *allocated_treatment_fractions,
            vector<float> *allocated_fractions,
            vector<int> *utilities_with_allocations);

    WaterSource(const WaterSource &water_source);

    virtual ~WaterSource();

    WaterSource &operator=(const WaterSource &water_source);

    bool operator<(const WaterSource *other);

    bool operator>(const WaterSource *other);

    bool operator==(const WaterSource *other);

    void continuityWaterSource(int week, float upstream_source_inflow,
                                   float wastewater_inflow,
                                   vector<float> &demand_outflow);

    virtual void addTreatmentCapacity(
            const float added_treatment_capacity,
            float allocations_added_treatment_capacity,
            int utility_id);

    virtual float calculateNetPresentConstructionCost(
            int week, int utility_id, float discount_rate,
            float *level_debt_service_payment) const;

    virtual void removeWater(int allocation_id, float volume);

    virtual void addCapacity(float capacity);

    virtual void setOnline();

    virtual float getAvailableAllocatedVolume(int utility_id);

    float getMin_environmental_outflow() const;

    void setMin_environmental_outflow(float min_environmental_outflow);

    float getAvailable_volume() const;

    float getTotal_outflow() const;

    bool isOnline() const;

    virtual void setFull();

    void setOutflow_previous_week(float outflow_previous_week);

    virtual float getCapacity();

    float getUpstream_source_inflow() const;

    float getDemand() const;

    float getUpstreamCatchmentInflow() const;

    static bool compare(WaterSource *lhs, WaterSource *rhs);

    virtual void setRealization(unsigned long r);

    virtual float getAllocatedCapacity(int utility_id);

    virtual float getAllocatedFraction(int utility_id);

    float getEvaporated_volume() const;

    virtual float getAllocatedTreatmentCapacity(int utility_id) const;

    float getTotal_treatment_capacity(int utility_id) const;

    void setAllocations(
            vector<int> *utilities_with_allocations,
            vector<float> *allocated_fractions,
            vector<float> *allocated_treatment_fractions);

    void resetAllocations(const vector<float> *new_allocated_fractions);

    void setAvailableAllocatedVolumes(
            float *available_allocated_volumes, float available_volume);

    float *getAvailable_allocated_volumes() const;

    vector<int> *getUtilities_with_allocations() const;

    float getWastewater_inflow() const;
};


#endif //TRIANGLEMODEL_WATERSOURCE_H
