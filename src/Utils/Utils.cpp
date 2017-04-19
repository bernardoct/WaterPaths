//
// Created by bernardo on 1/13/17.
//

#include "Utils.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include <fstream>

/**
 * Reads CSV file into 2D array.
 *
 * @param file_name Name of input file.
 * @param number_of_years Number of years in record (each row represents a year).
 * @param number_of_weeks Number of time steps in each year (52 weeks, 365 days, etc.)
 * @return Double pointer array containing years in rows and time steps in columns.
 */
double **Utils::parse2DCsvFile(char const *file_name, int number_of_records, int number_of_weeks) {

    double **data = 0;
    data = new double *[number_of_records];

    std::ifstream file(file_name);

    for (int row = 0; row < number_of_records; row++) {
        data[row] = new double[number_of_weeks];
        std::string line;
        std::getline(file, line);
//        if (!file.good())
//            break;

        std::stringstream iss(line);

        for (int col = 0; col < number_of_weeks; col++) {
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

double *Utils::parse1DCsvFile(char const *file_name, int number_of_weeks) {

    double *data = new double[number_of_weeks];

    std::ifstream file(file_name);

    for (int row = 0; row < number_of_weeks; row++) {
        std::string line;
        std::getline(file, line);
//        if (!file.good())
//            break;

//        std::stringstream convertor(line);
        data[row] = atof(line.c_str());
    }

    return data;
}

vector<WaterSource *> Utils::copyWaterSourceVector(vector<WaterSource *> water_sources_original) {
    vector<WaterSource *> water_sources_new;

    for (WaterSource *ws : water_sources_original) {
        if (ws->source_type == RESERVOIR) {
            water_sources_new.push_back(new Reservoir(*dynamic_cast<Reservoir *>(ws)));
        } else if (ws->source_type == INTAKE) {
            water_sources_new.push_back(new Intake(*dynamic_cast<Intake *>(ws)));
        }
    }

    return water_sources_new;
}

vector<Utility *> Utils::copyUtilityVector(vector<Utility *> utility_original) {
    vector<Utility *> utility_new;

    for (Utility *u : utility_original) {
        utility_new.push_back(new Utility(*u));
    }

    return utility_new;
}

vector<DroughtMitigationPolicy *>
Utils::copyDroughtMitigationPolicyVector(vector<DroughtMitigationPolicy *> drought_mitigation_policy_original) {
    vector<DroughtMitigationPolicy *> drought_mitigation_policy_new;

    for (DroughtMitigationPolicy *dmp : drought_mitigation_policy_original) {
        if (dmp->type == RESTRICTIONS)
            drought_mitigation_policy_new.push_back(new Restrictions(*dynamic_cast<Restrictions *>(dmp)));
        else if (dmp->type == TRANSFERS)
            drought_mitigation_policy_new.push_back(new Transfers(*dynamic_cast<Transfers *>(dmp)));
    }

    return drought_mitigation_policy_new;
}