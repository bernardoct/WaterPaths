#include "SystemComponents/WaterSources/Base/WaterSource.h"
#include "Utils/QPSolver/QuadProg++.h"
#include "Utils/Solutions.h"
#include "Problem/PaperTestProblem.h"
#include "InputFileParser/MasterSystemInputFileParser.h"
#include "Problem/InputFileProblem.h"

#ifdef  PARALLEL
#include <mpi.h>
#endif

#include <algorithm>
#include <getopt.h>
#include <fstream>


using namespace std;
using namespace Constants;
using namespace Solutions;

Problem *problem_ptr;
int failures = 0;

void eval(double *vars, double *objs, double *consts) {
    try {
        for (int i = 0; i < NUM_DEC_VAR; ++i) {
            if (isnan(vars[i])) {
                char error[50];
                sprintf(error, "Nan in decision variable %d", i);
                throw invalid_argument(error);
            }
        }
        failures += problem_ptr->functionEvaluation(vars, objs, consts);
	printf("destroying data collector\n");
        problem_ptr->destroyDataCollector();
	printf("data colletor destroyed!\n");
	for (int i = 0; i < 5; ++i) printf("%d ", objs[i]); printf("\n");
    } catch (...) {
        ofstream sol_out; // for debugging borg
        sol_out << endl;
        sol_out << "Failure! Decision Variable values: " << endl;
        cout << endl;
        cout << "Failure! Decision variable values: " << endl;
        for (int i = 0; i < NUM_DEC_VAR; ++i) sol_out << vars[i] << " ";
        sol_out << endl;
        for (int i = 0; i < NUM_OBJECTIVES; ++i) objs[i] = 1e5;
    }
}

int main(int argc, char *argv[]) {
    int c_num_dec = NON_INITIALIZED;
    int c_num_obj = NON_INITIALIZED;
    int c_num_constr = 0;
    double c_obj[c_num_obj];
    double c_constr[0];

    unsigned long n_realizations = 1000;
    unsigned long n_weeks = 1565;

    string system_io = DEFAULT_DATA_DIR;
    string solution_file;
    string uncertainty_file;
    string bootstrap_file;
    string utilities_rdm_file;
    string policies_rdm_file;
    string water_sources_rdm_file;
    string system_input_file;
    string rof_tables_directory = DEFAULT_ROF_TABLES_DIR;
    int standard_solution = NON_INITIALIZED;
    int n_threads = 2;
    int standard_rdm = 0;
    int first_solution = NON_INITIALIZED;
    int last_solution = NON_INITIALIZED;
    int import_export_rof_table = 0;
    bool verbose = false;
    bool tabular = false;
    bool plotting = true;
    bool run_optimization = false;
    bool print_objs_row = false;
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
    while ((c = getopt(argc, argv,
                       "?s:u:T:r:t:d:f:l:m:v:c:p:b:i:n:o:e:y:S:A:R:U:P:W:I:C:O:B:")) !=
           -1) {
        switch (c) {
            case '?':
                fprintf(stdout,
                        "%s\n"
                        "\t-?: print this error_message\n"
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
                        "\t-S: number of bootstrap samples per set for "
                        "bootstrap analysis.\n"
                        "\t-A: number of sets of bootstrap samples for "
                        "bootstrap analysis.\n"
                        "\t-R: RDM sample number\n"
                        "\t-U: UtilityParser RDM file\n"
                        "\t-P: Policies RDM file\n"
                        "\t-W: Water sources RDM file\n"
                        "\t-I: Problem input file. If -I is passed, all other "
                        "flags are ignored, their contents being specified in "
                        "the input file itself.\n"
                        "\t-O: Directory containing the pre-computed "
                        "ROF table binaries\n"
                        "\t-C: Import/export rof tables (1: export, 0:"
                        " do nothing (standard), -1: import)\n"
                        "\t-B: Export objectives for all utilities on a single "
                        "line",
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
                standard_solution = atoi(optarg);
                break;
            case 'v':
                verbose = static_cast<bool>(atoi(optarg));
                break;
            case 'c':
                tabular = static_cast<bool>(atoi(optarg));
                break;
            case 'p':
                plotting = strncmp(optarg, "true", 4) == 0;
                break;
            case 'b':
                run_optimization = strncmp(optarg, "true", 4) == 0;
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
                system_input_file = optarg;
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

    if (!system_input_file.empty()) {
        problem_ptr = new InputFileProblem(system_input_file);

        dynamic_cast<InputFileProblem *>(problem_ptr)->setRofTablesAndRunParams();
        run_optimization = dynamic_cast<InputFileProblem *>(problem_ptr)->isOptimize();
        nfe = dynamic_cast<InputFileProblem *>(problem_ptr)->getNFunctionEvals();
        output_frequency = dynamic_cast<InputFileProblem *>(problem_ptr)->getRuntimeOutputInterval();
        system_io = dynamic_cast<InputFileProblem *>(problem_ptr)->getOutputDir();
	c_num_dec = (int) dynamic_cast<InputFileProblem *>(problem_ptr)->getNDecVars();
	c_num_obj = (int) dynamic_cast<InputFileProblem *>(problem_ptr)->getNObjectives();
	seed = (int) dynamic_cast<InputFileProblem *>(problem_ptr)->getSeed();
	c_num_constr = 0;

    } else {
        vector<int> solutions_to_run_range;
        if (last_solution != NON_INITIALIZED) {
            solutions_to_run_range = {first_solution, last_solution};
        } else {
            solutions_to_run_range = {standard_solution, standard_solution};
        }

        problem_ptr = new PaperTestProblem(n_weeks, n_realizations,
                                           import_export_rof_table,
                                           system_io, rof_tables_directory,
                                           seed, n_threads,
                                           bootstrap_file, utilities_rdm_file,
                                           policies_rdm_file,
                                           water_sources_rdm_file,
                                           n_sets, n_bs_samples, solution_file,
                                           solutions_to_run_range, plotting,
                                           print_objs_row);
    }

    // If Borg is not called, run in simulation mode
    if (!run_optimization) {
        printf("Running single simulation.\n");
        problem_ptr->runSimulation();

        return 0;
    } else {
#ifdef  PARALLEL

        printf("Running Borg with:\n"
            "nfe: %lu\n"
            "output freq.: %lu\n"
            "n_weeks: %lu\n"
            "n_realizations: %lu\n\n",
            nfe, output_frequency, n_weeks, n_realizations);

        // for debugging borg, creating file to print each ranks DVs which isdone in Eval function

        BORG_Algorithm_ms_startup(&argc, &argv);
        // BORG_Algorithm_ms_islands((int) n_islands);
        // BORG_Algorithm_ms_initialization(INITIALIZATION_LATIN_GLOBAL);
        BORG_Algorithm_ms_max_evaluations((int) nfe);
        BORG_Algorithm_output_frequency((int) output_frequency);

        // Define the problem.
	    printf("\nNUMBER OF OBJECTIVES: %d\n", c_num_obj);
	    printf("\nNUMBER OF DEC VARS: %d\n", c_num_dec);
        BORG_Problem problem = BORG_Problem_create(c_num_dec, c_num_obj,
                                                   c_num_constr,
                                                   eval);
        // Set all the parameter bounds and epsilons
        cout << "setting up problem" << endl;
        problem_ptr->setProblemDefinition(problem);

        if (seed != NON_INITIALIZED) {
	    if (seed < 0) throw invalid_argument("See must be greater than 0.");
            srand(seed);
            WaterSource::setSeed(seed);
        BORG_Random_seed(seed);
        }
        char output_directory[256], output_file_name[256];
        char io_directory[256];
        char runtime[256];
        FILE* outputFile = nullptr;

        sprintf(output_directory, "%s", system_io.c_str());
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

        //int rank; // different seed on each processor
        //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        //BORG_Random_seed(37*seed*(rank+1));

        BORG_Archive result = BORG_Algorithm_ms_run(problem); // this actually runs the optimization
        //BORG_Archive result = BORG_Algorithm_run(problem, nfe);
        // If this is the master node, print out the final archive

        if (result != nullptr) {
            outputFile = fopen(output_file_name, "w");
            cout << "master node print, should see only once" << endl;
            cout << output_file_name << endl;
            if (!outputFile) {
                BORG_Debug("Unable to open final output file\n");
            }
            BORG_Archive_print(result, outputFile);
            BORG_Archive_destroy(result);
            fclose(outputFile);
        }

        printf("Number of failed function evaluations: %d.\n", failures);

        BORG_Algorithm_ms_shutdown();
        BORG_Problem_destroy(problem);
#else
        throw invalid_argument("This version of WaterPaths was not compiled "
                               "with Borg.");
#endif

        return 0;
    }
}
