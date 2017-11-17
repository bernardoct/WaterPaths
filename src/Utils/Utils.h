//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_AUX_H
#define TRIANGLEMODEL_AUX_H


#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../SystemComponents/WaterSources/Intake.h"
#include "../SystemComponents/Utility.h"
#include "../DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h"
#include "../DroughtMitigationInstruments/Restrictions.h"
#include "../Controls/Base/MinEnvironFlowControl.h"

class Utils {
public:

    static vector<vector<float>> parse2DCsvFile(basic_string<char, char_traits<char>, allocator<char>> file_name, 
        int max_lines = 100000000, vector<int> rows_to_read = vector<int>());

    static vector<float> parse1DCsvFile(basic_string<char, char_traits<char>, allocator<char>> file_name, 
        int max_lines = 100000000, vector<int> rows_to_read = vector<int>());

    static vector<WaterSource *> copyWaterSourceVector(vector<WaterSource *> water_sources_original);

    static vector<Utility *> copyUtilityVector(vector<Utility *> utility_original, bool clear_water_sources = false);

    static vector<DroughtMitigationPolicy *>
    copyDroughtMitigationPolicyVector(vector<DroughtMitigationPolicy *> drought_mitigation_policy_original);

    static bool isFirstWeekOfTheYear(int week);

    static float l2distanceSquare(vector<float> v1, vector<float> v2);

    static vector<int> getQuantileIndeces(vector<float> v1, float quantile);

    static vector<vector<float>> calculateDistances(vector<vector<float>> data_points);

    static int weekOfTheYear(int week);

    static string getexepath();

    static void removeIntFromVector(vector<int> &vec, int el);
};


#endif //TRIANGLEMODEL_AUX_H
