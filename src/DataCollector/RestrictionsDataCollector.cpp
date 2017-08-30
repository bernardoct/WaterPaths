//
// Created by bernardoct on 8/25/17.
//

#include <iomanip>
#include "RestrictionsDataCollector.h"

RestrictionsDataCollector::RestrictionsDataCollector(
        Restrictions *restriction_policy)
        : DataCollector(restriction_policy->id,
                        nullptr,
                        RESTRICTIONS,
                        NON_INITIALIZED),
          restriction_policy(restriction_policy) {}

string RestrictionsDataCollector::printTabularString(int week) {
    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
              << restriction_multipliers.at((unsigned long) week);

    return outStream.str();
}

string RestrictionsDataCollector::printCompactString(int week) {

    return to_string(restriction_multipliers.at((unsigned long) week)) + ",";
}

string RestrictionsDataCollector::printTabularStringHeaderLine1() {
    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << "Restr. " + to_string(id);
    return outStream.str();
}

string RestrictionsDataCollector::printTabularStringHeaderLine2() {
    stringstream outStream;

    outStream << setw(COLUMN_WIDTH) << "Multip.";
    return outStream.str();
}

string RestrictionsDataCollector::printCompactStringHeader() {
    return nullptr;
}

void RestrictionsDataCollector::collect_data() {
    restriction_multipliers.push_back
            (restriction_policy->getCurrent_multiplier());
}

const vector<double> &
RestrictionsDataCollector::getRestriction_multipliers() const {
    return restriction_multipliers;
}
