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
    int n_realizations;
    int n_weeks;
    int solution_no;
    int rdm_no;
    string output_directory;
    vector<int> realizations;
    vector<double> rdm_factors;
    vector<int> bootstrap_sample;
    MasterDataCollector master_data_collector;

    double checkAndFixInfraExpansionHighLowOrder(vector<int> *order, int id_low, int id_high, double capacity_low,
                                                 double capacity_high);

    vector<int> vecInfraRankToVecInt(vector<infraRank> v);

public:
    virtual void functionEvaluation(const double* vars, double* objs, double* consts)=0;

    void setBootstrap_sample(const vector<int> &bootstrap_sample);

    void setN_realizations(int n_realizations);

    void setN_weeks(int n_weeks);

    void setSol_number(int sol_number);

    void setOutput_directory(const string &output_directory);

    void setRealizations(const vector<int> &realizations);

    void setRdm_factors(const vector<double> &rdm_factors);

    void printOutput();
};


#endif //TRIANGLEMODEL_PROBLEM_H
