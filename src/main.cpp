#include "SystemComponents/WaterSources/Reservoir.h"
#include "Utils/Utils.h"
#include "Utils/QPSolver/QuadProg++.h"
#include "Utils/Solutions.h"
#include "Problem/Triangle.h"
// #include "../Borg/borgmm.h"
// #include "../Borg/borgProblemDefinition.h"

#include <getopt.h>
// #include <mpi.h>

#define NUM_OBJECTIVES 6;
#define NUM_DEC_VAR 57;

using namespace std;
using namespace Constants;
using namespace Solutions;

int main(int argc, char *argv[]) {

    const int c_num_dec = NUM_DEC_VAR;
    const int c_num_obj = NUM_OBJECTIVES;
    int c_num_constr = 0;
    double c_obj[c_num_obj * 4];
    double c_constr[0];

    int n_realizations = 1000;
    int n_weeks = 1565;
    char* system_io = const_cast<char *>("./");
    char* solution_file = const_cast<char *>("-1");
    char* uncertainty_file = const_cast<char *>("-1");
    char* bootstrap_file = const_cast<char *>("-1");
    char* rdm_file = const_cast<char *>("-1");
    int standard_solution = 0;
    int standard_rdm = 0;
    int first_solution = -1;
    int last_solution = -1;
    bool verbose = false;
    bool tabular = false;
    bool plotting = true;
    bool run_optimization = false;
    // omp_set_num_threads(1);
    unsigned int n_islands = 2;
    unsigned int nfe = 1000;
    unsigned int output_frequency = 200;
    unsigned int seed = 0;
    vector<int> bootstrap_sample = vector<int>();

    Triangle triangle(standard_solution, standard_rdm);

    int c;
    while ((c = getopt(argc, argv, "?s:u:T:r:t:d:f:l:m:v:c:p:b:i:n:o:e:y:R:")) != -1) {
        switch (c) {
            case '?':
                fprintf(stdout,
                        "%s\n"
                                "\t-?: print this message\n"
                                "\t-s: solutions file (hard coded solutions)\n"
                                "\t-u: uncertain factors file (hard coded values)\n"
                                "\t-T: number of threads (auto)\n"
                                "\t-r: number of realizations (%d)\n"
                                "\t-t: total simulated time in weeks (%d)\n"
                                "\t-d: directory for system I/O (%s)\n"
                                "\t-f: first solution number\n"
                                "\t-l: last solution number\n"
                                "\t-m: individual solution number\n"
                                "\t-v: verbose mode (false)\n"
                                "\t-c: tabular output (really big files, false)\n"
                                "\t-p: plotting output (smaller csv files)\n"
                                "\t-b: run optimization with Borg (false)\n"
                                "\t-i: number of islands if using Borg (2)\n"
                                "\t-n: number of function evaluations if using Borg (1000)\n"
                                "\t-o: output frequency if using Borg (200)\n"
                                "\t-e: seed number (none)\n"
                                "\t-y: file with bootstrap samples\n"
                                "\t-R: RDM file\n",
                        argv[0], n_realizations, n_weeks, system_io);
                return -1;
            case 's': solution_file = optarg; break;
            case 'u': uncertainty_file = optarg; break;
            // case 'T': omp_set_num_threads(atoi(optarg)); break;
            case 'T': break;
            case 'r': n_realizations = atoi(optarg); break;
            case 't': n_weeks = atoi(optarg); break;
            case 'd': system_io = optarg; break;
            case 'f': first_solution = atoi(optarg); break;
            case 'l': last_solution = atoi(optarg); break;
            case 'm': standard_solution = atoi(optarg); break;
            case 'v': verbose = static_cast<bool>(atoi(optarg)); break;
            case 'c': tabular = static_cast<bool>(atoi(optarg)); break;
            case 'p': plotting = static_cast<bool>(atoi(optarg)); break;
            case 'b': run_optimization = true; break;
            case 'i': n_islands = atoi(optarg); break;
            case 'n': nfe = atoi(optarg); break;
            case 'o': output_frequency = atoi(optarg); break;
            case 'e': seed = atoi(optarg); break;
            case 'y': bootstrap_file = optarg; break;
            case 'R': rdm_file = optarg; break;
            default:
                fprintf(stderr, "Unknown option (-%c)\n", c);
                return -1;
        }
    }

    triangle.setN_realizations(n_realizations);
    triangle.setN_weeks(n_weeks);
    triangle.setOutput_directory(system_io);

    if (!run_optimization) {
        if (strlen(bootstrap_file) > 2) {
            vector<double> v = Utils::parse1DCsvFile(bootstrap_file);
            bootstrap_sample = vector<int>(v.begin(), v.end());
        }

        vector<int> sol_range;
        if ((first_solution == -1 && last_solution != -1) ||
            (first_solution != -1 && last_solution == -1))
            __throw_invalid_argument("If you set a first or last solution, you "
                                             "must set the other as well.");

        if (strlen(system_io) == 0) {
            __throw_invalid_argument(
                    "You must specify an input output directory.");
        }

        vector<vector<double>> solutions;
        if (strlen(solution_file) > 2)
            solutions = Utils::parse2DCsvFile(string(system_io) +
                                                      solution_file);
        else
            printf("You must specify a solutions file.\n");

        if (first_solution == -1) {
            cout << endl << endl << endl << "Running solution "
                 << standard_solution << endl;
            triangle.setBootstrap_sample(bootstrap_sample);
            triangle.functionEvaluation(solutions[standard_solution].data(), c_obj, c_constr);
        } else {
            for (int s = first_solution; s < last_solution; ++s) {
                cout << endl << endl << endl << "Running solution "
                     << standard_solution << endl;
                triangle.setSol_number(s);
                triangle.setBootstrap_sample(bootstrap_sample);
                triangle.functionEvaluation(solutions[s].data(), c_obj, c_constr);
            }
        }


        return 0;
    } else {

//        printf("Running Borg with:\n"
//            "n_islands: %d\n"
//            "nfe: %d\n"
//            "output freq.: %d\n"
//            "n_weeks: %d\n"
//            "n_realizations: %d\n\n",
//            n_islands, nfe, output_frequency, n_weeks, n_realizations);

//        cout << "Defining problem" << endl;
/*        BORG_Algorithm_ms_startup(&argc, &argv);
        BORG_Algorithm_ms_islands(n_islands);
        BORG_Algorithm_ms_initialization(INITIALIZATION_LATIN_GLOBAL);
        BORG_Algorithm_ms_max_evaluations(nfe);
        BORG_Algorithm_output_frequency(output_frequency);

        // Define the problem.
        BORG_Problem problem = BORG_Problem_create(c_num_dec, c_num_obj,
                                                   c_num_constr,
                                                   triangle.functionEvaluation);

        // Set all the parameter bounds and epsilons
        setProblemDefinition(problem);

        srand(seed);
        char outputFilename[256];
        char runtime[256];
        FILE* outputFile = nullptr;
        sprintf(outputFilename, "/home/bct52/WaterPaths_mm/output/NC_output_MM_S%d.set", seed);
        // output path (make sure this exists)
        sprintf(runtime, "/home/bct52/WaterPaths_mm/output/NC_runtime_MM_S%d_M%%d.runtime", seed); // runtime
        // path (make sure this exists)

        BORG_Algorithm_output_runtime(runtime);

        // int rank; // different seed on each processor
        // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        // BORG_Random_seed(37*seed*(rank+1));
        BORG_Random_seed(seed);
        BORG_Archive result = BORG_Algorithm_ms_run(problem); // this actually runs the optimization

        // If this is the master node, print out the final archive
        if (result != nullptr) {
            outputFile = fopen(outputFilename, "w");
            if (!outputFile) {
                BORG_Debug("Unable to open final output file\n");
            }
            BORG_Archive_print(result, outputFile);
            BORG_Archive_destroy(result);
            fclose(outputFile);
        }

        BORG_Algorithm_ms_shutdown();
        BORG_Problem_destroy(problem);*/

        return 0;
    }

}
