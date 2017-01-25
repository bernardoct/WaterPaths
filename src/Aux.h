//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_AUX_H
#define TRIANGLEMODEL_AUX_H


class Aux {
public:
    static double **parse2DCsvFile(char const *file_name, int number_of_years, int number_of_weeks);

    static double *parse1DCsvFile(char const *file_name, int number_of_weeks);
};


#endif //TRIANGLEMODEL_AUX_H
