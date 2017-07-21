//
// Created by bernardoct on 7/9/17.
//

#ifndef TRIANGLEMODEL_ALLOCATEDRESERVOIR_H
#define TRIANGLEMODEL_ALLOCATEDRESERVOIR_H


#include "Reservoir.h"

class AllocatedReservoir : public Reservoir {
private:
    double *allocated_capacities;
    double *allocated_treatment_capacities;
    double *allocated_treatment_fractions;
    int wq_pool_id;
    double *allocated_fractions;
    vector<int> *utilities_with_allocations;
    double total_allocated_fraction;
    vector<bool> *online;

    void setAllocations(
            vector<int> *utilities_with_allocations,
            vector<double> *allocated_fractions,
            vector<double> *allocated_treatment_fractions);

public:
    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve,
            vector<int> *utilities_with_allocations,
            vector<double> *allocated_fractions, vector<double>
            *allocated_treatment_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series,
            DataSeries *storage_area_curve, const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate,
            vector<int> *utilities_with_allocations,
            vector<double> *allocated_fractions, vector<double>
            *allocated_treatment_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            vector<int> *utilities_with_allocations,
            vector<double> *allocated_fractions, vector<double>
            *allocated_treatment_fractions);

    AllocatedReservoir(
            const char *name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries *evaporation_series, double storage_area,
            const double construction_rof,
            const vector<double> &construction_time_range,
            double construction_cost, double bond_term,
            double bond_interest_rate,
            vector<int> *utilities_with_allocations,
            vector<double> *allocated_fractions, vector<double>
            *allocated_treatment_fractions);

    AllocatedReservoir &operator=(
            const AllocatedReservoir
            &allocated_reservoir);

    AllocatedReservoir(const AllocatedReservoir &allocated_reservoir);

    ~AllocatedReservoir();

    void applyContinuity(
            int week, double upstream_source_inflow,
            vector<double> *demand_outflow) override;

    double getAvailableAllocatedVolume(int utility_id) override;

    void removeWater(int allocation_id, double volume) override;

    void setFull() override;

    double getAllocatedCapacity(int utility_id) override;

    double getAllocatedFraction(int utility_id) override;

    void addCapacity(double capacity) override;

    void addTreatmentCapacity(
            const double added_plant_treatment_capacity,
            double
            allocated_fraction_of_total_capacity, int utility_id)
    override;

    double getAllocatedTreatmentCapacity(int utility_id) const override;


};


#endif //TRIANGLEMODEL_ALLOCATEDRESERVOIR_H
