//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "../Utils/Utils.h"
#include <ctime>
#include <algorithm>
#include <numeric>
#include <omp.h>
#ifdef  PARALLEL
#include <mpi.h>
#endif

Simulation::Simulation(
        vector<WaterSource *> &water_sources, Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvFlowControl *> &min_env_flow_controls,
        vector<vector<double>>& utilities_rdm,
        vector<vector<double>>& water_sources_rdm,
        vector<vector<double>>& policies_rdm,
        const unsigned long total_simulation_time,
        vector<unsigned long> &realizations_to_run) :
        total_simulation_time(total_simulation_time),
        realizations_to_run(realizations_to_run),
        import_export_rof_tables(DO_NOT_EXPORT_OR_IMPORT_ROF_TABLES),
        n_realizations(realizations_to_run.size()),
        water_sources(water_sources),
        water_sources_graph(water_sources_graph),
        water_sources_to_utilities(water_sources_to_utilities),
        utilities(utilities),
        drought_mitigation_policies(drought_mitigation_policies),
        min_env_flow_controls(min_env_flow_controls),
        utilities_rdm(utilities_rdm),
        water_sources_rdm(water_sources_rdm),
        policies_rdm(policies_rdm) {

    setupSimulation(
            water_sources, water_sources_graph,
            water_sources_to_utilities, utilities, drought_mitigation_policies,
            min_env_flow_controls, utilities_rdm, water_sources_rdm,
            policies_rdm, import_export_rof_tables, total_simulation_time,
            realizations_to_run);
}

Simulation::Simulation(
        vector<WaterSource *> &water_sources, Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvFlowControl *> &min_env_flow_controls,
        vector<vector<double>>& utilities_rdm,
        vector<vector<double>>& water_sources_rdm,
        vector<vector<double>>& policies_rdm,
        const unsigned long total_simulation_time,
        vector<unsigned long> &realizations_to_run,
        vector<vector<Matrix2D<double>>> &precomputed_rof_tables,
        vector<vector<double>> &table_storage_shift,
        string &rof_tables_folder) :
        total_simulation_time(total_simulation_time),
        realizations_to_run(realizations_to_run),
        import_export_rof_tables(IMPORT_ROF_TABLES),
        n_realizations(realizations_to_run.size()),
        water_sources(water_sources),
        water_sources_graph(water_sources_graph),
        water_sources_to_utilities(water_sources_to_utilities),
        utilities(utilities),
        drought_mitigation_policies(drought_mitigation_policies),
        min_env_flow_controls(min_env_flow_controls),
        utilities_rdm(utilities_rdm),
        water_sources_rdm(water_sources_rdm),
        policies_rdm(policies_rdm),
        precomputed_rof_tables(&precomputed_rof_tables),
        table_storage_shift(&table_storage_shift) {

    setRof_tables_folder(rof_tables_folder);

    setupSimulation(
            water_sources, water_sources_graph,
            water_sources_to_utilities, utilities, drought_mitigation_policies,
            min_env_flow_controls, utilities_rdm, water_sources_rdm,
            policies_rdm, import_export_rof_tables, total_simulation_time,
            realizations_to_run);
}

Simulation::Simulation(
        vector<WaterSource *> &water_sources, 
	    Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvFlowControl *> &min_env_flow_controls,
        vector<vector<double>>& utilities_rdm,
        vector<vector<double>>& water_sources_rdm,
        vector<vector<double>>& policies_rdm,
        const unsigned long total_simulation_time,
        vector<unsigned long> &realizations_to_run,
        string &rof_tables_folder) :
        total_simulation_time(total_simulation_time),
        realizations_to_run(realizations_to_run),
        import_export_rof_tables(EXPORT_ROF_TABLES),
        n_realizations(realizations_to_run.size()),
        water_sources(water_sources),
        water_sources_graph(water_sources_graph),
        water_sources_to_utilities(water_sources_to_utilities),
        utilities(utilities),
        drought_mitigation_policies(drought_mitigation_policies),
        min_env_flow_controls(min_env_flow_controls),
        utilities_rdm(utilities_rdm),
        water_sources_rdm(water_sources_rdm),
        policies_rdm(policies_rdm) {

    setRof_tables_folder(rof_tables_folder);

    setupSimulation(
            water_sources,
            water_sources_graph,
            water_sources_to_utilities,
            utilities,
            drought_mitigation_policies,
            min_env_flow_controls,
            utilities_rdm,
            water_sources_rdm,
            policies_rdm,
            import_export_rof_tables,
            total_simulation_time,
            realizations_to_run);
}

void Simulation::setupSimulation(
        vector<WaterSource *> &water_sources,
        Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvFlowControl *> &min_env_flow_controls,
        vector<vector<double>>& utilities_rdm,
        vector<vector<double>>& water_sources_rdm,
        vector<vector<double>>& policies_rdm,
        int import_export_rof_tables,
        const unsigned long total_simulation_time,
        vector<unsigned long> &realizations_to_run) {

    // Sort water sources and utilities by their IDs.
    //FIXME: THERE IS A STUPID MISTAKE HERE IN THE SORT FUNCTION THAT IS PREVENTING IT FROM WORKING UNDER WINDOWS AND LINUX.
    std::sort(water_sources.begin(), water_sources.end(), WaterSource::compare);
#ifdef _WIN32
    sort(utilities.begin(), utilities.end(), std::greater<>());
#else
    std::sort(utilities.begin(), utilities.end(), Utility::compById);
#endif

    // Check for missing water sources or vertices in graph.
    int number_of_connected_sources = 0;
    for (auto ws : water_sources) {
        if (ws->source_type != WATER_REUSE &&
                ws->source_type != RESERVOIR_EXPANSION &&
                ws->source_type != NEW_WATER_TREATMENT_PLANT) {
            ++number_of_connected_sources;
        }
    }
    if (water_sources_graph.getNumber_of_vertices() !=
            number_of_connected_sources) {
        __throw_invalid_argument("Number of storage-like or intake water "
                                 "sources in water sources vector must be "
                                 "the same as number of vertices in the "
                                 "connectivity graph.");
    }

    // Check if IDs are sequential.
    for (int ws = 1; ws < (int) water_sources.size(); ++ws) {
        if (water_sources[ws]->id != water_sources[ws - 1]->id + 1) {
            string error = "The IDs of water sources ";
            error += to_string(water_sources[ws]->id) + " and "
                    + to_string(water_sources[ws - 1]->id) +
                    " do not follow a unit progression. Water "
                    "sources IDs must be sequential.";
            throw_with_nested(invalid_argument(error.c_str()));
        }
    }

    for (int u = 1; u < (int) utilities.size(); ++u) {
        if (utilities[u]->id != utilities[u - 1]->id + 1) {
            cout << "The IDs of utilities " << utilities[u]->id << " "
                    "and " << utilities[u - 1]->id << " do not follow a "
                         "unit progression." << endl;
            throw_with_nested(invalid_argument("Improper utility ID sequencing"));
        }
    }

    // Check if sources listed in construction order array are of a utility are
    // listed as belonging to that utility
    for (int u = 0; u < (int) utilities.size(); ++u) {
        // Create a vector with rof and demand triggered infrastructure for
        // utility u.
        vector<int> demand_rof_infra_order =
                utilities[u]->getRof_infrastructure_construction_order();
        demand_rof_infra_order.insert(
                demand_rof_infra_order.begin(),
                utilities[u]->getDemand_infra_construction_order().begin(),
                utilities[u]->getDemand_infra_construction_order().end());
        // Iterate over demand and rof combined infrastructure vector
        // looking for sources declared as to be constructed that were not
        // declared as belonging to utility u.
        for (int ws :
                demand_rof_infra_order)
            if (std::find(water_sources_to_utilities[u].begin(),
                          water_sources_to_utilities[u].end(),
                          ws)
                == water_sources_to_utilities[u].end()) {
                cout << "Water source #" << ws << " is listed in the "
                        "construction order for utility " << utilities[u]->id
                     << " (" << utilities[u]->name << ")  but is  not  "
                             "present in  utility's list of water sources."
                     << endl;
                __throw_invalid_argument("Utility's construction order and "
                                                    "owned sources mismatch.");
            }

        for (int ws : water_sources_to_utilities[u])
            if (find_if(water_sources.begin(),
                        water_sources.end(),
                        [&ws](
                                const WaterSource *
                                obj) { return obj->id == ws; }) ==
                water_sources.end()) {
                cout << "Water source #" << ws << " not present in "
                        "comprehensive water sources vector." << endl;
                __throw_invalid_argument("Water sources declared to belong to"
                                                 " a utility is not present "
                                                 "in vector of water sources.");
            }
    }

    // Creates the data collector for the simulation.
    master_data_collector = new MasterDataCollector(n_realizations);
}

Simulation::~Simulation() = default;

/**
 * Assignment constructor
 * @param simulation
 * @return
 * @todo implement assignment constructor.
 */
Simulation &Simulation::operator=(const Simulation &simulation) {
    this->n_realizations = simulation.n_realizations;
    return *this;
}

void Simulation::createContinuityModels(unsigned long realization,
                                        ContinuityModelRealization *&realization_model,
                                        ContinuityModelROF *&rof_model) {

    // Create realization models by copying the water sources and utilities.
    vector<WaterSource *> water_sources_realization =
            Utils::copyWaterSourceVector(water_sources);
    vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization =
            Utils::copyDroughtMitigationPolicyVector(
                    drought_mitigation_policies);
    vector<Utility *> utilities_realization =
            Utils::copyUtilityVector(utilities);
    vector<MinEnvFlowControl *> min_env_flow_controls_realization =
            Utils::copyMinEnvFlowControlVector(min_env_flow_controls);

    // Store realization models in vector
    realization_model = new ContinuityModelRealization(
            water_sources_realization,
            water_sources_graph,
            water_sources_to_utilities,
            utilities_realization,
            drought_mitigation_policies_realization,
            min_env_flow_controls_realization,
            utilities_rdm.at(realization),
            water_sources_rdm.at(realization),
            policies_rdm.at(realization),
            (int) realization);

    // Create rof models by copying the water utilities and sources.
    vector<WaterSource *> water_sources_rof =
            Utils::copyWaterSourceVector(water_sources);
    vector<Utility *> utilities_rof = Utils::copyUtilityVector(utilities);
    vector<MinEnvFlowControl *> min_env_flow_controls_rof =
            Utils::copyMinEnvFlowControlVector(min_env_flow_controls);

    // Store realization models in vector
    rof_model = new ContinuityModelROF(
            water_sources_rof,
            water_sources_graph,
            water_sources_to_utilities,
            utilities_rof,
            min_env_flow_controls_rof,
            utilities_rdm.at(realization),
            water_sources_rdm.at(realization),
            total_simulation_time,
            import_export_rof_tables,
            realization);

    // Initialize rof models by connecting it to realization water sources.
    rof_model->connectRealizationWaterSources(water_sources_realization);
    rof_model->connectRealizationUtilities(utilities_realization);

    // Pass ROF tables to continuity model
    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        rof_model->setROFTablesAndShifts(precomputed_rof_tables->at(realization), *table_storage_shift);
    }

    // Link storage-rof tables of policies and rof models.
    for (DroughtMitigationPolicy *dmp :
            realization_model->getDrought_mitigation_policies())
        dmp->setStorage_to_rof_table_(
                rof_model->getUt_storage_to_rof_table());
}

MasterDataCollector* Simulation::runFullSimulation(unsigned long n_threads) {

    if (rof_tables_folder.length() == 0)
        rof_tables_folder = "rof_tables";

    // Run realizations.
    int had_catch = 0;
    string error_m = "Error in realizations ";
//    std::reverse(realizations_to_run.begin(), realizations_to_run.end());
#pragma omp parallel for ordered num_threads(n_threads) shared(had_catch)
//    for (int r = (int) n_realizations - 1; r >= 0; --r) {
    for (unsigned long r = 0; r < n_realizations; ++r) {
//        try {
            unsigned long realization = realizations_to_run.at(r);

            /// Create continuity models.
            ContinuityModelRealization* realization_model = nullptr;
            ContinuityModelROF* rof_model = nullptr;
            createContinuityModels(realization, realization_model, rof_model);

            /// Initialize data collector.
            master_data_collector->addRealization(
                    realization_model->getContinuity_water_sources(),
                    realization_model->getDrought_mitigation_policies(),
                    realization_model->getContinuity_utilities(),
                    r);

//            double start = omp_get_wtime();
            for (int w = 0; w < (int) total_simulation_time; ++w) {
                // DO NOT change the order of the steps. This would mess up
                // important dependencies.
                /// Calculate long-term risk-of-failre if current week is first week of the year.
                if (Utils::isFirstWeekOfTheYear(w))
                    realization_model->setLongTermROFs(
                            rof_model->calculateLongTermROF(w), w);
                /// Calculate short-term risk-of-failure
                if (import_export_rof_tables == IMPORT_ROF_TABLES) {
                    realization_model->setShortTermROFs(
                            rof_model->calculateShortTermROFTable(w));
		        } else {
                    realization_model->setShortTermROFs(
                            rof_model->calculateShortTermROF(w));
                }
                /// Apply drought mitigation policies
                realization_model->applyDroughtMitigationPolicies(w);
                /// Continuity calculations for current week
                realization_model->continuityStep(w);
                /// Collect system data for output printing and objective calculations.
                if (import_export_rof_tables != EXPORT_ROF_TABLES)
                    master_data_collector->collectData(r);
            }
            /// Export ROF tables for future simulations of the same problem with the same states-of-the-world.
            if (import_export_rof_tables == EXPORT_ROF_TABLES)
                rof_model->printROFTable(rof_tables_folder);
//#pragma omp critical
//{
//            double end = omp_get_wtime();
//            std::cout << "Realization " << realizations_to_run[r] << ": "
//                      << end - start << std::endl;
//}

            delete realization_model;
            delete rof_model;
//        } catch (...) {
//#pragma omp atomic
//                ++had_catch;
//            error_m += to_string(r) + " ";
//        }
    }
    /// Handle exception from the OpenMP region and pass it up to the
    /// problem class.
    if (had_catch) {
	int world_rank;
#ifdef  PARALLEL
	int mpi_initialized;
	MPI_Initialized(&mpi_initialized);
	if (mpi_initialized)
            MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	else
	    world_rank = 0;
#else
        world_rank = 0;
#endif
	error_m += ". Decision variables in sol_error_rank_" + to_string(world_rank) + ".";
	printf("%s", error_m.c_str());
        throw_with_nested(runtime_error(error_m.c_str()));
    }
    return master_data_collector;
}

void Simulation::setPrecomputed_rof_tables(const vector<vector<Matrix2D<double>>> &precomputed_rof_tables,
                                           vector<vector<double>> &table_storage_shift) {
    this->rof_tables_folder = rof_tables_folder;
}

void Simulation::setRof_tables_folder(const string &rof_tables_folder) {
    Simulation::rof_tables_folder = rof_tables_folder;
}
