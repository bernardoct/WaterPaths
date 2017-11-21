//
// Created by bernardoct on 6/26/17.
//

#ifndef TRIANGLEMODEL_CONTROLRULES_H
#define TRIANGLEMODEL_CONTROLRULES_H


class ControlRules {
    virtual double get_dependent_variable(double x) = 0;

    virtual double get_dependent_variable(int x) = 0;

    virtual double get_dependent_variable(double x, int week) = 0;

    virtual double get_dependent_variable(int x, int week) = 0;
};


#endif //TRIANGLEMODEL_CONTROLRULES_H
