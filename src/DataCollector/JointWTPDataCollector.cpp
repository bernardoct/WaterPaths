//
// Created by David on 5/6/2018.
//

#include <sstream>
#include <iomanip>
#include "JointWTPDataCollector.h"

JointWTPDataCollector::JointWTPDataCollector(JointWTP *wtp, AllocatedReservoir *allocated_reservoir,
                                             unsigned long realization)
        : AllocatedReservoirDataCollector(allocated_reservoir, realization),
          joint_wtp(wtp), parent_allocated_reservoir(allocated_reservoir) {

    wtp_utilities_with_allocations = *joint_wtp->getUtilities_with_allocations();
    reservoir_utilities_with_allocations = *parent_allocated_reservoir->getUtilities_with_allocations();

    wtp_id = joint_wtp->id;
    parent_source_id = parent_allocated_reservoir->id;
}

void JointWTPDataCollector::collect_data() {
    vector<double> temp_current_week_wtp_allocation_capacities;
    vector<double> temp_current_week_wtp_allocation_fractions;
    for (int u : wtp_utilities_with_allocations) {
        temp_current_week_wtp_allocation_capacities.push_back(joint_wtp->getAllocatedTreatmentCapacity(u));
        temp_current_week_wtp_allocation_fractions.push_back(joint_wtp->getAllocatedTreatmentCapacity(u)
                                                             / joint_wtp->getTotal_treatment_capacity(u));
    }
    wtp_allocated_treatment_fractions.push_back(temp_current_week_wtp_allocation_fractions);
    wtp_allocated_treatment_capacities.push_back(temp_current_week_wtp_allocation_capacities);

    vector<double> temp_current_week_reservoir_allocation_capacities;
    vector<double> temp_current_week_reservoir_allocation_fractions;
    for (int u : reservoir_utilities_with_allocations) {
        temp_current_week_reservoir_allocation_capacities.push_back(
                parent_allocated_reservoir->getAllocatedTreatmentCapacity(u));
        temp_current_week_reservoir_allocation_fractions.push_back(
                parent_allocated_reservoir->getAllocatedTreatmentCapacity(u)
                / parent_allocated_reservoir->getTotal_treatment_capacity(u));
    }
    reservoir_allocated_treatment_fractions.push_back(temp_current_week_reservoir_allocation_fractions);
    reservoir_allocated_treatment_capacities.push_back(temp_current_week_reservoir_allocation_capacities);

    wtp_external_allocated_treatment_fraction.push_back(joint_wtp->getExternalTreatmentAllocation());
}


string JointWTPDataCollector::printTabularString(int week) {

    stringstream out_stream;

    for (int u : wtp_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << wtp_allocated_treatment_fractions[week][u];
    out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
               << wtp_external_allocated_treatment_fraction[week];
    for (int u : wtp_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << wtp_allocated_treatment_capacities[week][u];

    for (int u : reservoir_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << reservoir_allocated_treatment_fractions[week][u];
    for (int u : reservoir_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << reservoir_allocated_treatment_capacities[week][u];

    return out_stream.str();
}

string JointWTPDataCollector::printCompactString(int week) {

    stringstream out_stream;

    for (int u : wtp_utilities_with_allocations)
        out_stream << wtp_allocated_treatment_fractions[week][u] << ",";
    out_stream << wtp_external_allocated_treatment_fraction[week] << ",";
    for (int u : wtp_utilities_with_allocations)
        out_stream << wtp_allocated_treatment_capacities[week][u] << ",";

    for (int u : reservoir_utilities_with_allocations)
        out_stream << reservoir_allocated_treatment_fractions[week][u] << ",";
    for (int u : reservoir_utilities_with_allocations)
        out_stream << reservoir_allocated_treatment_capacities[week][u] << ",";

    return out_stream.str();
}

string JointWTPDataCollector::printTabularStringHeaderLine1() {

    stringstream out_stream;

    for (int u : wtp_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Treatment" + to_string(u);
    out_stream << setw(COLUMN_WIDTH) << "TreatmentExternal";
    for (int u : wtp_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Treatment" + to_string(u);

    for (int u : reservoir_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Treatment" + to_string(u);
    for (int u : reservoir_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Treatment" + to_string(u);

    return out_stream.str();
}

string JointWTPDataCollector::printTabularStringHeaderLine2() {

    stringstream out_stream;

    for (int u : wtp_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Frac. " + to_string(u);
    out_stream << setw(COLUMN_WIDTH) << "Frac. Ext";
    for (int u : wtp_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Cap. " + to_string(u);

    for (int u : reservoir_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Frac. " + to_string(u);
    for (int u : reservoir_utilities_with_allocations)
        out_stream << setw(COLUMN_WIDTH) << "Cap. " + to_string(u);

    return out_stream.str();
}

string JointWTPDataCollector::printCompactStringHeader() {

    stringstream out_stream;

    for (int u : wtp_utilities_with_allocations)
        out_stream << wtp_id << "trt_frac_" + to_string(u) << ",";
    out_stream << wtp_id << "trt_frac_ext" << ",";
    for (int u : wtp_utilities_with_allocations)
        out_stream << wtp_id << "trt_cap_" + to_string(u) << ",";

    for (int u : reservoir_utilities_with_allocations)
        out_stream << parent_source_id << "trt_frac_" + to_string(u) << ",";
    for (int u : reservoir_utilities_with_allocations)
        out_stream << parent_source_id << "trt_cap_" + to_string(u) << ",";

    return out_stream.str();
}
