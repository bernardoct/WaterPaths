//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_AUX_H
#define TRIANGLEMODEL_AUX_H


#include "../SystemComponents/WaterSources/WaterSource.h"
#include "../SystemComponents/WaterSources/Intake.h"
#include "../SystemComponents/Utility.h"
#include "../DroughtMitigationInstruments/DroughtMitigationPolicy.h"
#include "../DroughtMitigationInstruments/Restrictions.h"

class Utils {
public:

    static vector<vector<double>> parse2DCsvFile(char const *file_name, int number_of_years, int number_of_weeks);

    static double *parse1DCsvFile(char const *file_name, int number_of_weeks);

    static vector<WaterSource *> copyWaterSourceVector(vector<WaterSource *> water_sources_original);

    static vector<Utility *> copyUtilityVector(vector<Utility *> utility_original, bool clear_water_sources = false);

    static vector<DroughtMitigationPolicy *>
    copyDroughtMitigationPolicyVector(vector<DroughtMitigationPolicy *> drought_mitigation_policy_original);

    static bool isFirstWeekOfTheYear(int week);

    static double l2distanceSquare(vector<double> v1, vector<double> v2);

    static vector<int> getQuantileIndeces(vector<double> v1, double quantile);

    static vector<vector<double>> calculateDistances(vector<vector<double>> data_points);

    static int weekOfTheYear(int week);
};


#endif //TRIANGLEMODEL_AUX_H
