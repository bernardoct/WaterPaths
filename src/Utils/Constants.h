//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_CONSTANTS_H
#define TRIANGLEMODEL_CONSTANTS_H


#include <map>


namespace Constants {
    const double PEAKING_FACTOR = 0.85; ///Peaking factor used to move from daily capacity to average weekly capacity.
    const int WEEKS_IN_YEAR = 52;
    const int WEEKS_ROF_SHORT_TERM = 52;
    const int WEEKS_ROF_LONG_TERM = 78;
    const int SHORT_TERM_ROF = 0;
    const int NUMBER_REALIZATIONS_ROF = 50;
    const bool ONLINE = true;
    const bool OFFLINE = false;
    const double STORAGE_CAPACITY_RATIO_FAIL = 0.2;
    const int FAILURE = 1;
    const int NON_FAILURE = 0;
    const int NONE = 0;
    const int INTAKE = 0;
    const int RESERVOIR = 1;
    const int TRANSFERS = 0;
    const int RESTRICTIONS = 1;
    const int UTILITIES = 0;
    const int RESERVOIRS = 1;
};


#endif //TRIANGLEMODEL_CONSTANTS_H
