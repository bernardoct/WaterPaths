//
// Created by bernardoct on 8/25/17.
//

#include <algorithm>
#include "TransfersDataCollector.h"

TransfersDataCollector::TransfersDataCollector(Transfers *transfer_policy)
        : DataCollector(transfer_policy->id,
                        nullptr,
                        TRANSFERS,
                        NON_INITIALIZED),
          utilities_ids(transfer_policy->getUtilities_ids()),
          transfer_policy(transfer_policy) {

    std::sort(utilities_ids.begin(),
              utilities_ids.end());
}

string TransfersDataCollector::printTabularString(int week) {

    stringstream outStream;

    for (float &a : demand_offsets.at((unsigned int) week))
        outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION) << a;

    return outStream.str();
}

string TransfersDataCollector::printCompactString(int week) {

    stringstream outStream;

    for (float &a : demand_offsets.at((unsigned int) week))
        outStream << a << ",";

    return outStream.str();
}

string TransfersDataCollector::printTabularStringHeaderLine1() {

    stringstream outStream;

    for (int id = 0; id < utilities_ids.size(); ++id)
        outStream << setw(COLUMN_WIDTH) << "Transf.";

    return outStream.str();
}

string TransfersDataCollector::printTabularStringHeaderLine2() {

    stringstream outStream;

    for (int buyer_id : utilities_ids)
        outStream << setw(COLUMN_WIDTH) << "Alloc. " + to_string(buyer_id);

    return outStream.str();
}

string TransfersDataCollector::printCompactStringHeader() {
    stringstream outStream;

    for (int &a : utilities_ids)
        outStream << a << "transf" << ",";

    return outStream.str();
}

void TransfersDataCollector::collect_data() {
    demand_offsets.push_back(transfer_policy->getAllocations());
}
