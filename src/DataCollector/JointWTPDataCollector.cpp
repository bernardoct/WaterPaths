//
// Created by dgorelic on 11/4/2019.
//

#include <sstream>
#include <iomanip>
#include "JointWTPDataCollector.h"

JointWTPDataCollector::JointWTPDataCollector(JointWTP *joint_wtp, unsigned long realization)
        : DataCollector(joint_wtp->id, joint_wtp->name, realization, joint_wtp->getAgreementType(), 8 * COLUMN_WIDTH),
        joint_wtp(joint_wtp), utilities_with_allocations(*joint_wtp->getUtilities_with_allocations()) {}

string JointWTPDataCollector::printTabularString(int week) {
    stringstream out_stream;

    for (int u : utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << partner_allocated_treatment_capacities[week][u];

    return out_stream.str();
}

string JointWTPDataCollector::printCompactString(int week) {
    stringstream out_stream;

    for (int u : utilities_with_allocations)
        out_stream << partner_allocated_treatment_capacities[week][u] << ",";

    return out_stream.str();
}

string JointWTPDataCollector::printTabularStringHeaderLine1() {
    stringstream out_stream;

    for (int u : utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Trmt Cap.";

    return out_stream.str();
}

string JointWTPDataCollector::printTabularStringHeaderLine2() {
    stringstream out_stream;

    for (int u : utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Alloc. " + to_string(u);

    return out_stream.str();
}

string JointWTPDataCollector::printCompactStringHeader() {
    stringstream out_stream;

    for (int u : utilities_with_allocations)
        out_stream << id << "trmt_alloc_" + to_string(u) << ",";

    return out_stream.str();
}

void JointWTPDataCollector::collect_data() {
    vector<double> alloc_trmt_cap_vector = joint_wtp->getAllocatedTreatmentCapacities();
    partner_allocated_treatment_capacities.push_back(alloc_trmt_cap_vector);
}
