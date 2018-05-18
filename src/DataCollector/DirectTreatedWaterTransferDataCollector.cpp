//
// Created by David on 5/17/2018.
//

#include <sstream>
#include <iomanip>
#include "DirectTreatedWaterTransferDataCollector.h"

DirectTreatedWaterTransferDataCollector::DirectTreatedWaterTransferDataCollector(DirectTreatedWaterTransfer *dtwt,
                                                                                 unsigned long realization)
        : DataCollector(dtwt->id, nullptr, realization, DIRECT_TRANSFERS, NON_INITIALIZED),
          dtwt_policy(dtwt) {

}

void DirectTreatedWaterTransferDataCollector::collect_data() {
    buying_utility = dtwt_policy->getBuyingUtilityID();
    weekly_initial_requested_transfer_volume.push_back(dtwt_policy->getInitialTransferRequest());

    possible_seller_utilities = dtwt_policy->getPotentialSellingUtilities();
    weekly_quantities_sold.push_back(dtwt_policy->getSoldQuantities());
    weekly_available_treatment_capacity_to_sell.push_back(dtwt_policy->getAvailableExcessTreatmentCapacityToSell());
    weekly_available_supply_capacity_to_sell.push_back(dtwt_policy->getAvailableExcessSupplyCapacityToSell());
}

string DirectTreatedWaterTransferDataCollector::printCompactStringHeader() {
    stringstream out_stream;

    out_stream << dtwt_policy->id << "init_req_" << to_string(buying_utility) << ",";
    for (int u : possible_seller_utilities)
        out_stream << dtwt_policy->id << "vol_sold_" + to_string(u) << ",";
    for (int u : possible_seller_utilities)
        out_stream << dtwt_policy->id << "exc_trt_cap_" + to_string(u) << ",";
    for (int u : possible_seller_utilities)
        out_stream << dtwt_policy->id << "exc_sup_vol_" + to_string(u) << ",";

    return out_stream.str();
}

string DirectTreatedWaterTransferDataCollector::printTabularString(int week) {
    stringstream out_stream;

    out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
               << weekly_initial_requested_transfer_volume[week];
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << weekly_quantities_sold[week][u];
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << weekly_available_treatment_capacity_to_sell[week][u];
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << setprecision(COLUMN_PRECISION)
                   << weekly_available_supply_capacity_to_sell[week][u];

    return out_stream.str();
}

string DirectTreatedWaterTransferDataCollector::printCompactString(int week) {
    stringstream out_stream;

    out_stream << weekly_initial_requested_transfer_volume[week] << ",";
    for (int u : possible_seller_utilities)
        out_stream << weekly_quantities_sold[week][u] << ",";
    for (int u : possible_seller_utilities)
        out_stream << weekly_available_treatment_capacity_to_sell[week][u] << ",";
    for (int u : possible_seller_utilities)
        out_stream << weekly_available_supply_capacity_to_sell[week][u] << ",";

    return out_stream.str();
}

string DirectTreatedWaterTransferDataCollector::printTabularStringHeaderLine1() {
    stringstream out_stream;

    out_stream << setw(COLUMN_WIDTH) << "Initial";
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << "Transfer" + to_string(u);
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << "Avail.Trtmt." + to_string(u);
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << "Avail.Stor." + to_string(u);

    return out_stream.str();
}

string DirectTreatedWaterTransferDataCollector::printTabularStringHeaderLine2() {
    stringstream out_stream;

    out_stream << setw(COLUMN_WIDTH) << "Request";
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << "Sold" + to_string(u);
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << "Capacity" + to_string(u);
    for (int u : possible_seller_utilities)
        out_stream << setw(COLUMN_WIDTH) << "Volume" + to_string(u);

    return out_stream.str();
}