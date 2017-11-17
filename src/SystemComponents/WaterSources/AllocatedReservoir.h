//
// Created by bernardoct on 7/9/17.
//

#ifndef TRIANGLEMODEL_ALLOCATEDRESERVOIR_H
#define TRIANGLEMODEL_ALLOCATEDRESERVOIR_H


#include "Reservoir.h"

class AllocatedReservoir : public Reservoir {
public:
    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            vector<int> *utilities_with_allocations,
            vector<float> *allocated_fractions, vector<float>
            *allocated_treatment_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float construction_cost, float bond_term,
            float bond_interest_rate,
            vector<int> *utilities_with_allocations,
            vector<float> *allocated_fractions, vector<float>
            *allocated_treatment_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            vector<int> *utilities_with_allocations,
            vector<float> *allocated_fractions, vector<float>
            *allocated_treatment_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const float capacity,
            const float max_treatment_capacity,
            EvaporationSeries *evaporation_series, float storage_area,
            const float construction_rof_or_demand,
            const vector<float> &construction_time_range,
            float construction_cost, float bond_term,
            float bond_interest_rate,
            vector<int> *utilities_with_allocations,
            vector<float> *allocated_fractions, vector<float>
            *allocated_treatment_fractions);

    AllocatedReservoir &operator=(
            const AllocatedReservoir
            &allocated_reservoir);

    AllocatedReservoir(const AllocatedReservoir &allocated_reservoir);

    ~AllocatedReservoir();

    void applyContinuity(int week, float upstream_source_inflow,
                             float wastewater_inflow,
                             vector<float> &demand_outflow) override;

    void setFull() override;

    float getAvailableAllocatedVolume(int utility_id) override;

    void removeWater(int allocation_id, float volume) override;

    float getAllocatedCapacity(int utility_id) override;

    float getAllocatedFraction(int utility_id) override;

    void addCapacity(float capacity) override;

    void addTreatmentCapacity(
            const float added_plant_treatment_capacity,
            float allocated_fraction_of_total_capacity, int utility_id)
    override;

    float getAllocatedTreatmentCapacity(int utility_id) const override;


};


#endif //TRIANGLEMODEL_ALLOCATEDRESERVOIR_H
