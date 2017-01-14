//
// Created by bernardo on 1/13/17.
//

#include "Aux.h"
#include <fstream>
#include <sstream>

double **Aux::parse2DCsvFile(char *file_name, int number_of_years, int number_of_time_steps) {

    double **data = 0;
    data = new double *[number_of_years];

    std::ifstream file(file_name);

    for (int row = 0; row < number_of_years; row++) {
        data[row] = new double[number_of_time_steps];
        std::string line;
        std::getline(file, line);
//        if (!file.good())
//            break;

        std::stringstream iss(line);

        for (int col = 0; col < number_of_time_steps; col++) {
            std::string val;
            std::getline(iss, val, ',');
//            if (!iss.good())
//                break;

            std::stringstream convertor(val);
            convertor >> data[row][col];
        }
    }

    return data;
}
