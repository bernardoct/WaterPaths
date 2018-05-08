//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_AUX_H
#define TRIANGLEMODEL_AUX_H


#include "../SystemComponents/WaterSources/Base/WaterSource.h"
#include "../SystemComponents/WaterSources/Intake.h"
#include "../SystemComponents/Utility/Utility.h"
#include "../DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h"
#include "../DroughtMitigationInstruments/Restrictions.h"
#include "../Controls/Base/MinEnvFlowControl.h"

class Utils {
public:

    static vector<vector<double>> parse2DCsvFile(basic_string<char, char_traits<char>, allocator<char>> file_name,
                                                 unsigned long max_lines = 10000000,
                                                 vector<unsigned long> rows_to_read = vector<unsigned long>());

    static vector<double> parse1DCsvFile(basic_string<char, char_traits<char>, allocator<char>> file_name,
                                         unsigned long max_lines = 10000000,
                                         vector<unsigned long> rows_to_read = vector<unsigned long>());

    static vector<WaterSource *> copyWaterSourceVector(vector<WaterSource *> water_sources_original);

    static vector<Utility *> copyUtilityVector(vector<Utility *> utility_original, bool clear_water_sources = false);

    static vector<DroughtMitigationPolicy *> copyDroughtMitigationPolicyVector(
            vector<DroughtMitigationPolicy *> drought_mitigation_policy_original);

    static vector<MinEnvFlowControl *> copyMinEnvFlowControlVector(
            vector<MinEnvFlowControl *> min_env_flow_controls_original);

    static bool isFirstWeekOfTheYear(int week);

    static int weekOfTheYear(int week);

    static void removeIntFromVector(vector<int> &vec, int el);

    static void print_exception(const exception &e, int level = 0);

    static vector<Bond *> copyBonds(vector<Bond *> bonds_original);
};


#endif //TRIANGLEMODEL_AUX_H
