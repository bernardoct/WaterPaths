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

public:
    explicit Triangle(int solution_no, int rdm_no);

    void functionEvaluation(const double *vars, double *objs, double *consts) override;

    void setRof_tables(const vector<Matrix3D<double>> &rof_tables);
};


#endif //TRIANGLEMODEL_TRIANGLE_H
