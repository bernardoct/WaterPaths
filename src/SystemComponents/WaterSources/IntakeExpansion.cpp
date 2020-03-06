//
// Created by dgorelic on 7/8/2019.
//

#include "IntakeExpansion.h"

IntakeExpansion::IntakeExpansion(const char *name, const int id, const unsigned int parent_intake_ID,
                                 const double capacity_added, const double treatment_capacity_added,
                                 const vector<double> &construction_time_range, vector<int> connected_sources,
                                 double permitting_period, Bond &bond)
        : WaterSource(name, id, vector<Catchment *>(), capacity_added, treatment_capacity_added, connected_sources, INTAKE_EXPANSION,
                      construction_time_range, permitting_period, bond),
          parent_intake_ID(parent_intake_ID) {}

/**
* Copy constructor.
* @param intake
*/
IntakeExpansion::IntakeExpansion(const IntakeExpansion &intake_expansion) :
        WaterSource(intake_expansion),
        parent_intake_ID(intake_expansion.parent_intake_ID) {}

/**
* Copy assignment operator
* @param intake
* @return
*/
IntakeExpansion &IntakeExpansion::operator=(const IntakeExpansion &intake_expansion) {
    WaterSource::operator=(intake_expansion);
    return *this;
}

void IntakeExpansion::applyContinuity(int week, double upstream_source_inflow, double wastewater_discharge,
                                      vector<double> &demand_outflow) {

}
