//
// Created by Bernardo on 11/24/2017.
//

#include <algorithm>
#include "Problem.h"

double * Problem::calculateObjectivesAndPrintOutput() {

    /// Calculate objective values.
    master_data_collector->setOutputDirectory(output_directory);

    /// Print output files.
    string fu = "/TestFiles/output/Utilities";
    string fws = "/TestFiles/output/WaterSources";
    string fp = "/TestFiles/output/Policies";
    string fo = "/TestFiles/output/Objectives";
    string fpw = "/TestFiles/output/Pathways";

    //FIXME:PRINT_POLICIES_OUTPUT_TABULAR BLOWING UP MEMORY.
    cout << "Calculating and printing Objectives" << endl;
    master_data_collector->calculatePrintObjectives(fo + "_s" + std::to_string(solution_no) + fname_sufix, true);
    cout << "Printing Pathways" << endl;
    master_data_collector->printPathways(fpw + "_s" + std::to_string(solution_no) + fname_sufix);
    cout << "Printing time series" << endl;
    master_data_collector->printUtilitiesOutputCompact(0, (int) n_weeks, fu + "_s"
                                                       + std::to_string(solution_no) + fname_sufix);
    master_data_collector->printWaterSourcesOutputCompact(0, (int) n_weeks, fws + "_s"
                                                          + std::to_string(solution_no) + fname_sufix);
    master_data_collector->printPoliciesOutputCompact(0, (int) n_weeks, fp + "_s"
                                                      + std::to_string(solution_no) + fname_sufix);
    cout << "Updating objectives pointer" << endl;
//    data_collector->printUtilitesOutputTabular(0,
//                                               n_weeks,
//                                               fu + "_s"
//                                               + std::to_string(solution_no));
//    data_collector->printWaterSourcesOutputTabular(0,
//                                                   n_weeks,
//                                                   fws + "_s"
//                                                   + std::to_string(solution_no));
//    data_collector->printPoliciesOutputTabular(0,
//                                               n_weeks,
//                                               fp + "_s"
//                                               + std::to_string(solution_no));
    double *obj_not_jla = master_data_collector->calculatePrintObjectives(fo + "_s" + std::to_string(solution_no),
                                                                          false).data();

    return obj_not_jla;
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


void Problem::setN_realizations(unsigned long n_realizations) {
    Problem::n_realizations = n_realizations;
}

void Problem::setN_weeks(unsigned long n_weeks) {
    Problem::n_weeks = n_weeks;
}

void Problem::setSol_number(unsigned long sol_number) {
    Problem::solution_no = sol_number;
}

void Problem::setOutput_directory(const string &output_directory) {
    Problem::output_directory = output_directory;
}

void Problem::setRealizations(const vector<unsigned long> &realizations) {
    Problem::realizations = realizations;
}

void Problem::setBootstrap_sample(const vector<unsigned long> &bootstrap_sample) {
    Problem::bootstrap_sample = bootstrap_sample;
}

void Problem::setRDMOptimization(vector<vector<double>> &utilities_rdm,
                                 vector<vector<double>> &water_sources_rdm,
                                 vector<vector<double>> &policies_rdm) {
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
}

void Problem::setRDMReevaluation(unsigned long rdm_no, vector<vector<double>> &utilities_rdm,
                                 vector<vector<double>> &water_sources_rdm,
                                 vector<vector<double>> &policies_rdm) {
    this->rdm_no = rdm_no;
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
}

void Problem::setFname_sufix(const string &fname_sufix) {
    Problem::fname_sufix = fname_sufix;
}

void Problem::setEvap_inflows_suffix(const string &evap_inflows_suffix) {
    Problem::evap_inflows_suffix = evap_inflows_suffix;
}
