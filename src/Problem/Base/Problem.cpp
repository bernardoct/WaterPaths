//
// Created by Bernardo on 11/24/2017.
//

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
//#include <zconf.h>
#include "Problem.h"
#include "../../Utils/Utils.h"
#include <omp.h>

vector<double> Problem::calculateAndPrintObjectives(bool print_files) {
    if (this->master_data_collector != nullptr) {
        if (print_files) {
            this->master_data_collector->setOutputDirectory(io_directory);
        }
        string fo = "Objectives";
        objectives = this->master_data_collector->calculatePrintObjectives(
                fo + "_s" + std::to_string(solution_no) + fname_sufix, print_files);
        return objectives;
    } else {
        objectives = vector<double>(25, 1e5);
        return objectives;
    }
}

void Problem::printTimeSeriesAndPathways() {
    /// Calculate objective values.
    if (this->master_data_collector != nullptr) {
//        this->master_data_collector->setOutputDirectory(io_directory);

        /// Print output files.
        string fu = "Utilities";
        string fws = "WaterSources";
        string fp = "Policies";
        string fpw = "Pathways";

        //FIXME:PRINT_POLICIES_OUTPUT_TABULAR BLOWING UP MEMORY.
        cout << "Printing Pathways" << endl;
        this->master_data_collector->setOutputDirectory(io_directory);
        this->master_data_collector->printPathways(
                fpw + "_s" + std::to_string(solution_no) + fname_sufix);
        cout << "Printing time series" << endl;
        this->master_data_collector->printUtilitiesOutputCompact(
                0, (int) n_weeks, fu + "_s" + std::to_string(solution_no) +
                                  fname_sufix);
        this->master_data_collector->printWaterSourcesOutputCompact(
                0, (int) n_weeks, fws + "_s" + std::to_string(solution_no) +
                                  fname_sufix);
        this->master_data_collector->printPoliciesOutputCompact(
                0, (int) n_weeks, fp + "_s" + std::to_string(solution_no) +
                                  fname_sufix);
        //    data_collector->printUtilitesOutputTabular(0,
        //                                               n_weeks,
        //                                               fu + "_s"
        //                                               + std::to_string(solution_no));
        //    data_collector->printWaterSourcesOutputTabular(0,
        //                                                   n_weeks,
        //                                                   fws + "_s"
        //                                                   + std::to_string(solution_no));
        //    data_collector->printPoliciesOutputTabular(0,
        //                                               n_weeks,
        //                                               fp + "_s"
        //                                               + std::to_string(solution_no));
    } else {
        printf("Trying to print pathways but data collector is empty. Either your simulation crashed or you deleted the data collector too early.\n");
    }

}

vector<int> Problem::vecInfraRankToVecInt(vector<infraRank> v) {
    vector<int> sorted;
    for (infraRank ir : v) {
        sorted.push_back(ir.id);
    }

    return sorted;
}

double Problem::checkAndFixInfraExpansionHighLowOrder(
        vector<int> *order, vector<double> *triggers, int id_low,
        int id_high, double capacity_low, double capacity_high) {

    auto pos_low = distance(order->begin(),
                            find(order->begin(),
                                 order->end(),
                                 id_low));

    auto pos_high = distance(order->begin(),
                             find(order->begin(),
                                  order->end(),
                                  id_high));

    if (pos_high < pos_low) {
        capacity_high += capacity_low;
        order->erase(order->begin() + pos_low);
        triggers->erase(triggers->begin() + pos_low);
    }

    return capacity_high;
}


void Problem::setN_weeks(unsigned long n_weeks) {
    Problem::n_weeks = n_weeks;
}

void Problem::setSol_number(unsigned long sol_number) {
    Problem::solution_no = sol_number;
}

void Problem::setIODirectory(const string &io_directory) {
    this->io_directory = io_directory;
}

void Problem::setRDMOptimization(vector<vector<double>> &utilities_rdm,
                                 vector<vector<double>> &water_sources_rdm,
                                 vector<vector<double>> &policies_rdm) {
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
}

void Problem::setRDMReevaluation(int rdm_no, vector<vector<double>> &utilities_rdm,
                                 vector<vector<double>> &water_sources_rdm,
                                 vector<vector<double>> &policies_rdm) {
    this->rdm_no = rdm_no;
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
}

void Problem::setFname_sufix(const string &fname_sufix) {
    Problem::fname_sufix = fname_sufix;
}

void Problem::setEvap_inflows_suffix(const string &evap_inflows_suffix) {
    Problem::evap_inflows_suffix = evap_inflows_suffix;
}

void Problem::setN_threads(unsigned long n_threads) {
    Problem::n_threads = n_threads;
}

const vector<double> &Problem::getObjectives() const {
    return objectives;
}

void Problem::setPrint_output_files(bool print_output_files) {
    Problem::print_output_files = print_output_files;
}

void Problem::setN_realizations(unsigned long n_realizations) {
    Problem::n_realizations = n_realizations;

    if (realizations_to_run.empty()) {
        realizations_to_run = vector<unsigned long>(n_realizations);
        iota(begin(realizations_to_run), end(realizations_to_run), 0);
    }
}

void Problem::setRealizationsToRun(vector<unsigned long> &realizations_to_run) {
    this->realizations_to_run = realizations_to_run;
}

MasterDataCollector *Problem::getMaster_data_collector() {
    return master_data_collector;
}

Problem::~Problem() {}

void Problem::destroyDataCollector() {
    if (master_data_collector != nullptr) {
        delete master_data_collector;
        master_data_collector = nullptr;
    } else {
        cerr << "Tried to delete nullptr master data collector.\n";
    }
}

Problem::Problem(unsigned long n_weeks) : n_weeks(n_weeks) {
    Reservoir::unsetSeed();
}

///**
// * Read pre-computed ROF tables.
// * @param folder Folder containing the ROF tables.
// * @param n_realizations number of realizations.
// */
//void
//Problem::setRofTables(unsigned long n_realizations, string rof_tables_directory) {
//
//    set<unsigned long> s( realizations_to_run.begin(), realizations_to_run.end() );
//    vector<unsigned long> realizations_to_run_load_tables;
//    realizations_to_run_load_tables.assign( s.begin(), s.end() );
//
//    //double start_time = omp_get_wtime();
//    cout << "Loading ROF tables" << endl;
//    int n_tiers = NO_OF_INSURANCE_STORAGE_TIERS + 1;
//    auto table_data_id = realizations_to_run_load_tables[0];
//
//    /// Get number of weeks in tables
//    string file_name = rof_tables_directory + "/tables_r" + to_string(table_data_id) + "_u0";
//    ifstream in(file_name, ios_base::binary);
//    if (!in.good()) {
//        string error_table_file = "Tables file not found: " + file_name;
//        throw invalid_argument(error_table_file.c_str());
//    }
//
//    //FIXME: FIGURE OUT NUMBER OF UTILITIES BY CHECKING IF TABLES_R0_U0, TABLES_R0_U1, AND SO ON EXIST.
//    char table_file_name[150];
//    for (n_utilities = 0; n_utilities < MAX_NUMBER_OF_UTILITIES; ++n_utilities) {
//        sprintf(table_file_name, "%s%stables_r%lu_u%d",
//                rof_tables_directory.c_str(), BAR.c_str(), table_data_id, n_utilities);
//        ifstream f(table_file_name);
//        if (!f.good()) {
//            if (n_utilities == 0) {
//                char error[200];
//                sprintf(error, "Table %s not found.", table_file_name);
//                throw invalid_argument(error);
//            }
//            break;
//        }
//    }
//
//    unsigned n_weeks_in_table;
//    in.read(reinterpret_cast<char *>(&n_weeks_in_table), sizeof(unsigned));
//
//    // Create empty tables
//    rof_tables = vector<vector<Matrix2D<double>>>(
//            n_realizations,
//            vector<Matrix2D<double>>((unsigned long) n_utilities,
//                                     Matrix2D<double>(n_weeks_in_table / n_tiers, n_tiers)));
//
//    this->rof_tables_directory = rof_tables_directory;
//
//    // Load ROF tables
//    for (auto r : realizations_to_run_load_tables) {
//        for (int u = 0; u < n_utilities; ++u) {
//            file_name = rof_tables_directory + BAR + "tables_r" + to_string(r) + "_u" + to_string(u);
//            ifstream in_file(file_name, ios_base::binary);
//            if (!in.good()) {
//                string error_table_file = "Tables file not found: " + file_name;
//                throw invalid_argument(error_table_file.c_str());
//            }
//
//            // Get table file size from table files.
//            unsigned stringsize;
//            in_file.read(reinterpret_cast<char *>(&stringsize), sizeof(unsigned));
//
//            // Get table information from table files.
//            double data[stringsize];
//            in_file.read(reinterpret_cast<char *>(&data),
//                    stringsize * sizeof(double));
//
//            // Create tables based on table files.
//            rof_tables[r][u].setData(data, (int) stringsize);
//
//            for (unsigned long i = 0; i < stringsize; ++i) {
//                double d = data[i];
//                if (std::isnan(d) || d > 1.01 || d < 0) {
//                    string error_m = "nan or out of [0,1] rof imported "
//                                     "tables. Realization " +
//                                     to_string(r) + "\n";
//                    printf("%s", error_m.c_str());
//                    throw logic_error(error_m.c_str());
//                }
//            }
//
//            in_file.close();
//        }
//    }
//
//    //printf("Loading tables took %f time.\n", omp_get_wtime() - start_time);
//}

/**
 * Read pre-computed ROF tables.
 * @param folder Folder containing the ROF tables.
 * @param n_realizations number of realizations.
 */
void
Problem::setRofTables(unsigned long n_realizations, string rof_tables_directory) {

    double start_time = omp_get_wtime();
    printf("Reading ROF tables.\n");
    string file_name = rof_tables_directory + "tables_r0_u0.csv";
    auto data_r0_u0 = Utils::parse2DCsvFile(file_name);
    auto n_weeks_in_table = (int) data_r0_u0.size();
    auto n_tiers = (int) data_r0_u0.at(0).size();

    if (n_tiers != NO_OF_INSURANCE_STORAGE_TIERS) {
        char error[75];
        sprintf(error, "Number of tiers in tables does not match number of tiers for this problem.");
    }

    n_utilities = 0;
    string fname = rof_tables_directory + "tables_r0_u0.csv";
    fstream f;
    std::ifstream ifile(fname.c_str());
    while ((bool) ifile) {
        n_utilities += 1;
        fname = rof_tables_directory + "tables_r0_u" + to_string(n_utilities) + ".csv";
        ifile = std::ifstream(fname.c_str());
    }

    rof_tables = vector<vector<Matrix2D<double>>>(
            n_realizations,
            vector<Matrix2D<double>>((unsigned long) n_utilities,
                                     Matrix2D<double>(n_weeks_in_table, n_tiers)));

    for (unsigned long r = 0; r < n_realizations; ++r) {
//        printf("Reading tables for realization %lu.\n", r);
        for (int u = 0; u < n_utilities; ++u) {
            string file_name = rof_tables_directory + "tables_r" + to_string(r) + "_u" + to_string(u) + ".csv";
            auto tables_utility_week = Utils::parse2DCsvFile(file_name);

            for (int w = 0; w < n_weeks; ++w) {
                rof_tables[r][u].setPartialData(w, tables_utility_week[w].data(), tables_utility_week[w].size());
            }
//        u = 1;
//        file_name = rof_tables_directory + "tables_r" + to_string(r) + "_u" + to_string(u) + ".csv";
//        tables_utility_week = Utils::parse2DCsvFile(file_name);
//
//        for (int w = 0; w < n_weeks; ++w) {
//            rof_tables[r][u].setPartialData(w, tables_utility_week[w].data(), tables_utility_week[w].size());
//        }
//        u = 2;
//        file_name = rof_tables_directory + "tables_r" + to_string(r) + "_u" + to_string(u) + ".csv";
//        tables_utility_week = Utils::parse2DCsvFile(file_name);
//
//        for (int w = 0; w < n_weeks; ++w) {
//            rof_tables[r][u].setPartialData(w, tables_utility_week[w].data(), tables_utility_week[w].size());
//        }
        }
    }

    printf("Loading tables took %f time.\n", omp_get_wtime() - start_time);
}


void Problem::setImport_export_rof_tables(int import_export_rof_tables, string rof_tables_directory) {
    if (std::abs(import_export_rof_tables) > 1)
        throw invalid_argument("Import/export ROF tables can be assigned as:\n"
                               "-1 - import tables\n"
                               "0 - ignore tables\n"
                               "1 - export tables.\n"
                               "The value entered is invalid.");
    this->import_export_rof_tables = import_export_rof_tables;
    this->rof_tables_directory = rof_tables_directory;

    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        setRofTables(n_realizations, rof_tables_directory);
    } else {
        Utils::createDir(rof_tables_directory);
    }
}

void Problem::runBootstrapRealizationThinning(int standard_solution, int n_sets, int n_bs_samples,
                                              int threads, vector<vector<int>> &realizations_to_run) {
    master_data_collector->setOutputDirectory(io_directory);
    master_data_collector->performBootstrapAnalysis(standard_solution, n_sets, n_bs_samples, threads, realizations_to_run);
}
