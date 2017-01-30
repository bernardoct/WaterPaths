//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_AUX_H
#define TRIANGLEMODEL_AUX_H


#include "../SystemComponents/WaterSources/WaterSource.h"
#include "../SystemComponents/Utility.h"

class Aux {
public:
    static double **parse2DCsvFile(char const *file_name, int number_of_years, int number_of_weeks);

    static double *parse1DCsvFile(char const *file_name, int number_of_weeks);

    static vector<WaterSource *> copyWaterSourceVector(vector<WaterSource *> water_sources_original);

    static vector<Utility *> copyUtilityVector(vector<Utility *> utility_original);
};


#endif //TRIANGLEMODEL_AUX_H
