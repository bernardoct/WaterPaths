//
// Created by Bernardo on 11/23/2017.
//

#ifndef TRIANGLEMODEL_TRIANGLE_H
#define TRIANGLEMODEL_TRIANGLE_H

#include "Base/Problem.h"

using namespace std;

class Triangle : public Problem {

public:
    explicit Triangle(int solution_no, int rdm_no);

    void functionEvaluation(const double *vars, double *objs, double *consts) override;
};


#endif //TRIANGLEMODEL_TRIANGLE_H
