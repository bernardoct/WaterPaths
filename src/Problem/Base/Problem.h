//
// Created by Bernardo on 11/24/2017.
//

#ifndef TRIANGLEMODEL_PROBLEM_H
#define TRIANGLEMODEL_PROBLEM_H

#include <vector>
#include "../../DataCollector/Base/DataCollector.h"
#include "../../DataCollector/MasterDataCollector.h"
#include "../../Utils/Utils.h"
#include "../../SystemComponents/WaterSources/Reservoir.h"
#ifdef  PARALLEL
#include "../../../Borg/borgms.h"
#endif

class Problem {
protected:
    unsigned long n_realizations;
    unsigned long n_weeks;
    unsigned long solution_no;
    unsigned long n_threads;
    unsigned long n_dec_vars = NON_INITIALIZED;
    unsigned long n_objectives = 5; /// Number of objectives implemented in current version.
    int n_utilities = NON_INITIALIZED;
    string io_directory;
    string fname_sufix;
    string evap_inflows_suffix;
    string rof_tables_directory;

    vector<unsigned long> realizations_to_run;
    MasterDataCollector *master_data_collector = nullptr;
    vector<double> objectives;
    bool print_output_files = true;

    int seed;
    int rdm_no = NON_INITIALIZED;
    int import_export_rof_tables;
    double table_gen_storage_multiplier;
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;
    vector<vector<Matrix2D<double>>> rof_tables;
    vector<vector<unsigned long>> bs_realizations;
    vector<int> solutions_to_run_range;
    string system_io, solutions_file, bootstrap_file;
    int n_sets, n_bs_samples;

    bool plotting, print_obj_row;

    virtual void readInputData();

public:
    explicit Problem();

    Problem(unsigned long n_weeks, int import_export_rof_table,
            string system_io, string &rof_tables_directory, int seed,
            unsigned long n_threads, string bootstrap_file,
            string &utilities_rdm_file, string &policies_rdm_file,
            string &water_sources_rdm_file, int n_sets, int n_bs_samples,
            string &solutions_file, vector<int> &solutions_to_run_range,
            bool plotting, bool print_obj_row, int n_realizations);

    virtual ~Problem();

    virtual int
    functionEvaluation(double *vars, double *objs, double *consts) = 0;

    void setN_weeks(unsigned long n_weeks);

    void setSol_number(unsigned long sol_number);

    void setIODirectory(const string &io_directory);

    const vector<double> &getObjectives() const;

    vector<double> calculateAndPrintObjectives(bool print_files);

    void setN_threads(unsigned long n_threads);

    void setPrint_output_files(bool print_output_files);

    void setN_realizations(unsigned long n_realizations);

    void setRealizationsToRun(vector<unsigned long> &realizations_to_run);

    void setEvap_inflows_suffix(const string &evap_inflows_suffix);

    void setFname_sufix(const string &fname_sufix);

    MasterDataCollector *getMaster_data_collector();

    void destroyDataCollector();

    void printTimeSeriesAndPathways(bool plot_time_series = true);

    void
    setRofTables(unsigned long n_realizations, string rof_tables_directory);

    void setImport_export_rof_tables(int import_export_rof_tables,
                                     string rof_tables_directory);

    void runBootstrapRealizationThinning(int standard_solution, int n_sets,
                                         int n_bs_samples,
                                         int threads,
                                         const vector<vector<unsigned long>> &bs_realizations);

    virtual void runSimulation() = 0;

    virtual unsigned long getNDecVars() const;

    virtual unsigned long getNObjectives() const;

#ifdef PARALLEL
    virtual void setProblemDefinition(BORG_Problem &problem) = 0;
#endif
};

#endif //TRIANGLEMODEL_PROBLEM_H
