#include "SystemComponents/WaterSources/Base/WaterSource.h"
#include "Utils/QPSolver/QuadProg++.h"
#include "Utils/Solutions.h"
#include "Problem/PaperTestProblem.h"
#include "Problem/Triangle.h"
#include "Utils/Utils.h"

#ifdef  PARALLEL
#include "../Borg/borgms.h"
#include <mpi.h>
#endif

#include <sys/stat.h>
#include <algorithm>
#include <getopt.h>
#include <fstream>
#include <omp.h>


#define NUM_OBJECTIVES 5;
//#define NUM_DEC_VAR 57;
#define NUM_DEC_VAR 27; // infrastructure turned off

using namespace std;
using namespace Constants;
using namespace Solutions;

PaperTestProblem *problem_ptr;
//Triangle *problem_ptr;
int failures = 0;
ofstream sol_out; // for debugging borg

void print_decision_vars(double *vars) {
    int nsols = NUM_DEC_VAR;
    for (int i = 0; i < nsols; ++i) {
        sol_out << vars[i] << ",";
    }
    sol_out << endl;
    cout << "eval\n" << endl;
    sol_out.flush();
}

void eval(double *vars, double *objs, double *consts) {
//    try {
//        print_decision_vars(vars);
        failures += problem_ptr->functionEvaluation(vars, objs, consts);
        //for (int i = 0; i < 57; ++i) printf("%f, ", vars[i]); for (int i = 0; i < 5; ++i) printf("%f, ", objs[i]); printf("\n");
        problem_ptr->destroyDataCollector();
//    } catch (...) {
//        sol_out << endl;
//        sol_out << "Failure! Decision Variable values: " << endl;
//        cout << endl;
//        cout << "Failure! Decision variable values: " << endl;
//        print_decision_vars(vars);
//        sol_out << endl;
//        sol_out << endl;
//    }
}

int main(int argc, char *argv[]) {
    const int c_num_dec = NUM_DEC_VAR;
    const int c_num_obj = NUM_OBJECTIVES;
    int c_num_constr = 0;
    double c_obj[c_num_obj];
    double c_constr[0];

    unsigned long n_realizations = 1000;
    unsigned long n_weeks = 1565;

    string system_io = DEFAULT_DATA_DIR;
    string solution_file = "-1";
    string uncertainty_file = "-1";
    string bootstrap_file = "-1";
    string utilities_rdm_file = "-1";
    string policies_rdm_file = "-1";
    string water_sources_rdm_file = "-1";
    string inflows_evap_directory_suffix = "-1";
    string rof_tables_directory = DEFAULT_ROF_TABLES_DIR;
    unsigned long standard_solution = 0;
    int n_threads;// = omp_get_num_procs();
    int standard_rdm = 0;
    int first_solution = -1;
    int last_solution = -1;
    int import_export_rof_table = 0;
    bool verbose = false;
    bool tabular = false;
    bool plotting = true;
    bool run_optimization = false;
    bool print_objs_row = false;
    // omp_set_num_threads(1);
    unsigned long n_islands = 2;
    unsigned long nfe = 1000;
    unsigned long output_frequency = 200;
    int seed = -1;
    int rdm_no = -1;
    int n_sets = -1;
    int n_bs_samples = -1;
    vector<vector<int>> realizations_to_run;
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;

    int c;
    while ((c = getopt(argc, argv, "?s:u:T:r:t:d:f:l:m:v:c:p:b:i:n:o:e:y:S:A:R:U:P:W:I:C:O:B:")) != -1) {
        switch (c) {
            case '?':
                fprintf(stdout,
                        "%s\n"
                        "\t-?: print this message\n"
                        "\t-s: solutions file (hard coded solutions)\n"
                        "\t-u: uncertain factors file (hard coded values)\n"
                        "\t-T: number of threads (auto)\n"
                        "\t-r: number of realizations (%lu)\n"
                        "\t-t: total simulated time in weeks (%lu)\n"
                        "\t-d: directory for system I/O (%s)\n"
                        "\t-f: first solution number\n"
                        "\t-l: last solution number\n"
                        "\t-m: individual solution number\n"
                        "\t-v: verbose mode (false)\n"
                        "\t-c: tabular output (really big files, false)\n"
                        "\t-p: plotting output (smaller csv files)\n"
                        "\t-b: run optimization with Borg (false)\n"
                        "\t-i: number of islands if using Borg (2)\n"
                        "\t-n: number of function evaluations if using"
                        " Borg (1000)\n"
                        "\t-o: output frequency if using Borg (200)\n"
                        "\t-e: seed number (none)\n"
                        "\t-y: file with bootstrap samples\n"
                        "\t-S: number of bootstrap samples per set for bootstrap analysis.\n"
                        "\t-A: number of sets of bootstrap samples for bootstrap analysis.\n"
                        "\t-R: RDM sample number\n"
                        "\t-U: Utility RDM file\n"
                        "\t-P: Policies RDM file\n"
                        "\t-W: Water sources RDM file\n"
                        "\t-I: Inflows and evaporation folder suffix to"
                        " be added to \"inflows\" and "
                        "\"evaporation\" (e.g., _high for "
                        "inflows_high)\n"
                        "\t-O: Directory containing the pre-computed "
                        "ROF table binaries\n"
                        "\t-C: Import/export rof tables (1: export, 0:"
                        " do nothing (standard), -1: import)\n"
                        "\t-B: Export objectives for all utilities on a single line",
                        argv[0], n_realizations, n_weeks, system_io.c_str());
                return -1;
            case 's':
                solution_file = optarg;
                break;
            case 'u':
                uncertainty_file = optarg;
                break;
            case 'T':
                n_threads = atoi(optarg);
                break;
            case 'r':
                n_realizations = (unsigned long) atoi(optarg);
                break;
            case 't':
                n_weeks = (unsigned long) atoi(optarg);
                break;
            case 'd':
                system_io = optarg;
                break;
            case 'f':
                first_solution = atoi(optarg);
                break;
            case 'l':
                last_solution = atoi(optarg);
                break;
            case 'm':
                standard_solution = (unsigned long) atoi(optarg);
                break;
            case 'v':
                verbose = static_cast<bool>(atoi(optarg));
                break;
            case 'c':
                tabular = static_cast<bool>(atoi(optarg));
                break;
            case 'p':
                plotting = static_cast<bool>(atoi(optarg));
                break;
            case 'b':
                run_optimization = true;
                break;
            case 'i':
                n_islands = (unsigned long) atoi(optarg);
                break;
            case 'n':
                nfe = (unsigned long) atoi(optarg);
                break;
            case 'o':
                output_frequency = (unsigned long) atoi(optarg);
                break;
            case 'e':
                seed = atoi(optarg);
                break;
            case 'y':
                bootstrap_file = optarg;
                break;
            case 'R':
                rdm_no = atoi(optarg);
                break;
            case 'U':
                utilities_rdm_file = optarg;
                break;
            case 'P':
                policies_rdm_file = optarg;
                break;
            case 'W':
                water_sources_rdm_file = optarg;
                break;
            case 'I':
                inflows_evap_directory_suffix = optarg;
                break;
            case 'C':
                import_export_rof_table = atoi(optarg);
                break;
            case 'S':
                n_bs_samples = atoi(optarg);
                break;
            case 'A':
                n_sets = atoi(optarg);
                break;
            case 'O':
                rof_tables_directory = optarg;
                break;
            default:
                fprintf(stderr, "Unknown option (-%c)\n", c);
                return -1;
        }
    }

    PaperTestProblem problem(n_weeks, import_export_rof_table);
//    Triangle problem(n_weeks, import_export_rof_table);
    if (seed > -1) {
        WaterSource::setSeed(seed);
        MasterDataCollector::setSeed(seed);
    }

    // Set basic realization parameters.
    problem.setN_weeks(n_weeks);
//    printf("%s\n", system_io.c_str());
    problem.setIODirectory(system_io);
    problem.setN_threads((unsigned long) n_threads);

    // Load bootstrap samples if necessary.
    if (strlen(bootstrap_file.c_str()) > 2) {
        auto bootstrap_samples_double = Utils::parse2DCsvFile(system_io + bootstrap_file);
        for (auto &v : bootstrap_samples_double) {
            realizations_to_run.push_back(vector<int>(v.begin(), v.end()));
            if (*std::max_element(v.begin(), v.end()) >= n_realizations)
                throw invalid_argument(
                        "Number of realizations must be higher than the ID of all realizations in the bootstrap samples file.");

        }
        problem.setPrint_output_files(false);
    }

    if (!run_optimization && strlen(system_io.c_str()) == 0) {
        throw invalid_argument(
                "You must specify an input output directory.");
    }

    // Set up input/output suffix, if necessary.
    if (strlen(inflows_evap_directory_suffix.c_str()) > 2) {
        problem.setEvap_inflows_suffix(inflows_evap_directory_suffix);
    }
    // Read RDM file, if any
    if (strlen(utilities_rdm_file.c_str()) > 2) {
        cout << "reading RDM file" << endl;
        if (rdm_no != NON_INITIALIZED) {
            auto utilities_rdm_row = Utils::parse2DCsvFile(system_io + utilities_rdm_file)[rdm_no];
            auto policies_rdm_row = Utils::parse2DCsvFile(system_io + policies_rdm_file)[rdm_no];
            auto water_sources_rdm_row = Utils::parse2DCsvFile(system_io + water_sources_rdm_file)[rdm_no];

            utilities_rdm = std::vector<vector<double>>(n_realizations, utilities_rdm_row);
            policies_rdm = std::vector<vector<double>>(n_realizations, policies_rdm_row);
            water_sources_rdm = std::vector<vector<double>>(n_realizations, water_sources_rdm_row);

            problem.setRDMReevaluation((unsigned long) rdm_no, utilities_rdm,
                                       water_sources_rdm, policies_rdm);

            if (strlen(inflows_evap_directory_suffix.c_str()) > 2)
                problem.setFname_sufix("_RDM" + std::to_string(rdm_no) +
                                       "_infevap" + inflows_evap_directory_suffix);
            else
                problem.setFname_sufix("_RDM" + std::to_string(rdm_no));
        } else {
            utilities_rdm = Utils::parse2DCsvFile(system_io + utilities_rdm_file);
            water_sources_rdm = Utils::parse2DCsvFile(system_io + water_sources_rdm_file);
            policies_rdm = Utils::parse2DCsvFile(system_io + policies_rdm_file);
            if (n_realizations > utilities_rdm.size()) {
                throw length_error("If no rdm number is passed, the number of realizations needs to be smaller "
                                   "or equal to the number of rows in the rdm files.");
            }
            problem.setRDMReevaluation((unsigned long) rdm_no, utilities_rdm,
                                       water_sources_rdm, policies_rdm);
        }
    }
    problem_ptr = &problem;

    // Set realizations to be run -- otherwise, n_realizations realizations will be run.
    if (!realizations_to_run.empty() && (n_sets <= 0 || n_bs_samples <= 0)) {
        auto realizations_to_run_ul = vector<unsigned long>(realizations_to_run[0].begin(),
                                                            realizations_to_run[0].end());
        problem.setRealizationsToRun(realizations_to_run_ul);
        problem.setN_realizations(*max_element(realizations_to_run_ul.begin(), realizations_to_run_ul.end()) + 1);
    } else {
        problem.setN_realizations(n_realizations);
    }
    problem.setImport_export_rof_tables(import_export_rof_table, (int) n_weeks,
            system_io + rof_tables_directory);
    problem.readInputData();

    // If Borg is not called, run in simulation mode
    if (!run_optimization) {
        vector<int> sol_range;
        // Check for basic input errors.
        if ((first_solution == -1 && last_solution != -1) ||
            (first_solution != -1 && last_solution == -1))
            throw invalid_argument("If you set a first or last solution, you "
                                   "must set the other as well.");

        vector<vector<double>> solutions;
        if (strlen(solution_file.c_str()) > 2) {
            solutions = Utils::parse2DCsvFile(system_io + solution_file);
            if (standard_solution >= solutions.size())
                throw invalid_argument("Number of solutions in file <= solution ID.\n");
        } else {
            throw invalid_argument("You must specify a solutions file.\n");
        }

        // Run model
        if (first_solution == -1) {
            cout << endl << endl << endl << "Running solution "
                 << standard_solution << endl;
            problem.setSol_number(standard_solution);
            problem_ptr->functionEvaluation(solutions[standard_solution].data(), c_obj, c_constr);

            // Export pathways and objectives, otherwise, if required, run bootstrap sub-sampling.
            if (n_sets > 0 && n_bs_samples > 0) {
                problem_ptr->runBootstrapRealizationThinning(
                        (int) standard_solution, n_sets, n_bs_samples, n_threads, realizations_to_run);
            } else if (import_export_rof_table != EXPORT_ROF_TABLES) {
                if (plotting)
                    problem.printTimeSeriesAndPathways();
                auto objectives = problem_ptr->calculateAndPrintObjectives(!print_objs_row);
                //            trianglePtr->getMaster_data_collector()->printNETCDFUtilities("netcdf_output");
            }

            problem_ptr->destroyDataCollector();
        } else {
            double time_0 = omp_get_wtime();
            ofstream objs_file;
            string file_name = system_io + "output" + BAR + "Objectives_RDM" + to_string(rdm_no) +
                               "_sols" + to_string(first_solution) + "_to_" + to_string(last_solution) + ".csv";
            objs_file.open(file_name);
            printf("Objectives file will be printed at %s.\n", file_name.c_str());
            for (int s = first_solution; s < last_solution; ++s) {
                cout << endl << endl << endl << "Running solution "
                     << s << endl;
                problem.setSol_number((unsigned long) s);
                problem_ptr->functionEvaluation(solutions[s].data(), c_obj, c_constr);
                vector<double> objectives = problem_ptr->calculateAndPrintObjectives(false);
                if (plotting)
                    problem.printTimeSeriesAndPathways();
                problem_ptr->destroyDataCollector();
                string line;
                for (double &o : objectives) {
                    line += to_string(o) + ",";
                }
                line.pop_back();
                objs_file << line << endl;
            }
            objs_file.close();
            printf("Time to simulate %d solutions: %f s", last_solution - first_solution, omp_get_wtime() - time_0);
        }

        return 0;
    } else {
#ifdef  PARALLEL

        printf("Running Borg with:\n"
            "n_islands: %lu\n"
            "nfe: %lu\n"
            "output freq.: %lu\n"
            "n_weeks: %lu\n"
            "n_realizations: %lu\n\n",
            n_islands, nfe, output_frequency, n_weeks, n_realizations);
         
        // for debugging borg, creating file to print each ranks DVs which isdone in Eval function   
        
        BORG_Algorithm_ms_startup(&argc, &argv);
        // BORG_Algorithm_ms_islands((int) n_islands);
        // BORG_Algorithm_ms_initialization(INITIALIZATION_LATIN_GLOBAL);
        BORG_Algorithm_ms_max_evaluations((int) nfe);
        BORG_Algorithm_output_frequency((int) output_frequency);

        // Define the problem.
        BORG_Problem problem = BORG_Problem_create(c_num_dec, c_num_obj,
                                                   c_num_constr,
                                                   eval);
        // Set all the parameter bounds and epsilons
        cout << "setting up problem" << endl;
        problem_ptr->setProblemDefinition(problem);

        if (seed > -1) {
            srand(seed);
            WaterSource::setSeed(seed);
	    BORG_Random_seed(seed);
        }
        char output_directory[256], output_file_name[256];
	char io_directory[256];
        char runtime[256];
        FILE* outputFile = nullptr;

	sprintf(output_directory, "%s%s", system_io.c_str(), DEFAULT_OUTPUT_DIR.c_str());
	//Utils::createDir(string(output_directory));

        // sprintf(output_file_name, "%sNC_output_MM_S%d_N%lu.set", output_directory, seed, nfe);
        sprintf(output_file_name, "%sNC_output_MS_S%d_N%lu.set", output_directory, seed, nfe);
        printf("Reference set will be in %s.\n", output_file_name);
        // output path (make sure this exists)
        // sprintf(runtime, "%sNC_runtime_MM_S%d_N%lu_M%%d.runtime", output_directory,
        sprintf(runtime, "%sNC_runtime_MS_S%d_N%lu.runtime", output_directory,
                seed, nfe); // runtime
        printf("Runtime files will be in %s.\n", runtime);
        // path (make sure this exists)

        BORG_Algorithm_output_runtime(runtime);

        int rank; // different seed on each processor
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        string rank_out_file = "diagnostic_output/DVs_rank_" + to_string(rank) + ".csv";
        sol_out.open(rank_out_file.c_str());

        //int rank; // different seed on each processor
        //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        //BORG_Random_seed(37*seed*(rank+1));
        
        BORG_Archive result = BORG_Algorithm_ms_run(problem); // this actually runs the optimization
        //BORG_Archive result = BORG_Algorithm_run(problem, nfe);
        // If this is the master node, print out the final archive

        if (result != nullptr) {
            outputFile = fopen(output_file_name, "w");
            cout << "master node print, should see only once" << endl;
            if (!outputFile) {
                BORG_Debug("Unable to open final output file\n");
            }
            BORG_Archive_print(result, outputFile);
            BORG_Archive_destroy(result);
            sol_out.close();
            fclose(outputFile);
        }

        printf("Number of failed function evaluations: %d.\n", failures);

        BORG_Algorithm_ms_shutdown();
        BORG_Problem_destroy(problem);
#else
        throw invalid_argument("This version of WaterPaths was not compiled with Borg.");
#endif

        return 0;
    }
}
