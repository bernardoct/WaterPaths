//
// Created by David on 9/18/2017.
//

#include <algorithm>
#include <iomanip>
#include "RawWaterReleaseDataCollector.h"

RawWaterReleaseDataCollector::RawWaterReleaseDataCollector(RawWaterReleases *raw_water_releases_policy,
                                                           unsigned long realization)
        : DataCollector(raw_water_releases_policy->id, nullptr, realization,
                        RAW_WATER_TRANSFERS, N_COLUMNS * COLUMN_WIDTH),
          utilities_ids(raw_water_releases_policy->getUtilities_ids()),
          raw_water_releases_policy(raw_water_releases_policy) {

}

string RawWaterReleaseDataCollector::printTabularString(int week) {

    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << raw_water_volume_released[week];

    for (double &a : utility_storage_targets.at((unsigned int)week))
        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << a;

    outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << request_denials[week];

    return outStream.str();
}

string RawWaterReleaseDataCollector::printCompactString(int week) {

    stringstream outStream;

    outStream << raw_water_volume_released[week] << ",";

    for (double &a : utility_storage_targets.at((unsigned int)week))
        outStream << a << ",";

    outStream << request_denials[week] << ",";

    return outStream.str();
}

string RawWaterReleaseDataCollector::printTabularStringHeaderLine1() {

    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << "RWT";

    for (int pid : utilities_ids)
        outStream << setw(COLUMN_WIDTH) << "RWT";

    outStream << setw(COLUMN_WIDTH) << "RWT";

    return outStream.str();
}

string RawWaterReleaseDataCollector::printTabularStringHeaderLine2() {

    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << "Vol. ";

//    outStream << setw(COLUMN_WIDTH) << "Targ. Rat. " + to_string(1);
//    outStream << setw(COLUMN_WIDTH) << "Targ. Rat. " + to_string(3);

    // FIXME: unclear why these statements are not working

    for (int pid : utilities_ids)
        outStream << setw(COLUMN_WIDTH) << "Targ. Rat. " + to_string(pid);

    outStream << setw(COLUMN_WIDTH) << "Req. Deny ";

    return outStream.str();
}

string RawWaterReleaseDataCollector::printCompactStringHeader() {
    stringstream outStream;

    outStream << id << "vol" << ","
              << id << "up_targ" << ","
              << id << "down_targ" << ","
              << id << "req_deny" << ",";

    return outStream.str();
}

void RawWaterReleaseDataCollector::collect_data() {

    raw_water_volume_released.push_back(raw_water_releases_policy->getRawWaterTransferVolume());
    utility_storage_targets.push_back(raw_water_releases_policy->getUtilityTargetStorageLevels());
    request_denials.push_back(raw_water_releases_policy->getRawWaterTransferDenials());
}