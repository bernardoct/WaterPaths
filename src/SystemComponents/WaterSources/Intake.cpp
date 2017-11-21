//
// Created by bernardo on 2/10/17.
//

#include <numeric>
#include "../../Utils/Constants.h"
#include "Intake.h"

Intake::Intake(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double max_treatment_capacity)
        : WaterSource(name,
                      id,
                      catchments,
                      NONE,
                      max_treatment_capacity,
                      INTAKE) {

    /// Update total catchment inflow, demand, and available water volume for week 0;
//    upstream_catchment_inflow = 0;
//    for (Catchment *c : catchments) {
//        upstream_catchment_inflow += c->getStreamflow(0);
//    }
//
//    total_demand = 0;
//    stored_volume = upstream_catchment_inflow - min_environmental_outflow;
}

Intake::Intake(
        const char *name, const int id, const vector<Catchment *> &catchments,
        const double raw_water_main_capacity,
        const double construction_rof_or_demand,
        const vector<double> construction_time_range, double permitting_period,
        double construction_npv_cost_of_capital, double bond_term,
        double bond_interest_rate) :
        WaterSource(name, id, catchments, NONE, raw_water_main_capacity, INTAKE,
                    construction_time_range, permitting_period,
                    construction_npv_cost_of_capital, bond_term,
                    bond_interest_rate) {

//    /// Update total catchment inflow, total_demand, and available water volume for week 0;
//    this->upstream_catchment_inflow = 0;
//    for (Catchment *c : catchments) {
//        this->upstream_catchment_inflow = c->getStreamflow(0);
//    }
//
//    total_demand = 0;
//    stored_volume = this->upstream_catchment_inflow - min_environmental_outflow;
}

/**
 *
 * @param intake
 */
Intake::Intake(const Intake &intake) : WaterSource(intake) {}

/**
 *
 * @param intake
 * @return
 */
Intake &Intake::operator=(const Intake &intake) {
    WaterSource::operator=(intake);
    return *this;
}

/**
 * Destructor.
 */
Intake::~Intake() {
    catchments.clear();
}

//FIXME: WHEN IMPLEMENTING RESERVOIR RULES, ADD A "BANK" OF WATER OVER AND UNDERUSE TO BE ADDED/SUBTRACTED FROM THE AVAILABLE VOLUME FOR THE NEXT REALIZATION.
/**
 * Calculates the water volume that will be available in the next time step continuity for the
 * current one. This should work well for small intakes, in which case there
 * should almost always be enough water available to make full use of the
 * intake's capacity.
 *
 * @param week
 * @param upstream_source_inflow upstream sources current outflow.
 * @param demand current demand
 */
void Intake::applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_inflow, vector<double> &demand) {

    double total_upstream_inflow = upstream_source_inflow +
                                   wastewater_inflow;

    total_demand = 0;
    for (int i = 0; i < demand.size(); ++i) {
        total_demand += demand[i];
    }

    /// Get all upstream catchment inflow.
    upstream_catchment_inflow = 0;
    for (Catchment *c : catchments)
        upstream_catchment_inflow += c->getStreamflow(week);

    /// Water availability for next ime step.
    double next_upstream_catchment_inflow = 0;
    for (Catchment *c : catchments)
        next_upstream_catchment_inflow += c->getStreamflow(week + 1);

    /// The available volume for the following week will be next week's gain
    /// - this week's minimum environmental outflow (assuming next week's
    /// will be more or less the same and this week's) as long as the intake
    /// capacity is not exceeded. This should work well for small intakes.
    available_volume = min(total_treatment_capacity,
                           next_upstream_catchment_inflow -
                           min_environmental_outflow);

    /// Records for the sake of output.
    this->total_demand = total_demand + policy_added_demand;
    policy_added_demand = 0;
    total_outflow = total_upstream_inflow + upstream_catchment_inflow -
                    total_demand;
    this->upstream_source_inflow = upstream_source_inflow;
    this->wastewater_inflow = wastewater_inflow;
}

void Intake::setRealization(unsigned long r) {
    WaterSource::setRealization(r);

    total_demand = 0;
    available_volume = this->upstream_catchment_inflow -
                       min_environmental_outflow;
}
