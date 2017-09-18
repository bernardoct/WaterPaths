//
// Created by David on 9/18/2017.
//

#include <algorithm>
#include <iomanip>
#include "RawWaterReleaseDataCollector.h"

RawWaterReleaseDataCollector::RawWaterReleaseDataCollector(RawWaterReleases *raw_water_releases_policy)
        : DataCollector(raw_water_releases_policy->id,
                        nullptr,
                        RAW_WATER_TRANSFERS,
                        N_COLUMNS * COLUMN_WIDTH),
          utilities_ids(raw_water_releases_policy->getUtilities_ids()),
          raw_water_releases_policy(raw_water_releases_policy) {

    std::sort(utilities_ids.begin(),
              utilities_ids.end());
}

string RawWaterReleaseDataCollector::printTabularString(int week) {

    stringstream outStream;


    outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << raw_water_volume_released;

    for (double &a : storage_targets.at((unsigned int) week))
        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << a;

    return outStream.str();
}

string RawWaterReleaseDataCollector::printCompactString(int week) {

    stringstream outStream;

    outStream << raw_water_volume_released << ",";

    for (double &a : storage_targets.at((unsigned int) week))
        outStream << a << ",";

    return outStream.str();
}

string RawWaterReleaseDataCollector::printTabularStringHeaderLine1() {

    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << "RWT";

    for (int id = 0; id < utilities_ids.size(); ++id)
        outStream << setw(COLUMN_WIDTH) << "RWT";

    return outStream.str();
}

string RawWaterReleaseDataCollector::printTabularStringHeaderLine2() {

    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << "Vol. ";

    for (int buyer_id : utilities_ids)
        outStream << setw(COLUMN_WIDTH) << "Targ. Rat. " + to_string(buyer_id);

    return outStream.str();
}

string RawWaterReleaseDataCollector::printCompactStringHeader() {
    return nullptr;
}

void RawWaterReleaseDataCollector::collect_data() {
    raw_water_volume_released = raw_water_releases_policy->getRawWaterTransferVolume();
    storage_targets.push_back(raw_water_releases_policy->getUtilityTargetStorageLevels());
}