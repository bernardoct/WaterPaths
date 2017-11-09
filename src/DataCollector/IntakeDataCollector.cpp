//
// Created by bernardoct on 8/26/17.
//

#include <iomanip>
#include "IntakeDataCollector.h"

IntakeDataCollector::IntakeDataCollector(Intake *intake)
        : DataCollector(intake->id,
                        intake->name,
                        INTAKE,
                        5 * COLUMN_WIDTH), intake(intake) {}

string IntakeDataCollector::printTabularString(int week) {
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

    return outStream.str();
}

string IntakeDataCollector::printCompactString(int week) {
    stringstream outStream;

    outStream
            << demands[week] << ","
            << total_upstream_sources_inflows[week] << ","
            << wastewater_inflows[week] << ","
            << total_catchments_inflow[week] << ","
            << outflows[week] << ",";

    return outStream.str();
}

string IntakeDataCollector::printTabularStringHeaderLine1() {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << " "
            << setw(COLUMN_WIDTH) << "Upstream"
            << setw(COLUMN_WIDTH) << "Wastewat."
            << setw(COLUMN_WIDTH) << "Catchment"
            << setw(COLUMN_WIDTH) << " ";

    return outStream.str();
}

string IntakeDataCollector::printTabularStringHeaderLine2() {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << "Demands"
            << setw(COLUMN_WIDTH) << "Spillage"
            << setw(COLUMN_WIDTH) << "Inflow"
            << setw(COLUMN_WIDTH) << "Inflow"
            << setw(COLUMN_WIDTH) << "Outflow";

    return outStream.str();
}

string IntakeDataCollector::printCompactStringHeader() {

    stringstream outStream;

    outStream
            << id << "demand" << ","
            << id << "up_spill" << ","
            << id << "ww_inflow" << ","
            << id << "catch_inflow" << ","
            << id << "ds_spill" << ",";

    return outStream.str();
}

void IntakeDataCollector::collect_data() {

    demands.push_back(intake->getDemand());
    total_upstream_sources_inflows
            .push_back(intake->getUpstream_source_inflow());
    wastewater_inflows.push_back(intake->getWastewater_inflow());
    outflows.push_back(intake->getTotal_outflow());
    total_catchments_inflow.push_back(intake->getUpstreamCatchmentInflow());
}

