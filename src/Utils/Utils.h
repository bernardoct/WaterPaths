//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_AUX_H
#define TRIANGLEMODEL_AUX_H


#include <sstream>
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

    static void createDir(string directory);


private:
    template<class T> struct the_type { using type = T; };

    static double convert_token(string &s, the_type<double>) { return stod(s); };
    static int convert_token(string &s, the_type<int>) { return stoi(s); };
    static string convert_token(string &s, the_type<string>) { return s; };
public:
    template <class T>
    static void tokenizeString(string &line, vector<T> &tokenized_vector, char token = ' ') {
        tokenized_vector = vector<T>(0);
        if (line[0] != '#') {
            istringstream ss(line);

            int c = 0;
            while (!ss.eof()) {
                string s;
                if (!getline(ss, s, token))
                    break;
                try {
                    tokenized_vector.push_back(convert_token(s, the_type<T>()));
                } catch (const std::invalid_argument &e) {
                    cout << "NaN found in file system tag " << ", line " << line << endl;
                    e.what();
                }
                c++;
            }
        }
    };
};


#endif //TRIANGLEMODEL_AUX_H
