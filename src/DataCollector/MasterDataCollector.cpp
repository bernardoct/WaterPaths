//;;
// Created by bernardoct on 8/26/17.
//

#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include <numeric>
#include <random>
#include <algorithm>

#ifdef NETCDF
#include <netcdf> 
using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;
// Return this in event of a problem.
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
static const int NC_ERR = 2;
#define DEFLATE_LEVEL_9 9
#endif

#include "MasterDataCollector.h"
#include "../Utils/ObjectivesCalculator.h"
#include "../Utils/Utils.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "TransfersDataCollector.h"
#include "../SystemComponents/WaterSources/Quarry.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "ReservoirDataCollector.h"
#include "IntakeDataCollector.h"
#include "QuaryDataCollector.h"
#include "WaterReuseDataCollector.h"
#include "AllocatedReservoirDataCollector.h"
#include "EmptyDataCollector.h"

using namespace Constants;


MasterDataCollector::MasterDataCollector(unsigned long n_realizations) : n_realizations(n_realizations) {}

MasterDataCollector::~MasterDataCollector() {
    for (vector<DataCollector *> dcs : water_source_collectors)
        for (DataCollector *dc : dcs)
            delete dc;

    for (vector<DataCollector *> dcs : drought_mitigation_policy_collectors)
        for (DataCollector *dc : dcs)
            delete dc;

    for (vector<UtilitiesDataCollector *> dcs : utility_collectors)
        for (UtilitiesDataCollector *dc : dcs)
            delete dc;
}

/**
* @todo NetCDF file being saved with hardly any compression. Someone who knows more
* about NetCDF may be able to improve compression, which would be great given the 
* amount of output that can be generated in one run.
*/
int MasterDataCollector::printNETCDFUtilities(string base_file_name) {
#ifdef NETCDF
    unsigned long n_weeks = utility_collectors[0][0]->getCombined_storage().size();
    unsigned long n_realizations = utility_collectors[0].size();
    unsigned long n_utilities = utility_collectors.size();
    unsigned long n_vars = 6;

    try {
	string file_name = io_directory + base_file_name + ".nc";
	printf("Printing NetCDF output in %s.\n", file_name.c_str());
	int ncid, dimid, csid, strofid, retval;
	vector<int> group_ids(n_realizations);
	vector<int> utilities_group_ids(n_utilities * n_realizations);
	vector<int> vars_ids(n_utilities * n_realizations * n_vars);

        /* Create the file. The NC_NETCDF4 flag tells netCDF to
         * create a netCDF-4/HDF5 file.
	 * */
        if ((retval = nc_create(file_name.c_str(), NC_NETCDF4|NC_CLOBBER, &ncid)))
		               ERR(retval);

        nc_def_dim(ncid, "weeks", n_weeks, &dimid);
        for (int r = 0; r < (int) n_realizations; ++r) {
	    if ((retval = nc_def_grp(ncid, (string("realization_") + to_string(r)).c_str(), &group_ids[r])))
		             ERR (retval);
	    for (int u = 0; u < (int) n_utilities; ++u) {
	        if ((retval = nc_def_grp(group_ids[r], utility_collectors[u][r]->name, &utilities_group_ids[n_utilities * r + u])))
		                 ERR (retval);	    

                if ((retval = nc_def_var(utilities_group_ids[n_utilities * r + u], "storage", 
						NC_FLOAT, 1, &dimid, &vars_ids[n_utilities * r * n_vars + u])))
                    ERR(retval);
                if ((retval = nc_def_var(utilities_group_ids[n_utilities * r + u], "st_rof", 
						NC_FLOAT, 1, &dimid, &vars_ids[n_utilities * r * n_vars + u + 1])))
                    ERR(retval);
                if ((retval = nc_def_var(utilities_group_ids[n_utilities * r + u], "lt_rof", 
						NC_FLOAT, 1, &dimid, &vars_ids[n_utilities * r * n_vars + u + 2])))
                    ERR(retval);
                if ((retval = nc_def_var(utilities_group_ids[n_utilities * r + u], "res_demand", 
						NC_FLOAT, 1, &dimid, &vars_ids[n_utilities * r * n_vars + u + 3])))
                    ERR(retval);
                if ((retval = nc_def_var(utilities_group_ids[n_utilities * r + u], "cf_size", 
						NC_FLOAT, 1, &dimid, &vars_ids[n_utilities * r * n_vars + u+ 4])))
                    ERR(retval);
                if ((retval = nc_def_var(utilities_group_ids[n_utilities * r + u], "infr_pmts", 
						NC_FLOAT, 1, &dimid, &vars_ids[n_utilities * r * n_vars + u + 5])))
                    ERR(retval);

                if ((retval = nc_def_var_deflate(utilities_group_ids[n_utilities * r + u], 
						vars_ids[n_utilities * r * n_vars + u], 1, 1, DEFLATE_LEVEL_9)))
                    ERR(retval);
                if ((retval = nc_def_var_deflate(utilities_group_ids[n_utilities * r + u], 
						vars_ids[n_utilities * r * n_vars + u + 1], 1, 1, DEFLATE_LEVEL_9)))
                    ERR(retval);
                if ((retval = nc_def_var_deflate(utilities_group_ids[n_utilities * r + u], 
						vars_ids[n_utilities * r * n_vars + u + 2], 1, 1, DEFLATE_LEVEL_9)))
                    ERR(retval);
                if ((retval = nc_def_var_deflate(utilities_group_ids[n_utilities * r + u], 
						vars_ids[n_utilities * r * n_vars + u + 3], 1, 1, DEFLATE_LEVEL_9)))
                    ERR(retval);
                if ((retval = nc_def_var_deflate(utilities_group_ids[n_utilities * r + u], 
						vars_ids[n_utilities * r * n_vars + u + 4], 1, 1, DEFLATE_LEVEL_9)))
                    ERR(retval);
                if ((retval = nc_def_var_deflate(utilities_group_ids[n_utilities * r + u], 
						vars_ids[n_utilities * r * n_vars + u + 5], 1, 1, DEFLATE_LEVEL_9)))
                    ERR(retval);

                if ((retval = nc_put_var_double(utilities_group_ids[n_utilities * r + u], vars_ids[n_utilities * r * n_vars + u],
						utility_collectors[u][r]->getCombined_storage().data())))
	            ERR(retval);
                if ((retval = nc_put_var_double(utilities_group_ids[n_utilities * r + u], vars_ids[n_utilities * r * n_vars + u + 1],
						utility_collectors[u][r]->getSt_rof().data())))
	            ERR(retval);
                if ((retval = nc_put_var_double(utilities_group_ids[n_utilities * r + u], vars_ids[n_utilities * r * n_vars + u + 2],
						utility_collectors[u][r]->getLt_rof().data())))
	            ERR(retval);
                if ((retval = nc_put_var_double(utilities_group_ids[n_utilities * r + u], vars_ids[n_utilities * r * n_vars + u + 3],
						utility_collectors[u][r]->getRestricted_demand().data())))
	            ERR(retval);
                if ((retval = nc_put_var_double(utilities_group_ids[n_utilities * r + u], vars_ids[n_utilities * r * n_vars + u + 4],
						utility_collectors[u][r]->getContingency_fund_size().data())))
	            ERR(retval);
                if ((retval = nc_put_var_double(utilities_group_ids[n_utilities * r + u], vars_ids[n_utilities * r * n_vars + u + 5],
						utility_collectors[u][r]->getDebt_service_payments().data())))
	            ERR(retval);
	    }
	}
	return 0;
    } catch(NcException& e){
        e.what();
        return NC_ERR;
    }
#else
    printf("This version of WaterPaths was not compiled with NetCDF. NetCDF result files will not be printed.\n");
    return 1;
#endif
}

void MasterDataCollector::printPoliciesOutputCompact(
        int week_i, int week_f, string file_name) {
    if (!drought_mitigation_policy_collectors.empty()) {
#pragma omp parallel for
        for (int r = 0; r < (int) drought_mitigation_policy_collectors[0].size();
             ++r) {
            std::ofstream out_stream;
            out_stream.open(output_directory + file_name + "_r"
                            + std::to_string(r) + ".csv");

            string line;
            for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                line += p[r]->printCompactStringHeader();
            line.pop_back();
            out_stream << line << endl;

            for (int w = week_i; w < week_f; ++w) {
                line = "";
                for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                    line += p[r]->printCompactString(w);
                line.pop_back();
                out_stream << line << endl;
            }

            out_stream.close();
        }
    }
}


void MasterDataCollector::printPoliciesOutputTabular(
        int week_i, int week_f, string file_name) {
    if (!drought_mitigation_policy_collectors.empty()) {
#pragma omp parallel for
        for (int r = 0; r < (int) drought_mitigation_policy_collectors[0].size();
             ++r) {
            std::ofstream out_stream;
            out_stream.open(output_directory + file_name + "_r"
                            + std::to_string(r) + ".tab");

            out_stream << "    ";
            for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                out_stream << p[r]->printTabularStringHeaderLine1();
            out_stream << endl;

            out_stream << "Week";
            for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                out_stream << p[r]->printTabularStringHeaderLine2();
            out_stream << endl;

            for (int w = week_i; w < week_f; ++w) {
                out_stream << setw(4) << w;
                for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                    out_stream << p[r]->printTabularString(w);
                out_stream << endl;
            }

            out_stream.close();
        }
    }
}

void MasterDataCollector::printUtilitiesOutputCompact(
        int week_i, int week_f, string file_name) {
#pragma omp parallel for
    for (int r = 0; r < (int) utility_collectors[0].size(); ++r) {
        std::ofstream out_stream;
        out_stream.open(output_directory + file_name + "_r"
                        + std::to_string(r) + ".csv");

        string line;
        for (vector<UtilitiesDataCollector *> &p : utility_collectors)
            line += p[r]->printCompactStringHeader();
        line.pop_back();
        out_stream << line << endl;

        for (int w = week_i; w < week_f; ++w) {
            line = "";
            for (vector<UtilitiesDataCollector *> &p : utility_collectors)
                line += p[r]->printCompactString(w);
            line.pop_back();
            out_stream << line << endl;
        }

        out_stream.close();
    }
}


void MasterDataCollector::printUtilitesOutputTabular(
        int week_i, int week_f, string file_name) {
#pragma omp parallel for
    for (int r = 0; r < (int) utility_collectors[0].size(); ++r) {
        std::ofstream out_stream;
        out_stream.open(output_directory + file_name + "_r"
                        + std::to_string(r) + ".tab");

        stringstream names;
        names << "    ";
        for (vector<UtilitiesDataCollector *> &p : utility_collectors)
            names << setw(p[0]->table_width) << p[r]->name;

        out_stream << names.str();
        out_stream << endl;

        out_stream << "    ";
        for (vector<UtilitiesDataCollector *> &p : utility_collectors)
            out_stream << p[r]->printTabularStringHeaderLine1();
        out_stream << endl;

        out_stream << "Week";
        for (vector<UtilitiesDataCollector *> &p : utility_collectors)
            out_stream << p[r]->printTabularStringHeaderLine2();
        out_stream << endl;

        for (int w = week_i; w < week_f; ++w) {
            out_stream << setw(4) << w;
            for (vector<UtilitiesDataCollector *> &p : utility_collectors)
                out_stream << p[r]->printTabularString(w);
            out_stream << endl;
        }

        out_stream.close();
    }
}

void MasterDataCollector::printWaterSourcesOutputCompact(
        int week_i, int week_f, string file_name) {
#pragma omp parallel for
    for (int r = 0; r < (int) water_source_collectors[0].size(); ++r) {
        try {
            std::ofstream out_stream;
            out_stream.open(output_directory + file_name + "_r"
                            + std::to_string(r) + ".csv");

            string line;
            for (vector<DataCollector *> p : water_source_collectors)
                line += p[r]->printCompactStringHeader();
            line.pop_back();
            out_stream << line << endl;

            for (int w = week_i; w < week_f; ++w) {
                line = "";
                for (vector<DataCollector *> p : water_source_collectors)
                    line += p[r]->printCompactString(w);
                line.pop_back();
                out_stream << line << endl;
            }

            out_stream.close();
        } catch (...) {
            printf("Warning: water sources data for realization %d not saved dur to error.\n", r);
        }
    }
}

void MasterDataCollector::printWaterSourcesOutputTabular(
        int week_i, int week_f, string file_name) {
#pragma omp parallel for
    for (int r = 0; r < (int) water_source_collectors[0].size(); ++r) {
        std::ofstream out_stream;
        out_stream.open(output_directory + file_name + "_r"
                        + std::to_string(r) + ".tab");

        stringstream names;
        names << "    ";
        for (vector<DataCollector *> p : water_source_collectors)
            names << setw(p[0]->table_width) << p[r]->name;

        out_stream << names.str();
        out_stream << endl;

        out_stream << "    ";
        for (vector<DataCollector *> p : water_source_collectors)
            out_stream << p[r]->printTabularStringHeaderLine1();
        out_stream << endl;

        out_stream << "Week";
        for (vector<DataCollector *> p : water_source_collectors)
            out_stream << p[r]->printTabularStringHeaderLine2();
        out_stream << endl;

        for (int w = week_i; w < week_f; ++w) {
            out_stream << setw(4) << w;
            for (vector<DataCollector *> p : water_source_collectors)
                out_stream << p[r]->printTabularString(w);
            out_stream << endl;
        }

        out_stream.close();
    }
}

void MasterDataCollector::printUtilityObjectivesToRowOutStream(vector<UtilitiesDataCollector *> &u,
        std::ofstream &outStream, vector<double> &objectives) {
    /// Create vector with restriction policies pertaining only to the
    /// utility whose objectives are being calculated.
    vector<RestrictionsDataCollector> restrictions;
    for (vector<DataCollector *> p : drought_mitigation_policy_collectors) {
        if (p[0]->type == RESTRICTIONS && p[0]->id == u[0]->id) {
            for (int i = 0; i < (int) p.size(); ++i) {
                restrictions.push_back(
                        dynamic_cast<RestrictionsDataCollector &>(*p[i]));
            }
        }
    }

    /// Reliability
    double reliability = ObjectivesCalculator::calculateReliabilityObjective(u);
    /// Restriction Frequency
    double restriction_freq = ObjectivesCalculator::
    calculateRestrictionFrequencyObjective(restrictions);
    /// Infrastructure NPC
    double inf_npc = ObjectivesCalculator::
    calculateNetPresentCostInfrastructureObjective(u);
    /// Peak Financial Cost
    double financial_cost = ObjectivesCalculator::
    calculatePeakFinancialCostsObjective(u);
    /// Worse Case Costs
    double worse_cost = ObjectivesCalculator::calculateWorseCaseCostsObjective(u);

    outStream << setw(COLUMN_WIDTH) << u[0]->name
              /// Reliability
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << reliability
              /// Restriction Frequency
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << restriction_freq
              /// Infrastructure NPC
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << inf_npc
              /// Peak Financial Cost
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << financial_cost
              /// Worse Case Costs
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << worse_cost
              << endl;

    objectives.push_back(reliability);
    objectives.push_back(restriction_freq);
    objectives.push_back(inf_npc);
    objectives.push_back(financial_cost);
    objectives.push_back(worse_cost);
}

vector<double> MasterDataCollector::calculatePrintObjectives(string file_name, bool print) {
    vector<double> objectives;

    if (print) {
        cout << "Calculating and printing Objectives" << endl;
        string obj_file_path = output_directory + file_name + ".out";
//        cout << obj_file_path << endl;

        std::ofstream outStream;
        outStream.open(obj_file_path);

        outStream << setw(COLUMN_WIDTH) << "      " << setw((COLUMN_WIDTH * 2))
                  << "Reliability"
                  << setw(COLUMN_WIDTH * 2) << "Restriction Freq."
                  //              << setw(COLUMN_WIDTH * 2) << "Jordan Lake Alloc."
                  << setw(COLUMN_WIDTH * 2) << "Infrastructure NPC"
                  << setw(COLUMN_WIDTH * 2) << "Peak Financial Cost"
                  << setw(COLUMN_WIDTH * 2) << "Worse Case Costs" << endl;

        for (auto &u : utility_collectors) {
            printUtilityObjectivesToRowOutStream(u, outStream, objectives);
        }

        outStream.close();

        for (int i = 0; i < (int) objectives.size(); ++i) {
            double o = objectives.at(i);
            if (o > 10e10 || o < -0.1) {
                char error[512];
                sprintf(error, "Objective %d has absurd value of %f. Aborting.\n", i, o);
                throw_with_nested(runtime_error(error));
            }
        }
    } else {
//        cout << "Calculating Objectives" << endl;
        for (auto &u : utility_collectors) {
            /// Create vector with restriction policies pertaining only to the
            /// utility whose objectives are being calculated.
            vector<RestrictionsDataCollector> utility_restrictions;
            for (auto &p : drought_mitigation_policy_collectors)
                if (p[0]->type == RESTRICTIONS && p[0]->id == u[0]->id)
                    for (int i = 0; i < (int) p.size(); ++i) {
                        utility_restrictions.push_back(
                                dynamic_cast<RestrictionsDataCollector &>(*p[i]));
                    }

            objectives.push_back
                    (ObjectivesCalculator::calculateReliabilityObjective(u));
            objectives.push_back
                    (ObjectivesCalculator::calculateRestrictionFrequencyObjective(utility_restrictions));
            objectives.push_back
                    (ObjectivesCalculator::calculateNetPresentCostInfrastructureObjective(u));
            objectives.push_back
                    (ObjectivesCalculator::calculatePeakFinancialCostsObjective(u));
            objectives.push_back
                    (ObjectivesCalculator::calculateWorseCaseCostsObjective(u));
        }
    }
    return objectives;
}

void MasterDataCollector::performBootstrapAnalysis(
		int sol_id, int n_sets, int n_samples, int n_threads, vector<vector<int>> bootstrap_samples) {
    printf("Running bootstrap samples.\n");
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)

    int min = 0;
    int max = (int) n_realizations - 1;
    std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased
    std::ofstream outStream_realizations;
    std::ofstream outStream_objs;
    string line;
    outStream_objs.open(output_directory + "bootstrap_objs_" + to_string(n_sets) + "_" + 
		    to_string(n_samples) + "_S" + to_string(sol_id) + ".csv");

    /// Either read samples from file or create new ones.
    vector<vector<int>> bootstrap_sample_sets(n_sets, vector<int>((unsigned long) n_samples));
    if (bootstrap_samples.size() > 0) {
	    bootstrap_sample_sets = bootstrap_samples;
    } else {
        outStream_realizations.open(output_directory + "bootstrap_realizations_" +
                                    to_string(n_sets) + "_" + to_string(n_samples) + "_S" +
                                    to_string(sol_id) + ".csv");
        for (int set = 0; set < n_sets; ++set) {
            /// Generate one set of bootstrapped realizations, if none was specified.
            line = "";
            for (int &s : bootstrap_sample_sets[set]) {
                s = uni(rng);
                line += to_string(s) + ",";
            }
            line.pop_back();
            outStream_realizations << line << endl;
        }
    }

    vector<vector<double>> objectives(n_sets);
#pragma omp parallel for num_threads(n_threads) shared(objectives)
    for (int set = 0; set < n_sets; ++set) {
        /// Calculate objectives for the set of bootstrapped realizations.
        for (auto &u : utility_collectors) {
            /// Create vector with restriction policies pertaining only to the
            /// utility whose objectives are being calculated.
            vector<RestrictionsDataCollector> utility_restrictions;
            for (auto &p : drought_mitigation_policy_collectors)
                if (p[0]->type == RESTRICTIONS && p[0]->id == u[0]->id)
                    for (int i = 0; i < (int) p.size(); ++i) {
                        utility_restrictions.push_back(
                                dynamic_cast<RestrictionsDataCollector &>(*p[i]));
                    }

	    vector<unsigned long> bootstrap_sample_set = vector<unsigned long>(
			    bootstrap_sample_sets[set].begin(), 
			    bootstrap_sample_sets[set].end());

            objectives[set].push_back
                    (ObjectivesCalculator::calculateReliabilityObjective(u, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculateRestrictionFrequencyObjective(utility_restrictions, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculateNetPresentCostInfrastructureObjective(u, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculatePeakFinancialCostsObjective(u, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculateWorseCaseCostsObjective(u, bootstrap_sample_set));
        }
    }

        /// Print objectives.
    for (int set = 0; set < n_sets; ++set) {
        line = "";
        for (double &o : objectives[set]) {
            line += to_string(o) + ",";
        }
        line.pop_back();
        outStream_objs << line << endl;
    }

    string file_name = output_directory + "objectives_all_reals_" + to_string(n_sets) + 
	    "_" + to_string(n_samples) + "_S" + to_string(sol_id) + ".csv";
    vector<double> objectives_all_reals = calculatePrintObjectives("", false);
    
    line = "";
    for (double &o : objectives_all_reals) {
	    line += to_string(o) + ",";
    }
    line.pop_back();
    
    std::ofstream outStream_objs_all_reals;
    outStream_objs_all_reals.open(file_name);
    outStream_objs_all_reals << line << endl;

    outStream_objs_all_reals.close();
    outStream_realizations.close();
    outStream_objs.close();

}

void MasterDataCollector::printPathways(string file_name) {
    std::ofstream outStream;
    outStream.open(output_directory + file_name + ".out");

    outStream << "Realization\tutility\tweek\tinfra." << endl;

    for (auto &uc : utility_collectors)
        for (int r = 0; r < (int) uc.size(); ++r) {
            for (vector<int> infra : uc[r]->getPathways()) {
                outStream << r << "\t" << infra[0] << "\t" << infra[1] << "\t"
                          << infra[2] << endl;
            }
        }

    outStream.close();
}

void MasterDataCollector::setOutputDirectory(string io_directory) {
    // Check if io_directory is not being set for the same io_directory it is already set. Avoids unnecessary verbose.
    if (io_directory != output_directory) {
        output_directory = io_directory + DEFAULT_OUTPUT_DIR;
        Utils::createDir(output_directory);
        cout << "Output will be printed to folder " << output_directory << endl;
    }
}

DataCollector* MasterDataCollector::createPolicyDataCollector(DroughtMitigationPolicy* dmp, unsigned long r) {
    if (dmp->type == RESTRICTIONS)
        return new RestrictionsDataCollector(dynamic_cast<Restrictions *> (dmp), r);
    else if (dmp->type == TRANSFERS)
        return new TransfersDataCollector(dynamic_cast<Transfers *> (dmp), r);
    else if (dmp->type == INSURANCE_STORAGE_ROF)
        return new EmptyDataCollector();
    else
        throw invalid_argument("Drought mitigation policy not recognized. "
                                 "Did you forget to add it to the "
                                 "MasterDataCollector::addRealization"
                                 " function?");
}

DataCollector* MasterDataCollector::createWaterSourceDataCollector(WaterSource* ws, unsigned long r) {
    if (ws->source_type == RESERVOIR)
        return new ReservoirDataCollector(dynamic_cast<Reservoir *> (ws), r);
    else if (ws->source_type == INTAKE)
        return new IntakeDataCollector(dynamic_cast<Intake *> (ws), r);
    else if (ws->source_type == QUARRY)
        return new QuaryDataCollector(dynamic_cast<Quarry *> (ws), r);
    else if (ws->source_type == WATER_REUSE)
        return new WaterReuseDataCollector(dynamic_cast<WaterReuse *> (ws), r);
    else if (ws->source_type == ALLOCATED_RESERVOIR)
        return new AllocatedReservoirDataCollector(dynamic_cast<AllocatedReservoir *> (ws), r);
    else if (ws->source_type ==
             RESERVOIR_EXPANSION ||
             ws->source_type ==
             NEW_WATER_TREATMENT_PLANT ||
             ws->source_type ==
             SOURCE_RELOCATION)
        return new EmptyDataCollector();
    else
        throw invalid_argument("Water source not recognized. "
                                 "Did you forget to add it to the "
                                 "MasterDataCollector::addRealization"
                                 " function?");
}

void MasterDataCollector::addRealization(
        vector<WaterSource *> water_sources_realization,
        vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization,
        vector<Utility *> utilities_realization,
        unsigned long r) {
    /// If collectors vectors have not yet been initialized, initialize them.
#pragma omp critical
    {
        if (water_source_collectors.empty()) {
            water_source_collectors = vector<vector<DataCollector *>>
                    (water_sources_realization.size(), vector<DataCollector *>(n_realizations));
            drought_mitigation_policy_collectors = vector<vector<DataCollector *>>
                    (drought_mitigation_policies_realization.size(), vector<DataCollector *>(n_realizations));
            utility_collectors = vector<vector<UtilitiesDataCollector *>>
                    (utilities_realization.size(), vector<UtilitiesDataCollector *>(n_realizations));
        }
    };

    /// Create utilities data collectors
    for (int u = 0; u < (int) utilities_realization.size(); ++u) {
        utility_collectors[u][r] = new UtilitiesDataCollector(utilities_realization[u], r);
    }

    /// Create drought mitigation policies data collector
    for (int dmp = 0; dmp < (int) drought_mitigation_policies_realization.size(); ++dmp)
        drought_mitigation_policy_collectors[dmp][r] =
                createPolicyDataCollector(drought_mitigation_policies_realization[dmp], r);

    /// Create water sources data collectors
    for (int ws = 0; ws < (int) water_sources_realization.size(); ++ws) {
        water_source_collectors[ws][r] = createWaterSourceDataCollector(water_sources_realization[ws], r);
    }
} 

void MasterDataCollector::removeRealization(unsigned long r) {
    for (int u = 0; u < (int) utility_collectors.size(); ++u) {
        delete utility_collectors[u][r];
        utility_collectors[u][r] = nullptr;
    }
    for (int dmp = 0; dmp < (int) drought_mitigation_policy_collectors.size(); ++dmp) {
	delete drought_mitigation_policy_collectors[dmp][r];
        drought_mitigation_policy_collectors[dmp][r] = nullptr;
    }
    for (int ws = 0; ws < (int) water_source_collectors.size(); ++ws) {
	delete water_source_collectors[ws][r];
        water_source_collectors[ws][r] = nullptr;
    }
}

//void MasterDataCollector::removeNullptrs(vector<vector<void *>> vector_of_collectors) {
//    for (auto &v : vector_of_collectors) {
//        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
//    }
//}

void MasterDataCollector::cleanCollectorsOfDeletedRealizations() {
//    removeNullptrs(utility_collectors);
//    removeNullptrs(drought_mitigation_policy_collectors);
//    removeNullptrs(water_source_collectors);

    for (auto &v : utility_collectors) {
        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
    }
    for (auto &v : drought_mitigation_policy_collectors) {
        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
    }
    for (auto &v : water_source_collectors) {
        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
    }
}


void MasterDataCollector::collectData(unsigned long r) {
    for (vector<UtilitiesDataCollector *> &uc : utility_collectors)
        uc[r]->collect_data();
    for (vector<DataCollector *> dmp : drought_mitigation_policy_collectors)
        dmp[r]->collect_data();
    for (vector<DataCollector *> ws : water_source_collectors)
        ws[r]->collect_data();
}
