//
// Created by Bernardo on 11/24/2017.
//

#ifndef TRIANGLEMODEL_PROBLEM_H
#define TRIANGLEMODEL_PROBLEM_H

#include <vector>
#include "../../DataCollector/Base/DataCollector.h"
#include "../../DataCollector/MasterDataCollector.h"

struct infraRank {
    int id;
    double xreal;

    infraRank(int id, double xreal) {
        this->id = id;
        this->xreal = xreal;
    }
};

struct by_xreal {
    bool operator()(const infraRank &ir1, const infraRank &ir2) {
        return ir1.xreal < ir2.xreal;
    }
};

class Problem {
protected:
    unsigned long n_realizations;
    unsigned long  n_weeks;
    unsigned long  solution_no;
    unsigned long  rdm_no;
    string output_directory;
    vector<unsigned long > realizations;
    vector<double> rdm_factors;
    vector<unsigned long > bootstrap_sample;
    MasterDataCollector* master_data_collector;
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;

    double checkAndFixInfraExpansionHighLowOrder(vector<int> *order, int id_low, int id_high, double capacity_low,
                                                 double capacity_high);

    vector<int> vecInfraRankToVecInt(vector<infraRank> v);

public:
    virtual void functionEvaluation(const double* vars, double* objs, double* consts)=0;

    void setBootstrap_sample(const vector<unsigned long> &bootstrap_sample);

    void setN_realizations(unsigned long n_realizations);

    void setN_weeks(unsigned long n_weeks);

    void setSol_number(unsigned long sol_number);

    void setOutput_directory(const string &output_directory);

    void setRealizations(const vector<unsigned long> &realizations);

    void setRdm_factors(const vector<double> &rdm_factors);

    double* calculateObjectivesAndPrintOutput();
};


#endif //TRIANGLEMODEL_PROBLEM_H
