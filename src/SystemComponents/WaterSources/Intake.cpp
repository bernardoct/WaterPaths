//
// Created by bernardo on 2/10/17.
//

#include "../../Utils/Constants.h"
#include "Intake.h"

Intake::Intake(
        const char *name, const int id,
        const vector<Catchment *> &catchments,
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
//    demand = 0;
//    available_volume = upstream_catchment_inflow - min_environmental_outflow;
}

Intake::Intake(
        const char *name, const int id,
        const vector<Catchment *> &catchments,
        const double raw_water_main_capacity, const double construction_rof,
        const vector<double> construction_time_range,
        double construction_npv_cost_of_capital, double bond_term,
        double bond_interest_rate) :
        WaterSource(name,
                    id,
                    catchments,
                    NONE,
                    raw_water_main_capacity,
                    INTAKE,
                    construction_rof,
                    construction_time_range,
                    construction_npv_cost_of_capital,
                    bond_term,
                    bond_interest_rate) {

//    /// Update total catchment inflow, demand, and available water volume for week 0;
//    this->upstream_catchment_inflow = 0;
//    for (Catchment *c : catchments) {
//        this->upstream_catchment_inflow = c->getStreamflow(0);
//    }
//
//    demand = 0;
//    available_volume = this->upstream_catchment_inflow - min_environmental_outflow;
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
 * current one. The update of the total outflow for the current time step is performed with simple mass
 * balance: outflow equals the sum of the inflows. The water volume that will be for sure available for the next time
 * step is the catchment inflows of next week plus the upstream sources' minimum environmental inflow minus the intake's
 * own environmental inflow. The actually available volume may be greater if the reservoirs above are overflowing, but
 * since the spillover for the next time step hasn't been calculated yet, this value cannot be used. This doesn't matter
 * anyways because if the reservoir is overflowing -- there is no need to say that intake will have a greater water
 * availability to save water from the upstream reservoirs if they themselves are overflowing.
 *
 * The available volume depends on the inflows in the coming week which depend on the available volume of reservoirs
 * in the coming week which depends on the available volume of the intake. This circulatirity issue is minimized here
 * by allowing for under or overdraws but "charging" them in the following week.
 *
 * It may also look that the code assumes the intake operators know what the inflow will be in the following week. This
 * is not a problem because this information is actually used only in the following week, when they are measuring the inflow
 * of the stream where the intake is located.
 *
 * @param week
 * @param upstream_source_inflow upstream sources current outflow.
 * @param demand current demand
 */
void Intake::applyContinuity(
        int week, double upstream_source_inflow,
        double demand) {

    /// Get all upstream catchment inflow.
    upstream_catchment_inflow = 0;
    for (Catchment *c : catchments)
        upstream_catchment_inflow += c->getStreamflow(week);

    /// Mass balance for current time step.
    double total_outflow_approx = upstream_source_inflow +
                                  upstream_catchment_inflow - demand;

    /// Amount of water that could have been drawn but was let pass or that was over drawn.
    double over_under_use = total_outflow_approx - min_environmental_outflow;

    /// Take the over/underdrawn amount to be used in the next time step.
    total_outflow = min_environmental_outflow;

    /// Water availability for next ime step.
    double next_upstream_catchment_inflow = 0;
    for (Catchment *c : catchments)
        next_upstream_catchment_inflow += c->getStreamflow(week + 1);

    /// Estimated available volume for the coming week, corrected for over or underdraws in the current week.
    available_volume = min(raw_water_main_capacity + over_under_use,
                           upstream_min_env_inflow +
                           next_upstream_catchment_inflow -
            min_environmental_outflow + over_under_use);

    /// Records for the sake of output.
    this->demand = demand;
    this->upstream_source_inflow = upstream_source_inflow;
}

void Intake::setUpstream_min_env_flow(double upstream_min_env_flow) {
    this->upstream_min_env_inflow = upstream_min_env_flow;
}

void Intake::setRealization(unsigned long r) {
    WaterSource::setRealization(r);

    demand = 0;
    available_volume = this->upstream_catchment_inflow -
                       min_environmental_outflow;
}
