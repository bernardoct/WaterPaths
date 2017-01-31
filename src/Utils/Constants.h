//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_CONSTANTS_H
#define TRIANGLEMODEL_CONSTANTS_H


#include <map>

namespace Constants {
    const int WEEKS_IN_YEAR = 52;
    const int WEEKS_ROF_SHORT_TERM = 52;
//    const int WEEKS_ROF_LONG_TERM = 78;
    const int SHORT_TERM_ROF = 0;
    const int NUMBER_REALIZATIONS_ROF = 50;
    const bool ONLINE = true;
    const double STORAGE_CAPACITY_RATIO_FAIL = 0.2;
    const double ROF_SHORT_TERM_PRECISION = 1.0 / (NUMBER_REALIZATIONS_ROF * WEEKS_ROF_SHORT_TERM);
};


#endif //TRIANGLEMODEL_CONSTANTS_H
