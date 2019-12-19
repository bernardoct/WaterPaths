//
// Created by Bernardo on 12/4/2019.
//

#include <omp.h>
#include <sys/stat.h>
// for windows mkdir
#ifdef _WIN32
#include <direct.h>
#endif

#include "InputFileProblem.h"
#include "../Simulation/Simulation.h"

#ifdef PARALLEL
#include <mpi.h>

void InputFileProblem::setProblemDefinition(BORG_Problem &problem)
{
    // The parameter bounds are the same for all formulations
    auto dec_vars_bounds = parser.getDecVarsBounds();
    auto objs_epsilons = parser.getObjsEpsilons();
    if (dec_vars_bounds.empty() || objs_epsilons.empty()) {
        throw invalid_argument("To perform an optimization run, the bounds for "
                               "the decision variables and objectives epsilons "
                               "must be specified in the input file.");
    }

    for (int i = 0; i < dec_vars_bounds.size(); ++i) {
        BORG_Problem_set_bounds(problem, i, dec_vars_bounds[i][0], dec_vars_bounds[i][1]);
    }
    for (int j = 0; j < objs_epsilons.size(); ++j) {
        BORG_Problem_set_epsilon(problem, j, objs_epsilons[j]);
    }
}
#endif

InputFileProblem::InputFileProblem(string &system_input_file) : Problem() {
    parser.preloadAndCheckInputFile(system_input_file);
}


int InputFileProblem::functionEvaluation(double *vars, double *objs,
                                         double *consts) {

    Simulation *s;

    parser.createSystemObjects(vars);
    realizations_to_run = parser.getRealizationsToRun();
    n_realizations = parser.getNRealizations();
    n_threads = parser.getNThreads();
    n_weeks = parser.getNWeeks();
    solutions_file = parser.getSolutionsFile();
    solutions_to_run = parser.getSolutionsToRun();
    solutions_decvars = parser.getSolutionsDecvars();

    setImport_export_rof_tables(parser.getUseRofTables(),
                                parser.getRofTablesDir());


    // Creates simulation object depending on use (or lack thereof) ROF tables
    printf("Starting Simulation\n");
    double start_time = omp_get_wtime();
    if (import_export_rof_tables == EXPORT_ROF_TABLES) {
        s = new Simulation(parser.getWaterSources(),
                            parser.getWaterSourcesGraph(),
                            parser.getReservoirUtilityConnectivityMatrix(),
                            parser.getUtilities(),
                            parser.getDroughtMitigationPolicy(),
                            parser.getReservoirControlRules(),
                            parser.getRdmUtilities(),
                            parser.getRdmWaterSources(),
                            parser.getRdmDmp(),
                            parser.getNWeeks(),
                            parser.getRealizationsToRun(),
                            parser.getRofTablesDir());
        this->master_data_collector = s->runFullSimulation(n_threads, vars);
    } else if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        s = new Simulation(parser.getWaterSources(),
                            parser.getWaterSourcesGraph(),
                            parser.getReservoirUtilityConnectivityMatrix(),
                            parser.getUtilities(),
                            parser.getDroughtMitigationPolicy(),
                            parser.getReservoirControlRules(),
                            parser.getRdmUtilities(),
                            parser.getRdmWaterSources(),
                            parser.getRdmDmp(),
                            parser.getNWeeks(),
                            parser.getRealizationsToRun(),
                            rof_tables,
                            parser.getTableStorageShift(),
                            rof_tables_directory);
        this->master_data_collector = s->runFullSimulation(n_threads, vars);
    } else {
        s = new Simulation(parser.getWaterSources(),
                            parser.getWaterSourcesGraph(),
                            parser.getReservoirUtilityConnectivityMatrix(),
                            parser.getUtilities(),
                            parser.getDroughtMitigationPolicy(),
                            parser.getReservoirControlRules(),
                            parser.getRdmUtilities(),
                            parser.getRdmWaterSources(),
                            parser.getRdmDmp(),
                            parser.getNWeeks(),
                            parser.getRealizationsToRun());
        this->master_data_collector = s->runFullSimulation(n_threads, vars);
    }
    double end_time = omp_get_wtime();
    printf("Function evaluation time: %f\n", end_time - start_time);

    // Calculate objectives and store them in Borg decision variables array.
#ifdef  PARALLEL
    if (import_export_rof_tables != EXPORT_ROF_TABLES) {
        objectives = calculateAndPrintObjectives(false);

        int i = 0;
        objs[i] = 1. - min(min(objectives[i], objectives[5 + i]),
                   objectives[10 + i]);
        for (i = 1; i < 5; ++i) {
            objs[i] = max(max(objectives[i], objectives[5 + i]),
                            objectives[10 + i]);
        }

    for (int i = 0; i < NUM_OBJECTIVES; ++i) {
         if (isnan(objs[i])) {
        for (int j = 0; j < NUM_OBJECTIVES; ++j) {
            objs[i] = 1e5;
        }
        break;
        }
    }
    }

    if (s != nullptr) {
        delete s;
    }
    s = nullptr;
#endif
//    } catch (const std::exception& e) {
//        simulationExceptionHander(e, s, objs, vars);
//	return 1;
//    }

    return 0;
}

const vector<vector<double>> &InputFileProblem::getSolutionsDecvars() const {
    return parser.getSolutionsDecvars();
}

void InputFileProblem::runSimulation() {
    auto solutions_dec_vars = parser.getSolutionsDecvars();
    n_sets = parser.getNBootstrapSamples();
    n_bs_samples = parser.getBootstrapSampleSize();
    plotting = parser.isPrintTimeSeries();

    if (solutions_dec_vars.size() > 1) {
        ofstream objs_file = createOutputFile();
        for (auto dvs : solutions_dec_vars) {
            functionEvaluation(dvs.data(), nullptr, nullptr);
            vector<double> objectives = calculateAndPrintObjectives(false);
            printTimeSeriesAndPathways(plotting);
            destroyDataCollector();
            printObjsInLineInFile(objs_file, objectives);

            if (n_sets != NON_INITIALIZED && n_bs_samples != NON_INITIALIZED) {
                auto sols = parser.getSolutionsToRun();
                runBootstrapRealizationThinning(
                        (sols.empty() ? -1 : (int) sols[0]), n_sets, n_bs_samples,
                        (int) n_threads, bs_realizations);
            }
        }
        objs_file.close();
    } else {
        if (solutions_dec_vars.size() == 1) {
            functionEvaluation(solutions_dec_vars[0].data(), nullptr, nullptr);
        } else {
            functionEvaluation(nullptr, nullptr, nullptr);
        }
        calculateAndPrintObjectives(true);
        printTimeSeriesAndPathways(plotting);
        if (n_sets != NON_INITIALIZED && n_bs_samples != NON_INITIALIZED) {
            auto sols = parser.getSolutionsToRun();
            runBootstrapRealizationThinning(
                    (sols.empty() ? -1 : (int) sols[0]), n_sets, n_bs_samples,
                    (int) n_threads, bs_realizations);
        }
        destroyDataCollector();
    }
}

ofstream InputFileProblem::createOutputFile() const {
    string output_dir = system_io + "output" + BAR;
    if (mkdir(output_dir.c_str(), 700) != 0) {
        ofstream objs_file;
        string file_name = system_io + "output" + BAR + "Objectives" +
                           (rdm_no == NON_INITIALIZED ? "" : "_RDM" +
                                                             to_string(
                                                                     rdm_no)) +
                           "_sols" + to_string(solutions_to_run_range[0]) +
                           "_to_" + to_string(solutions_to_run_range[1]) +
                           ".csv";
        objs_file.open(file_name);
        printf("Objectives will be printed to file %s\n",
               file_name.c_str());
        return objs_file;
    } else {
        throw runtime_error("Cannot create directory " + output_dir);
    }
}

void InputFileProblem::printObjsInLineInFile(ofstream &objs_file,
                                             const vector<double> &objectives) const {
    string line;
    for (const double &o : objectives) {
        line += to_string(o) + ",";
    }
    line.pop_back();
    objs_file << line << endl;
}

bool InputFileProblem::isOptimize() const {
    return parser.isOptimize();
}

int InputFileProblem::getNFunctionEvals() const {
    return parser.getNFunctionEvals();
}

int InputFileProblem::getRuntimeOutputInterval() const {
    return parser.getRuntimeOutputInterval();
}
