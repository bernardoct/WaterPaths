//
// Created by dgorelic on 3/2/2020.
//

#include <sstream>
#include <iomanip>
#include "AllocatedIntakeDataCollector.h"

AllocatedIntakeDataCollector::AllocatedIntakeDataCollector(AllocatedIntake *intake, unsigned long realization)
        : DataCollector(intake->id, intake->name, realization, ALLOCATED_INTAKE, 15 * COLUMN_WIDTH),
          intake(intake),
          utilities_with_allocations(*intake->getUtilities_with_allocations()) {}

string AllocatedIntakeDataCollector::printTabularString(int week) {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << demands[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << total_upstream_sources_inflows[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << wastewater_inflows[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << total_catchments_inflow[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << outflows[week];

    for (int u : utilities_with_allocations)
        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                  << allocated_supply_capacity[week][u];
    for (int u : utilities_with_allocations)
        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                  << allocated_demands[week][u];

    return outStream.str();
}

string AllocatedIntakeDataCollector::printCompactString(int week) {
    stringstream outStream;

    outStream
            << demands[week] << ","
            << total_upstream_sources_inflows[week] << ","
            << wastewater_inflows[week] << ","
            << total_catchments_inflow[week] << ","
            << outflows[week] << ",";

    for (int u : utilities_with_allocations)
        outStream << allocated_supply_capacity[week][u] << ",";
    for (int u : utilities_with_allocations)
        outStream << allocated_demands[week][u] << ",";

    return outStream.str();
}

string AllocatedIntakeDataCollector::printTabularStringHeaderLine1() {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << " "
            << setw(COLUMN_WIDTH) << "Upstream"
            << setw(COLUMN_WIDTH) << "Wastewat."
            << setw(COLUMN_WIDTH) << "Catchment"
            << setw(COLUMN_WIDTH) << " ";

    for (int u : utilities_with_allocations)
        outStream << u << ",";
    for (int u : utilities_with_allocations)
        outStream << u << ",";

    return outStream.str();
}

string AllocatedIntakeDataCollector::printTabularStringHeaderLine2() {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << "Demands"
            << setw(COLUMN_WIDTH) << "Spillage"
            << setw(COLUMN_WIDTH) << "Inflow"
            << setw(COLUMN_WIDTH) << "Inflow"
            << setw(COLUMN_WIDTH) << "Outflow";

    for (int u : utilities_with_allocations)
        outStream << "AllocCapacity" << ",";
    for (int u : utilities_with_allocations)
        outStream << "AllocDemand" << ",";

    return outStream.str();
}

string AllocatedIntakeDataCollector::printCompactStringHeader() {
    stringstream outStream;

    outStream
            << id << "demand" << ","
            << id << "up_spill" << ","
            << id << "ww_inflow" << ","
            << id << "catch_inflow" << ","
            << id << "ds_spill" << ",";

    for (int u : utilities_with_allocations)
        outStream << id << "_" + to_string(u) + "_alloc_cap" << ",";
    for (int u : utilities_with_allocations)
        outStream << id << "_" + to_string(u) + "_alloc_dem" << ",";

    return outStream.str();
}

void AllocatedIntakeDataCollector::collect_data() {
    demands.push_back(intake->getDemand());
    total_upstream_sources_inflows
            .push_back(intake->getUpstream_source_inflow());
    wastewater_inflows.push_back(intake->getWastewater_inflow());
    outflows.push_back(intake->getTotal_outflow());
    total_catchments_inflow.push_back(intake->getUpstreamCatchmentInflow());
    allocated_supply_capacity.push_back(intake->getAllocatedSupplyCapacities());
    allocated_demands.push_back(intake->getAllocatedDemands());
}
