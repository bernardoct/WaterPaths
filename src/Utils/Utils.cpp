//
// Created by bernardo on 1/13/17.
//

#include "Utils.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "../SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../SystemComponents/WaterSources/Quarry.h"
#include "../DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../SystemComponents/WaterSources/SequentialJointTreatmentExpansion.h"
#include "../SystemComponents/WaterSources/Relocation.h"
#include "../Controls/FixedMinEnvFlowControl.h"
#include "../Controls/InflowMinEnvFlowControl.h"
#include "../Controls/SeasonalMinEnvFlowControl.h"
#include "../Controls/StorageMinEnvFlowControl.h"
#include "../Controls/Custom/JordanLakeMinEnvFlowControl.h"
#include "../Controls/Custom/FallsLakeMinEnvFlowControl.h"
#include "../SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../SystemComponents/Bonds/BalloonPaymentBond.h"
#include "../SystemComponents/Bonds/FloatingInterestBalloonPaymentBond.h"
#include "../SystemComponents/WaterSources/IntakeExpansion.h"
#include "../SystemComponents/WaterSources/FixedJointWTP.h"
#include <fstream>
#include <algorithm>
#include <climits>
#include <unistd.h>
#include <sys/stat.h>

/**
 * Reads csv file into table, exported as a vector of vector of doubles.
 * @param file_name input file name (full path).
 * @param max_lines
 * @return
 */
vector<vector<double>> Utils::parse2DCsvFile(string file_name, unsigned long max_lines,
                                             vector<unsigned long> rows_to_read) {

    vector<vector<double> > data;
    ifstream inputFile(file_name);
    int l = -1;
    int ml = (int) (rows_to_read.empty() ? max_lines : *max_element(
                            rows_to_read.begin(), rows_to_read.end())) + 1;
//    if (!rows_to_read.empty())
//        ml = max_lines;
//    else
//        ml = *max_element(rows_to_read.begin(), rows_to_read.end());
    if (inputFile.is_open()) {
        while (inputFile && l < ml) {
            l++;
            string s;
            if (!getline(inputFile, s)) break;
	    if (s.find(" ") != string::npos) {
		char error[500];
		sprintf(error, "File %s seems to be space-separated.", file_name.c_str());
	        throw std::invalid_argument(error);
	    }

            vector<double> record;
            if (s[0] != '#' &&
                (rows_to_read.empty() ||
                 std::find(rows_to_read.begin(), rows_to_read.end(), l)
                 != rows_to_read.end())) {
                istringstream ss(s);

                int c = 0;
                while (ss) {
                    string line;
                    if (!getline(ss, line, ','))
                        break;
                    try {
                        record.push_back(stof(line));
                    } catch (const std::invalid_argument e) {
                        cout << "NaN found in file " << file_name << " line "
                             << l << " column " << c << endl;
                        e.what();
                    }
                    c++;
                }
            }
            data.push_back(record);
        }
    } else {
	string error = "File " + file_name + " not found.";
	char error_char[error.size() + 1];
	strcpy(error_char, error.c_str());
        throw invalid_argument(error_char);
    }

    if (rows_to_read.empty())
        return data;
    else {
        vector<vector<double>> return_data;
        for (int i : rows_to_read)
            return_data.push_back(data[i]);
        return return_data;
    }
}

vector<double> Utils::parse1DCsvFile(string file_name, unsigned long max_lines,
                                     vector<unsigned long> rows_to_read) {
    vector<double> data;
    ifstream infile(file_name);
    unsigned long l = 0;

    while (infile && l < max_lines) {
        l++;
        string s;
        if (!getline(infile, s)) break;
        if (s[0] != '#' && (rows_to_read.empty() || (!rows_to_read.empty() && l - 1 == rows_to_read[0]))) {

            istringstream ss(s);
            double record;

            try {
                record = stof(ss.str());
                data.push_back(record);
            } catch (const std::invalid_argument e) {
                cout << "NaN found in file " << file_name << " line " << l << endl;
                e.what();
            }
            if (!rows_to_read.empty())// && rows_to_read[0] != NON_INITIALIZED) 
		    rows_to_read.erase(rows_to_read.begin());
        }
    }

    if (!infile.eof() && l < max_lines) {
        cerr << "Could not read file " << file_name << "\n";
        throw invalid_argument("File not found.");
    }

    return data;
}

vector<MinEnvFlowControl *> Utils::copyMinEnvFlowControlVector(
        vector<MinEnvFlowControl *> min_env_flow_controls_original) {
    vector<MinEnvFlowControl *> min_env_flow_controls_new;

    for (MinEnvFlowControl *mef : min_env_flow_controls_original) {
        if (mef->type == FIXED_FLOW_CONTROLS)
            min_env_flow_controls_new.push_back(
                    new FixedMinEnvFlowControl(*dynamic_cast<FixedMinEnvFlowControl *>(mef)));
        else if (mef->type == INFLOW_CONTROLS)
            min_env_flow_controls_new.push_back(
                    new InflowMinEnvFlowControl(*dynamic_cast<InflowMinEnvFlowControl *>(mef)));
        else if (mef->type == SEASONAL_CONTROLS)
            min_env_flow_controls_new.push_back(
                    new SeasonalMinEnvFlowControl(*dynamic_cast<SeasonalMinEnvFlowControl *>(mef)));
        else if (mef->type == STORAGE_CONTROLS)
            min_env_flow_controls_new.push_back(
                    new StorageMinEnvFlowControl(*dynamic_cast<StorageMinEnvFlowControl *>(mef)));
        else if (mef->type == JORDAN_CONTROLS)
            min_env_flow_controls_new.push_back(
                    new JordanLakeMinEnvFlowControl(*dynamic_cast<JordanLakeMinEnvFlowControl *>(mef)));
        else if (mef->type == FALLS_CONTROLS)
            min_env_flow_controls_new.push_back(
                    new FallsLakeMinEnvFlowControl(*dynamic_cast<FallsLakeMinEnvFlowControl *>(mef)));
        else
            throw invalid_argument("One of the minimum environmental flow controls "
                                             "does not have an implementation in the "
                                             "Utils::copyWaterSourceVector function. "
                                             "Please add your control to it.");
    }

    return min_env_flow_controls_new;
}

//#pragma optimize("", off)
vector<WaterSource *> Utils::copyWaterSourceVector(
        vector<WaterSource *> water_sources_original) {
    vector<WaterSource *> water_sources_new;

    for (WaterSource *ws : water_sources_original) {
        if (ws->source_type == RESERVOIR)
            water_sources_new.push_back(
                    new Reservoir(*dynamic_cast<Reservoir *>(ws)));
        else if (ws->source_type == INTAKE)
            water_sources_new.push_back(
                    new Intake(*dynamic_cast<Intake *>(ws)));
        else if (ws->source_type == RESERVOIR_EXPANSION)
            water_sources_new.push_back(
                    new ReservoirExpansion(
                            *dynamic_cast<ReservoirExpansion *>(ws)));
        else if (ws->source_type == INTAKE_EXPANSION)
            water_sources_new.push_back(
                    new IntakeExpansion(
                            *dynamic_cast<IntakeExpansion *>(ws)));
        else if (ws->source_type == QUARRY)
            water_sources_new.push_back(
                    new Quarry(*dynamic_cast<Quarry *>(ws)));
        else if (ws->source_type == WATER_REUSE)
            water_sources_new.push_back(
                    new WaterReuse(*dynamic_cast<WaterReuse *>(ws)));
        else if (ws->source_type == ALLOCATED_RESERVOIR)
            water_sources_new.push_back(
                    new AllocatedReservoir(
                            *dynamic_cast<AllocatedReservoir *>(ws)));
        else if (ws->source_type == NEW_JOINT_WATER_TREATMENT_PLANT) {
            if (ws->getAgreementType() == NEW_JOINT_WATER_TREATMENT_PLANT_FIXED_ALLOCATIONS) {
                water_sources_new.push_back(new FixedJointWTP(*dynamic_cast<FixedJointWTP *>(ws)));
            } else if (ws->getAgreementType() == NEW_JOINT_WATER_TREATMENT_PLANT_VARIABLE_ALLOCATIONS) {
//                water_sources_new.push_back(new VariableJointWTP(*dynamic_cast<VariableJointWTP *>(ws))); NOT BUILT YET
            } else {
                throw logic_error("Error in Utils::copyWaterSourceVector, JointWTP classes not working properly.");
            }
        } else if (ws->source_type == NEW_WATER_TREATMENT_PLANT)
            water_sources_new.push_back(
                    new SequentialJointTreatmentExpansion(
                            *dynamic_cast<SequentialJointTreatmentExpansion *>(ws)));
        else if (ws->source_type == SOURCE_RELOCATION)
            water_sources_new.push_back(
                    new Relocation(
                            *dynamic_cast<Relocation *>(ws)));
        else
            throw invalid_argument("One of the water sources does not have "
                                             "an implementation in the "
                                             "Utils::copyWaterSourceVector "
                                             "function. Please add your "
                                             "source to it.");
    }

    return water_sources_new;
}

vector<Utility *> Utils::copyUtilityVector(vector<Utility *> utility_original,
                                           bool clear_water_sources) {
    vector<Utility *> utility_new;

    for (Utility *u : utility_original) {
        utility_new.push_back(new Utility(*u));
    }

    if (clear_water_sources)
        for (Utility *u : utility_new) {
            u->clearWaterSources();
        }

    return utility_new;
}

vector<DroughtMitigationPolicy *>
Utils::copyDroughtMitigationPolicyVector(
        vector<DroughtMitigationPolicy *> drought_mitigation_policy_original) {
    vector<DroughtMitigationPolicy *> drought_mitigation_policy_new;

    for (DroughtMitigationPolicy *dmp : drought_mitigation_policy_original) {
        if (dmp->type == RESTRICTIONS)
            drought_mitigation_policy_new.push_back(
                    new Restrictions(*dynamic_cast<Restrictions *>(dmp)));
        else if (dmp->type == TRANSFERS)
            drought_mitigation_policy_new.push_back(
                    new Transfers(*dynamic_cast<Transfers *>(dmp)));
        else if (dmp->type == INSURANCE_STORAGE_ROF)
            drought_mitigation_policy_new.push_back(
                    new InsuranceStorageToROF(
                            *dynamic_cast<InsuranceStorageToROF *>(dmp)));
    }

    return drought_mitigation_policy_new;
}

vector<Bond *> Utils::copyBonds(vector<Bond *> bonds_original) {
    vector<Bond *> bonds_new;

    for (Bond *bond : bonds_original) {
        if (bond->type == LEVEL_DEBT_SERVICE)
            bonds_new.push_back(new LevelDebtServiceBond(*dynamic_cast<LevelDebtServiceBond *>(bond)));
        else if (bond->type == BALLOON_PAYMENT)
            bonds_new.push_back(new BalloonPaymentBond(*dynamic_cast<BalloonPaymentBond *>(bond)));
        else if (bond->type == FLOATING_INTEREST)
            bonds_new.push_back(new FloatingInterestBalloonPaymentBond(
                    *dynamic_cast<FloatingInterestBalloonPaymentBond *>(bond)));
        else
            throw invalid_argument("Your bond type does not have a corresponding "
                                     "copy function in Utils::copyBonds yet.\n");
    }

    return bonds_new;
}

bool Utils::isFirstWeekOfTheYear(int week) {
    return WEEK_OF_YEAR[week] == 0;
}

int Utils::weekOfTheYear(int week) {
    return WEEK_OF_YEAR[week];
}

void Utils::removeIntFromVector(vector<int>& vec, int el) {

    auto vbeg = vec.begin();
    auto vend = vec.end();
    vec.erase(std::remove(vbeg, vend, el), vend);
}

void Utils::print_exception(const std::exception& e, int level) {
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& e) {
        print_exception(e, level+1);
    } catch(...) {}
}

void Utils::createDir(string directory) {
    string create_dir_command;
#ifdef _WIN32
    create_dir_command = "if not exist \"" + directory + "\" mkdir ";
#else
    create_dir_command = "mkdir -p";
#endif
    struct stat sb;
        // Check if io_directory exists and print either location or that io_directory does not exist.
    if (stat(directory.c_str(), &sb) != 0) {
        auto output = system((create_dir_command + " " + directory).c_str());
    }
}
