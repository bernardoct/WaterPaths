//
// Created by bernardoct on 4/26/17.
//

#ifndef TRIANGLEMODEL_PROBLEM_H
#define TRIANGLEMODEL_PROBLEM_H


#include "../../src/Simulation/Simulation.h"

class Problem {
protected:
    Simulation *simulation;
    unsigned int seed;

public:
    Problem(unsigned int seed);

    virtual void run()= 0;
};


#endif //TRIANGLEMODEL_PROBLEM_H
