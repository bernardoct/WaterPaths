//
// Created by Bernardo on 11/24/2017.
//

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include "Problem.h"
#include "../../Utils/Utils.h"
#include <omp.h>

Problem::Problem() {
    Reservoir::unsetSeed();
}

Problem::Problem(unsigned long n_weeks, int import_export_rof_table,
                 string system_io, string &rof_tables_directory, int seed,
                 unsigned long n_threads, string bootstrap_file,
                 string &utilities_rdm_file, string &policies_rdm_file,
                 string &water_sources_rdm_file, int n_sets, int n_bs_samples,
                 string &solutions_file, vector<int> &solutions_to_run_range,
                 bool plotting, bool print_obj_row, int n_realizations)
        : n_weeks(n_weeks), import_export_rof_tables(import_export_rof_table),
          seed(seed), n_threads(n_threads), bootstrap_file(bootstrap_file),
          system_io(system_io), n_sets(n_sets), n_bs_samples(n_bs_samples),
          plotting(plotting), print_obj_row(print_obj_row),
          solutions_to_run_range(solutions_to_run_range),
          solutions_file(solutions_file), n_realizations(n_realizations) {}

Problem::~Problem() = default;

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

void Problem::printTimeSeriesAndPathways(bool plot_time_series) {
    // Calculate objective values.
    if (this->master_data_collector != nullptr) {

        // Print output files.
        string fu = "Utilities";
        string fws = "WaterSources";
        string fp = "Policies";
        string fpw = "Pathways";

        //FIXME:PRINT_POLICIES_OUTPUT_TABULAR BLOWING UP MEMORY.
        cout << "Printing Pathways" << endl;
        this->master_data_collector->setOutputDirectory(io_directory);
        this->master_data_collector->printPathways(
                fpw + "_s" + std::to_string(solution_no) + fname_sufix);

        // Only print time series if requested.
        if (plot_time_series) {
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
        }
    } else {
        printf("Trying to print pathways but data collector is empty. Either your simulation crashed or you deleted the data collector too early.\n");
    }
}

void Problem::destroyDataCollector() {
    if (master_data_collector != nullptr) {
        delete master_data_collector;
        master_data_collector = nullptr;
    } else {
        cerr << "Tried to delete nullptr master data collector.\n";
    }
}

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

        for (int u = 0; u < n_utilities; ++u) {
            string file_name = rof_tables_directory + "tables_r" + to_string(r) + "_u" + to_string(u) + ".csv";
            auto tables_utility_week = Utils::parse2DCsvFile(file_name);

            for (int w = 0; w < n_weeks; ++w) {
                rof_tables[r][u].setPartialData(w, tables_utility_week[w].data(), tables_utility_week[w].size());
            }
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
                                              int threads,
                                              const vector<vector<unsigned long>> &bs_realizations) {
    master_data_collector->setOutputDirectory(io_directory);
    master_data_collector->performBootstrapAnalysis(standard_solution, n_sets,
                                                    n_bs_samples, threads,
                                                    bs_realizations);
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

void Problem::readInputData() {}

unsigned long Problem::getNDecVars() const {
    return n_dec_vars;
}

unsigned long Problem::getNObjectives() const {
    return n_objectives;
}
