//
// Created by bernardo on 2/10/17.
//

#include "../../Utils/Constants.h"
#include "Intake.h"

Intake::Intake(const string &source_name, const int id, const double min_environmental_outflow,
               const vector<Catchment *> &catchments, bool online)
        : WaterSource(source_name,
                      id,
                      min_environmental_outflow,
                      catchments,
                      online,
                      NONE,
                      INTAKE) {
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

void Intake::updateAvailableVolume(int week, double upstream_source_inflow, double demand_outflow) {

    upstream_catchment_inflow = 0;
    for (Catchment *c : catchments) {
        upstream_catchment_inflow += c->getStreamflow(week);
    }

    demand_previous_week = demand_outflow;
    available_volume = upstream_source_inflow + upstream_catchment_inflow - min_environmental_outflow;
    this->upstream_source_inflow = upstream_source_inflow;
    total_outflow = available_volume - demand_outflow + min_environmental_outflow;
}
