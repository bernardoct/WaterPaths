//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_CONSTANTS_H
#define TRIANGLEMODEL_CONSTANTS_H


#include <map>
#include <cmath>


namespace Constants {
    const double PEAKING_FACTOR = 0.85; ///Peaking factor used to move from daily capacity to average weekly capacity.
    const double WEEKS_IN_YEAR = 52.179;
    const int NUMBER_OF_MONTHS = 12;
    const double WEEKS_IN_MONTH = WEEKS_IN_YEAR / (double) NUMBER_OF_MONTHS;
    const int WEEKS_ROF_SHORT_TERM = 52;
    const int WEEKS_ROF_LONG_TERM = 78;
    const int SHORT_TERM_ROF = 0;
    const int LONG_TERM_ROF = 1;
    const int NUMBER_REALIZATIONS_INSURANCE_PRICING = 50;
    const int NUMBER_REALIZATIONS_ROF = 50;
    const int TREATMENT_CAPACITY_VS_VOLUME_SMALL_WTP = 200;
    const int ILLIMITED_TREATMENT_CAPACITY = 99999;
    const double STORAGE_CAPACITY_RATIO_FAIL = 0.2;

    const bool APPLY_DEMAND_BUFFER = true;

    const bool ONLINE = true;
    const bool OFFLINE = false;

    const int FAILURE = 1;
    const int NON_FAILURE = 0;

    const int NONE = 0;
    const int NON_INITIALIZED = -1;
    
    const int INTAKE = 0;
    const int RESERVOIR = 1;
    const int RESERVOIR_EXPANSION = 2;
    const int QUARRY = 3;
    const int WATER_REUSE = 4;
    const int ALLOCATED_RESERVOIR = 5;
    const int NEW_WATER_TREATMENT_PLANT = 6;
    const int SOURCE_RELOCATION = 7;

    const int TRANSFERS = 0;
    const int RESTRICTIONS = 1;
    const int INSURANCE_PSEUDO_ROF = 2;
    const int INSURANCE_STORAGE_ROF = 3;
    const int RAW_WATER_TRANSFERS = 4;

    const int FIXED_FLOW_CONTROLS = 0;
    const int INFLOW_CONTROLS = 1;
    const int SEASONAL_CONTROLS = 2;
    const int STORAGE_CONTROLS = 3;
    const int JORDAN_CONTROLS = 4;
    const int FALLS_CONTROLS = 5;
    
    const int COLUMN_WIDTH = 10;
    const int COLUMN_PRECISION = 4;

    const double WORSE_CASE_COST_PERCENTILE = 0.99;

    const double INSURANCE_SHIFT_STORAGE_CURVES_THRESHOLD = 0.05;
    const int NO_OF_INSURANCE_STORAGE_TIERS = (int) std::round(1. / INSURANCE_SHIFT_STORAGE_CURVES_THRESHOLD);

    const int WATER_QUALITY_ALLOCATION = -1;

    const std::string base_directory = "C:/Users/David/OneDrive/UNC/Research/WSC/Coding/RevampedTriangle";

    // FIXME: SEE ABOUT HOW TO UP THIS TO 50 INSTEAD OF 20
    const int NO_OF_RAW_WATER_TRANSFER_STORAGE_TIERS = NO_OF_INSURANCE_STORAGE_TIERS;
};


#endif //TRIANGLEMODEL_CONSTANTS_H
