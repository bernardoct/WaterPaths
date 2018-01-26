//
// Created by Bernardo on 11/23/2017.
//

#ifndef TRIANGLEMODEL_TRIANGLE_H
#define TRIANGLEMODEL_TRIANGLE_H

#include "Base/Problem.h"

using namespace std;

class Triangle : public Problem {
private:
    vector<Matrix3D<double>> rof_tables;
    vector<vector<double>> infra_table_shift;
    const bool use_precomp_rof_tables;

public:
    explicit Triangle(int solution_no, int rdm_no, bool use_precomp_rof_tables);

    void functionEvaluation(const double *vars, double *objs, double *consts) override;

    void setRofTables(const string &folder, int n_realizations, int n_weeks,
                      vector<vector<double>> &infra_table_shift);
};


#endif //TRIANGLEMODEL_TRIANGLE_H
