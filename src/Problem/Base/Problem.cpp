//
// Created by Bernardo on 11/24/2017.
//

#include <algorithm>
#include "Problem.h"

void Problem::printOutput() {

}

vector<int> Problem::vecInfraRankToVecInt(vector<infraRank> v) {
    vector<int> sorted;
    for (infraRank ir : v) {
        sorted.push_back(ir.id);
    }

    return sorted;
}

double Problem::checkAndFixInfraExpansionHighLowOrder(
        vector<int> *order, int id_low,
        int id_high, double capacity_low, double capacity_high) {

    long pos_low = distance(order->begin(),
                            find(order->begin(),
                                 order->end(),
                                 id_low));

    long pos_high = distance(order->begin(),
                             find(order->begin(),
                                  order->end(),
                                  id_high));

    if (pos_high < pos_low) {
        capacity_high += capacity_low;
        order->erase(order->begin() + pos_low);
    }

    return capacity_high;
}


void Problem::setN_realizations(int n_realizations) {
    Problem::n_realizations = n_realizations;
}

void Problem::setN_weeks(int n_weeks) {
    Problem::n_weeks = n_weeks;
}

void Problem::setSol_number(int sol_number) {
    Problem::solution_no = sol_number;
}

void Problem::setOutput_directory(const string &output_directory) {
    Problem::output_directory = output_directory;
}

void Problem::setRealizations(const vector<int> &realizations) {
    Problem::realizations = realizations;
}

void Problem::setRdm_factors(const vector<double> &rdm_factors) {
    Problem::rdm_factors = rdm_factors;
}

void Problem::setBootstrap_sample(const vector<int> &bootstrap_sample) {
    Problem::bootstrap_sample = bootstrap_sample;
}