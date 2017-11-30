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

    unsigned long n_realizations = 1000;
    unsigned long n_weeks = 1565;
    string system_io = "./";
    string solution_file = "-1";
    string uncertainty_file = "-1";
    string bootstrap_file = "-1";
    string utilities_rdm_file = "-1";
    string policies_rdm_file = "-1";
    string water_sources_rdm_file = "-1";
    string inflows_evap_directory_suffix = "-1";
    unsigned long standard_solution = 0;
    int standard_rdm = 0;
    int first_solution = -1;
    int last_solution = -1;
    bool verbose = false;
    bool tabular = false;
    bool plotting = true;
    bool run_optimization = false;
    // omp_set_num_threads(1);
    unsigned long n_islands = 2;
    unsigned long nfe = 1000;
    unsigned long output_frequency = 200;
    unsigned long seed = 0;
    int rdm_no = -1;
    vector<unsigned long> bootstrap_sample = vector<unsigned long>();
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;

    Triangle triangle(standard_solution, standard_rdm);

    int c;
    while ((c = getopt(argc, argv, "?s:u:T:r:t:d:f:l:m:v:c:p:b:i:n:o:e:y:R:U:P:W:I:")) != -1) {
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
                                "\t-n: number of function evaluations if using Borg (1000)\n"
                                "\t-o: output frequency if using Borg (200)\n"
                                "\t-e: seed number (none)\n"
                                "\t-y: file with bootstrap samples\n"
                                "\t-R: RDM sample number\n"
                                "\t-U: Utilities RDM file\n"
                                "\t-P: Policies RDM file\n"
                                "\t-W: Water sources RDM file\n"
                                "\t-I: Inflows and evaporation folder suffix to be added to \"inflows\" and \"evaporation\" (e.g., _high for inflows_high)\n",
                        argv[0], n_realizations, n_weeks, system_io.c_str());
                return -1;
            case 's': solution_file = optarg; break;
            case 'u': uncertainty_file = optarg; break;
            // case 'T': omp_set_num_threads(atoi(optarg)); break;
            case 'T': break;
            case 'r': n_realizations = (unsigned long)atoi(optarg); break;
            case 't': n_weeks = (unsigned long)atoi(optarg); break;
            case 'd': system_io = optarg; break;
            case 'f': first_solution = atoi(optarg); break;
            case 'l': last_solution = atoi(optarg); break;
            case 'm': standard_solution = (unsigned long) atoi(optarg); break;
            case 'v': verbose = static_cast<bool>(atoi(optarg)); break;
            case 'c': tabular = static_cast<bool>(atoi(optarg)); break;
            case 'p': plotting = static_cast<bool>(atoi(optarg)); break;
            case 'b': run_optimization = true; break;
            case 'i': n_islands = (unsigned long) atoi(optarg); break;
            case 'n': nfe = (unsigned long) atoi(optarg); break;
            case 'o': output_frequency = (unsigned long) atoi(optarg); break;
            case 'e': seed = (unsigned long) atoi(optarg); break;
            case 'y': bootstrap_file = optarg; break;
            case 'R': rdm_no = atoi(optarg); break;
            case 'U': utilities_rdm_file = optarg; break;
            case 'P': policies_rdm_file = optarg; break;
            case 'W': water_sources_rdm_file = optarg; break;
            case 'I': inflows_evap_directory_suffix = optarg; break;
            default:
                fprintf(stderr, "Unknown option (-%c)\n", c);
                return -1;
        }
    }

    /// Set basic realization parameters.
    triangle.setN_realizations(n_realizations);
    triangle.setN_weeks(n_weeks);
    triangle.setOutput_directory(system_io);
    if (strlen(bootstrap_file.c_str()) > 2) {
        vector<double> v = Utils::parse1DCsvFile(system_io + bootstrap_file);
        bootstrap_sample = vector<unsigned long>(v.begin(), v.end());
    }

    if (!run_optimization) {
        vector<int> sol_range;
        if ((first_solution == -1 && last_solution != -1) ||
            (first_solution != -1 && last_solution == -1))
            __throw_invalid_argument("If you set a first or last solution, you "
                                             "must set the other as well.");

        if (strlen(system_io.c_str()) == 0) {
            __throw_invalid_argument(
                    "You must specify an input output directory.");
        }

        vector<vector<double>> solutions;
        if (strlen(solution_file.c_str()) > 2) {
            solutions = Utils::parse2DCsvFile(system_io + solution_file);
        } else {
            printf("You must specify a solutions file.\n");
        }

        if (strlen(inflows_evap_directory_suffix.c_str()) > 2)
            triangle.setEvap_inflows_suffix(inflows_evap_directory_suffix);

        /// Read RDM file, if any
        if (strlen(utilities_rdm_file.c_str()) > 2) {
            if (rdm_no != NON_INITIALIZED) {
                auto utilities_rdm_row = Utils::parse2DCsvFile(system_io + utilities_rdm_file)[rdm_no];
                auto policies_rdm_row = Utils::parse2DCsvFile(system_io + policies_rdm_file)[rdm_no];
                auto water_sources_rdm_row = Utils::parse2DCsvFile(system_io + water_sources_rdm_file)[rdm_no];

                utilities_rdm = std::vector<vector<double>>(n_realizations, utilities_rdm_row);
                policies_rdm = std::vector<vector<double>>(n_realizations, policies_rdm_row);
                water_sources_rdm = std::vector<vector<double>>(n_realizations, water_sources_rdm_row);

                triangle.setRDMReevaluation((unsigned long) rdm_no, utilities_rdm,
                                            water_sources_rdm, policies_rdm);
            } else {
                utilities_rdm = Utils::parse2DCsvFile(system_io + utilities_rdm_file);
                water_sources_rdm = Utils::parse2DCsvFile(system_io + policies_rdm_file);
                policies_rdm = Utils::parse2DCsvFile(system_io + water_sources_rdm_file);
                if (n_realizations > utilities_rdm.size()) {
                    __throw_length_error("If no rdm number is passed, the number of realizations needs to be smaller "
                                                 "or equal to the number of rows in the rdm files.");
                }
                triangle.setRDMReevaluation((unsigned long) rdm_no, utilities_rdm,
                                            water_sources_rdm, policies_rdm);
            }
            if (strlen(inflows_evap_directory_suffix.c_str()) > 2)
                triangle.setFname_sufix("_RDM" + std::to_string(rdm_no) + "_infevap" + inflows_evap_directory_suffix);
            else
                triangle.setFname_sufix("_RDM" + std::to_string(rdm_no));
        }

        if (first_solution == -1) {
            cout << endl << endl << endl << "Running solution "
                 << standard_solution << endl;
            triangle.setSol_number(standard_solution);
            triangle.setBootstrap_sample(bootstrap_sample);
            triangle.functionEvaluation(solutions[standard_solution].data(), c_obj, c_constr);
            triangle.calculateObjectivesAndPrintOutput();
        } else {
            for (int s = first_solution; s < last_solution; ++s) {
                cout << endl << endl << endl << "Running solution "
                     << s << endl;
                triangle.setSol_number((unsigned long) s);
                triangle.setBootstrap_sample(bootstrap_sample);
                triangle.functionEvaluation(solutions[s].data(), c_obj, c_constr);
                triangle.calculateObjectivesAndPrintOutput();
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
