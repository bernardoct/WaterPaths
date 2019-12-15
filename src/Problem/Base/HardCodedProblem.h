//
// Created by Bernardo on 12/8/2019.
//

#ifndef TRIANGLEMODEL_HARDCODEDPROBLEM_H
#define TRIANGLEMODEL_HARDCODEDPROBLEM_H

#include "Problem.h"

class HardCodedProblem : public Problem {
public:
    HardCodedProblem(unsigned long n_weeks, int n_realizations,
                     int import_export_rof_table, string system_io,
                     string &rof_tables_directory, int seed,
                     unsigned long n_threads, string bootstrap_file,
                     string &utilities_rdm_file, string &policies_rdm_file,
                     string &water_sources_rdm_file, int n_sets,
                     int n_bs_samples, string &solutions_file,
                     vector<int> &solutions_to_run_range, bool plotting,
                     bool print_obj_row);

    void setRDMOptimization(vector<vector<double>> &utilities_rdm,
                            vector<vector<double>> &water_sources_rdm,
                            vector<vector<double>> &policies_rdm);

    void setRDMReevaluation(int rdm_no, vector<vector<double>> &utilities_rdm,
                            vector<vector<double>> &water_sources_rdm,
                            vector<vector<double>> &policies_rdm);

    static double checkAndFixInfraExpansionHighLowOrder(vector<int> &order,
                                                 vector<double> &triggers,
                                                 int id_low, int id_high,
                                                 double capacity_low,
                                                 double capacity_high);

    vector<int> vecInfraRankToVecInt(vector<infraRank> v);

    void runSimulation() override;

    virtual void readInputData() override;
};

#endif //TRIANGLEMODEL_HARDCODEDPROBLEM_H
