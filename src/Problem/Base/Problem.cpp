//
// Created by Bernardo on 11/24/2017.
//

#include <algorithm>
#include <numeric>
#include <random>
#include "Problem.h"

vector<double> Problem::calculateAndPrintObjectives(bool print_files) {
    if (this->master_data_collector != nullptr) {
        this->master_data_collector->setOutputDirectory(output_directory);
        string fo = "/TestFiles/output/Objectives";
        objectives = this->master_data_collector->calculatePrintObjectives(
                fo + "_s" + std::to_string(solution_no) + fname_sufix, print_files);
    
        return objectives;
    } else {
	objectives = vector<double>(25, 1e5);
	return objectives;
    }
}

void Problem::printTimeSeriesAndPathways() {
    /// Calculate objective values.
    if (this->master_data_collector != nullptr) {
    this->master_data_collector->setOutputDirectory(output_directory);

    /// Print output files.
    string fu = "/TestFiles/output/Utilities";
    string fws = "/TestFiles/output/WaterSources";
    string fp = "/TestFiles/output/Policies";
    string fpw = "/TestFiles/output/Pathways";

    //FIXME:PRINT_POLICIES_OUTPUT_TABULAR BLOWING UP MEMORY.
    cout << "Printing Pathways" << endl;
    this->master_data_collector->printPathways(
            fpw + "_s" + std::to_string(solution_no) + fname_sufix);
    cout << "Printing time series" << endl;
    this->master_data_collector->printUtilitiesOutputCompact(
            0, (int) n_weeks, fu + "_s" + std::to_string(solution_no) +
                    fname_sufix);
    this->master_data_collector->printWaterSourcesOutputCompact(
            0, (int) n_weeks, fws + "_s" + std::to_string(solution_no) +
                    fname_sufix);
    this->master_data_collector->printPoliciesOutputCompact(
            0, (int) n_weeks, fp + "_s" + std::to_string(solution_no) +
                    fname_sufix);
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
    } else {
	printf("Trying to print pathways but data collector is empty. Either your simulation crashed or you deleted the data collector too early.\n");
    }

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


void Problem::setN_weeks(unsigned long n_weeks) {
    Problem::n_weeks = n_weeks;
}

void Problem::setSol_number(unsigned long sol_number) {
    Problem::solution_no = sol_number;
}

void Problem::setOutput_directory(const string &output_directory) {
    cout << "Output will be printed in " << output_directory << endl;
    Problem::output_directory = output_directory;
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

void Problem::setN_threads(unsigned long n_threads) {
    Problem::n_threads = n_threads;
}

const vector<double> &Problem::getObjectives() const {
    return objectives;
}

void Problem::setPrint_output_files(bool print_output_files) {
    Problem::print_output_files = print_output_files;
}

void Problem::setN_realizations(unsigned long n_realizations) {
    Problem::n_realizations = n_realizations;

    realizations_to_run = vector<unsigned long>(n_realizations);
    iota(begin(realizations_to_run), end(realizations_to_run), 0);
}

void Problem::setRealizationsToRun(vector<unsigned long>& realizations_to_run) {
    this->realizations_to_run = realizations_to_run;
}

MasterDataCollector* Problem::getMaster_data_collector() {
    return master_data_collector;
}

Problem::~Problem() {}

void Problem::destroyDataCollector() {
    if (master_data_collector != nullptr) {
        delete master_data_collector;
	    master_data_collector = nullptr;
    } else {
        cerr << "Tried to delete nullptr master data collector.\n";
    }
}

Problem::Problem(unsigned long n_weeks) : n_weeks(n_weeks) {

}
