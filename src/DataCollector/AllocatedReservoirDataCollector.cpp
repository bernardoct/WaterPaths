//
// Created by bernardoct on 8/26/17.
//

#include <sstream>
#include <iomanip>
#include <algorithm>
#include "AllocatedReservoirDataCollector.h"

AllocatedReservoirDataCollector::AllocatedReservoirDataCollector(
            AllocatedReservoir *allocated_reservoir, unsigned long realization)
        : ReservoirDataCollector(allocated_reservoir, ALLOCATED_RESERVOIR,
                                 (7 + (int) allocated_reservoir->
                                                 getUtilities_with_allocations()->
                                                 size()) * COLUMN_WIDTH,
                                 realization),
          allocated_reservoir(allocated_reservoir) {}

string AllocatedReservoirDataCollector::printTabularString(int week) {
    string output = ReservoirDataCollector::printTabularString(week);

    stringstream out_stream;

    out_stream << output;

    for (double v : allocated_stored_volumes[week])
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << v;

    return out_stream.str();
}

string AllocatedReservoirDataCollector::printCompactString(int week) {
    string output = ReservoirDataCollector::printCompactString(week);

    stringstream out_stream;

    out_stream << output;

    for (double v : allocated_stored_volumes[week])
        out_stream << v << ",";

    return out_stream.str();
}

string AllocatedReservoirDataCollector::printTabularStringHeaderLine1() {
    string output = ReservoirDataCollector::printTabularStringHeaderLine1();

    stringstream out_stream;

    out_stream << output;

    for (int u : *(allocated_reservoir->getUtilities_with_allocations()))
        out_stream << setw(COLUMN_WIDTH) << "Stored V.";

    return out_stream.str();
}

string AllocatedReservoirDataCollector::printTabularStringHeaderLine2() {
    string output = ReservoirDataCollector::printTabularStringHeaderLine2();

    stringstream out_stream;

    out_stream << output;

    for (int u : *(allocated_reservoir->getUtilities_with_allocations()))
        out_stream << setw(COLUMN_WIDTH) << "Alloc. " + to_string(u);

    return out_stream.str();
}

string AllocatedReservoirDataCollector::printCompactStringHeader() {

    stringstream out_stream;

    for (int u : *(allocated_reservoir->getUtilities_with_allocations()))
        out_stream << id << "alloc_" + to_string(u) << ",";

    return ReservoirDataCollector::printCompactStringHeader() + out_stream.str();
}

void AllocatedReservoirDataCollector::collect_data() {
    ReservoirDataCollector::collect_data();
    vector<double> alloc_vol_vector = vector<double>
            (allocated_reservoir->getAvailable_allocated_volumes(),
             allocated_reservoir->getAvailable_allocated_volumes() +
             allocated_reservoir->getUtilities_with_allocations()->size());
    allocated_stored_volumes.push_back(alloc_vol_vector);
}
