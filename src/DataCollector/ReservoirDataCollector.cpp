//
// Created by bernardoct on 8/25/17.
//

#include <sstream>
#include <iomanip>
#include "ReservoirDataCollector.h"

ReservoirDataCollector::ReservoirDataCollector(Reservoir *reservoir, unsigned long realization)
        : DataCollector(reservoir->id, reservoir->name, realization, RESERVOIR, 7 * COLUMN_WIDTH), reservoir(reservoir),
          fixed_area(reservoir->fixed_area), fixed_area_value(reservoir->getArea()) {
}

ReservoirDataCollector::ReservoirDataCollector(Reservoir *reservoir, int type, int table_width,
                                               unsigned long realization)
        : DataCollector(reservoir->id, reservoir->name, realization, type, table_width), reservoir(reservoir) {
}

string ReservoirDataCollector::printTabularString(int week) {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << stored_volume[week];

    if (fixed_area)
        outStream << setw(COLUMN_WIDTH) << area[week];

    outStream
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << demands[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << total_upstream_sources_inflows[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << wastewater_inflows[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << total_catchments_inflow[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << evaporated_volume[week]
            << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
            << outflows[week];

    return outStream.str();
}

string ReservoirDataCollector::printCompactString(int week) {
    stringstream outStream;

    outStream
            << stored_volume[week] << ",";

    if (fixed_area)
        outStream << fixed_area << ",";

    outStream
            << demands[week] << ","
            << total_upstream_sources_inflows[week] << ","
            << wastewater_inflows[week] << ","
            << total_catchments_inflow[week] << ","
            << evaporated_volume[week] << ","
            << outflows[week] << ",";

    return outStream.str();
}

string ReservoirDataCollector::printTabularStringHeaderLine1() {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << "Stored";

    if (reservoir->fixed_area)
        outStream << setw(COLUMN_WIDTH) << "Surface";

    outStream
            << setw(COLUMN_WIDTH) << " "
            << setw(COLUMN_WIDTH) << "Upstream"
            << setw(COLUMN_WIDTH) << "Wastewat."
            << setw(COLUMN_WIDTH) << "Catchment"
            << setw(COLUMN_WIDTH) << "Evap."
            << setw(COLUMN_WIDTH) << " ";

    return outStream.str();
}

string ReservoirDataCollector::printTabularStringHeaderLine2() {
    stringstream outStream;

    outStream
            << setw(2 * COLUMN_WIDTH) << "Volume";

    if (fixed_area)
        outStream << setw(COLUMN_WIDTH) << "Area";

    outStream
            << setw(COLUMN_WIDTH) << "Demands"
            << setw(COLUMN_WIDTH) << "Spillage"
            << setw(COLUMN_WIDTH) << "Inflows"
            << setw(COLUMN_WIDTH) << "Inflow"
            << setw(COLUMN_WIDTH) << "Volume"
            << setw(COLUMN_WIDTH) << "Spillage";

    return outStream.str();
}

string ReservoirDataCollector::printCompactStringHeader() {
    stringstream outStream;

    outStream
            << id << "volume" << ",";

    if (fixed_area)
        outStream << id << "s_area" << ",";

    outStream
            << id << "demand" << ","
            << id << "up_spill" << ","
            << id << "ww_inflow" << ","
            << id << "catch_inflow" << ","
            << id << "evap" << ","
            << id << "ds_spill" << ",";

    return outStream.str();
}

void ReservoirDataCollector::collect_data() {

    stored_volume.push_back(reservoir->getAvailableSupplyVolume());
    demands.push_back(reservoir->getDemand());
    total_upstream_sources_inflows
            .push_back(reservoir->getUpstream_source_inflow());
    outflows.push_back(reservoir->getTotal_outflow());
    total_catchments_inflow.push_back(reservoir->getUpstreamCatchmentInflow());
    evaporated_volume.push_back(reservoir->getEvaporated_volume());
    wastewater_inflows.push_back(reservoir->getWastewater_inflow());
    if (reservoir->fixed_area) area.push_back(reservoir->getArea());
}
