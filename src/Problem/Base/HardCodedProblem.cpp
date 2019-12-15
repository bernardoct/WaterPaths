//
// Created by Bernardo on 12/8/2019.
//

#include <algorithm>
#include <omp.h>
#include "HardCodedProblem.h"

HardCodedProblem::HardCodedProblem(unsigned long n_weeks, int n_realizations,
                                   int import_export_rof_table,
                                   string system_io, string &rof_tables_directory, int seed,
                                   unsigned long n_threads,
                                   string bootstrap_file,
                                   string &utilities_rdm_file,
                                   string &policies_rdm_file,
                                   string &water_sources_rdm_file, int n_sets,
                                   int n_bs_samples, string &solutions_file,
                                   vector<int> &solutions_to_run_range,
                                   bool plotting, bool print_obj_row)
        : Problem(n_weeks, import_export_rof_table, system_io, rof_tables_directory,
                seed, n_threads, bootstrap_file, utilities_rdm_file,
                policies_rdm_file, water_sources_rdm_file, n_sets, n_bs_samples,
                solutions_file, solutions_to_run_range, plotting, print_obj_row,
                n_realizations) {

    setN_threads(n_threads);
    setIODirectory(system_io);
    setN_weeks(n_weeks);

    // Load bootstrap samples if necessary.
    if (strlen(bootstrap_file.c_str()) > 2) {
        auto bootstrap_samples_double = Utils::parse2DCsvFile(
                system_io + bootstrap_file);
        for (auto &v : bootstrap_samples_double) {
            this->bs_realizations.emplace_back(v.begin(), v.end());
            if (*std::max_element(v.begin(), v.end()) >=
                (double) n_realizations)
                throw invalid_argument(
                        "Number of realizations must be higher than the ID of "
                        "all realizations in the bootstrap samples file.");
        }
        setPrint_output_files(false);
    }

    // Read RDM file, if any
    if (strlen(utilities_rdm_file.c_str()) > 2) {
        cout << "reading RDM file" << endl;
        if (rdm_no != NON_INITIALIZED) {
            auto utilities_rdm_row = Utils::parse2DCsvFile(
                    system_io + utilities_rdm_file)[rdm_no];
            auto policies_rdm_row = Utils::parse2DCsvFile(
                    system_io + policies_rdm_file)[rdm_no];
            auto water_sources_rdm_row = Utils::parse2DCsvFile(
                    system_io + water_sources_rdm_file)[rdm_no];

            utilities_rdm = std::vector<vector<double>>(n_realizations,
                                                        utilities_rdm_row);
            policies_rdm = std::vector<vector<double>>(n_realizations,
                                                       policies_rdm_row);
            water_sources_rdm = std::vector<vector<double>>(n_realizations,
                                                            water_sources_rdm_row);

            setFname_sufix("_RDM" + std::to_string(rdm_no));
        } else {
            utilities_rdm = Utils::parse2DCsvFile(
                    system_io + utilities_rdm_file);
            water_sources_rdm = Utils::parse2DCsvFile(
                    system_io + water_sources_rdm_file);
            policies_rdm = Utils::parse2DCsvFile(
                    system_io + policies_rdm_file);
            if (n_realizations > utilities_rdm.size()) {
                throw length_error(
                        "If no rdm number is passed, the number of realizations"
                        " needs to be smaller or equal to the number of rows in"
                        " the rdm files.");
            }
            setRDMReevaluation(rdm_no, utilities_rdm,
                               water_sources_rdm, policies_rdm);
        }
    }

    if (import_export_rof_table == EXPORT_ROF_TABLES) {
        table_gen_storage_multiplier = BASE_STORAGE_CAPACITY_MULTIPLIER;
    } else {
        table_gen_storage_multiplier = 1.;
    }

    if (seed > -1) {
        WaterSource::setSeed(seed);
        MasterDataCollector::setSeed(seed);
    }

    // Set realizations to be run -- otherwise, n_realizations realizations will be run.
    if (!realizations_to_run.empty() &&
        (n_sets <= 0 || n_bs_samples <= 0)) {
        auto realizations_to_run_ul = vector<unsigned long>(
                realizations_to_run.begin(),
                realizations_to_run.end());
        setRealizationsToRun(realizations_to_run_ul);
        setN_realizations(
                *max_element(realizations_to_run_ul.begin(),
                             realizations_to_run_ul.end()) +
                1);
    } else {
        setN_realizations(n_realizations);
    }
    setImport_export_rof_tables(import_export_rof_table,
                                system_io + rof_tables_directory);
}

void HardCodedProblem::runSimulation() {

    // Check for basic input errors.
    if ((solutions_to_run_range[0] == -1 && solutions_to_run_range[1] != -1) ||
        (solutions_to_run_range[0] != -1 && solutions_to_run_range[1] == -1))
        throw invalid_argument("If you set a first or last solution, you "
                               "must set the other as well.");

    vector<vector<double>> solutions;
    if (strlen(solutions_file.c_str()) > 2) {
        solutions = Utils::parse2DCsvFile(system_io + solutions_file);
        if (solutions_to_run_range[0] >= solutions.size())
            throw invalid_argument("Number of solutions in file "
                                   "<= solution ID.\n");
    } else {
        throw invalid_argument("You must specify a solutions file.\n");
    }

    // Run model
    if (solutions_to_run_range[0] == solutions_to_run_range[1]) {
        setSol_number(solutions_to_run_range[0]);
        functionEvaluation(solutions[solutions_to_run_range[0]].data(),
                           nullptr, nullptr);

        // Export pathways and objectives, otherwise, if required, run bootstrap sub-sampling.
        if (n_sets > 0 && n_bs_samples > 0) {
            runBootstrapRealizationThinning(
                    (int) solutions_to_run_range[0], n_sets, n_bs_samples,
                    n_threads, bs_realizations);
        } else if (import_export_rof_tables != EXPORT_ROF_TABLES) {
            if (plotting)
                printTimeSeriesAndPathways();
            auto objectives = calculateAndPrintObjectives(!print_obj_row);
            //            trianglePtr->getMaster_data_collector()->printNETCDFUtilities("netcdf_output");
        }

        destroyDataCollector();
    } else {
        double time_0 = omp_get_wtime();
        ofstream objs_file;
        string file_name = system_io + "output" + BAR + "Objectives" +
                           (rdm_no == NON_INITIALIZED ? "" : "_RDM" + to_string(
                                   rdm_no)) +
                           "_sols" + to_string(solutions_to_run_range[0]) +
                           "_to_" + to_string(solutions_to_run_range[1]) +
                           ".csv";
        objs_file.open(file_name);
        printf("Objectives file will be printed at %s.\n", file_name.c_str());

        for (int s = solutions_to_run_range[0];
             s < solutions_to_run_range[1]; ++s) {
            cout << endl << endl << "Running solution "
                 << s << (rdm_no != NON_INITIALIZED ? " RDM " : "")
                 << (rdm_no != NON_INITIALIZED ? to_string(rdm_no).c_str()
                                               : "")
                 << endl;
            setSol_number((unsigned long) s);
            functionEvaluation(solutions[s].data(), nullptr,
                               nullptr);
            vector<double> objectives = calculateAndPrintObjectives(
                    false);
            printTimeSeriesAndPathways(plotting);
            destroyDataCollector();
            string line;
            for (double &o : objectives) {
                line += to_string(o) + ",";
            }
            line.pop_back();
            objs_file << line << endl;
        }

        objs_file.close();
        printf("Time to simulate %d solutions: %f s",
               solutions_to_run_range[0] - solutions_to_run_range[1],
               omp_get_wtime() - time_0);
    }
}

vector<int> HardCodedProblem::vecInfraRankToVecInt(vector<infraRank> v) {
    vector<int> sorted;
    for (infraRank ir : v) {
        sorted.push_back(ir.id);
    }

    return sorted;
}

double HardCodedProblem::checkAndFixInfraExpansionHighLowOrder(
        vector<int> &order, vector<double> &triggers, int id_low,
        int id_high, double capacity_low, double capacity_high) {

    auto pos_low = distance(order.begin(),
                            find(order.begin(),
                                 order.end(),
                                 id_low));

    auto pos_high = distance(order.begin(),
                             find(order.begin(),
                                  order.end(),
                                  id_high));

    if (pos_high < pos_low) {
        capacity_high += capacity_low;
        order.erase(order.begin() + pos_low);
        triggers.erase(triggers.begin() + pos_low);
    }

    return capacity_high;
}

void HardCodedProblem::setRDMOptimization(vector<vector<double>> &utilities_rdm,
                                          vector<vector<double>> &water_sources_rdm,
                                          vector<vector<double>> &policies_rdm) {
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
}

void
HardCodedProblem::setRDMReevaluation(int rdm_no,
                                     vector<vector<double>> &utilities_rdm,
                                     vector<vector<double>> &water_sources_rdm,
                                     vector<vector<double>> &policies_rdm) {
    this->rdm_no = rdm_no;
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
}

void HardCodedProblem::readInputData() {
    int i = 0;
}
