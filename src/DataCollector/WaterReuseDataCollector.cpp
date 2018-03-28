//
// Created by bernardoct on 8/26/17.
//

#include <sstream>
#include <iomanip>
#include "WaterReuseDataCollector.h"

WaterReuseDataCollector::WaterReuseDataCollector(WaterReuse *water_reuse, unsigned long realization)
        : DataCollector(water_reuse->id, water_reuse->name, realization, WATER_REUSE, 2 * COLUMN_WIDTH),
          water_reuse(water_reuse) {}

string WaterReuseDataCollector::printTabularString(int week) {
    stringstream out_stream;

    out_stream << setw(2 * COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
               << water_reuse->getReused_volume();

    return out_stream.str();
}

string WaterReuseDataCollector::printCompactString(int week) {
    stringstream out_stream;

    out_stream << water_reuse->getReused_volume() << ",";

    return out_stream.str();
}

string WaterReuseDataCollector::printTabularStringHeaderLine1() {
    stringstream out_stream;

    out_stream
            << setw(2 * COLUMN_WIDTH) << "Reused";

    return out_stream.str();
}

string WaterReuseDataCollector::printTabularStringHeaderLine2() {
    stringstream out_stream;

    out_stream
            << setw(2 * COLUMN_WIDTH) << "Volume";

    return out_stream.str();
}

string WaterReuseDataCollector::printCompactStringHeader() {
    return to_string(id) + "demand,";
}

void WaterReuseDataCollector::collect_data() {

}
