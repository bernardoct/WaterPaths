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
        int import_export_rof_tables,
        const unsigned long total_simulation_time,
        vector<unsigned long> &realizations_to_run) :
        total_simulation_time(total_simulation_time),
        realizations_to_run(realizations_to_run),
        import_export_rof_tables(import_export_rof_tables),
        n_realizations(realizations_to_run.size()) {

    /// Sort water sources and utilities by their IDs.
    //FIXME: THERE IS A STUPID MISTAKE HERE IN THE SORT FUNCTION THAT IS PREVENTING IT FROM WORKING UNDER WINDOWS AND LINUX.
    std::sort(water_sources.begin(), water_sources.end(), WaterSource::compare);
#ifdef _WIN32
    sort(utilities.begin(), utilities.end(), std::greater<>());
#else
    std::sort(utilities.begin(),
              utilities.end(),
              Utility::compById);
#endif

    /// Check if IDs are sequential.
    for (int ws = 1; ws < water_sources.size(); ++ws) {
        if (water_sources[ws]->id != water_sources[ws - 1]->id + 1) {
            cout << "The IDs of water sources " << water_sources[ws]->id << " "
                    "and " << water_sources[ws - 1]->id << " do not follow a "
                         "unit progression." << endl;
            throw_with_nested(invalid_argument("Improper water source ID sequencing"));
        }
    }

    for (int u = 1; u < utilities.size(); ++u) {
        if (utilities[u]->id != utilities[u - 1]->id + 1) {
            cout << "The IDs of utilities " << utilities[u]->id << " "
                    "and " << utilities[u - 1]->id << " do not follow a "
                         "unit progression." << endl;
            throw_with_nested(invalid_argument("Improper utility ID sequencing"));
        }
    }

    /// Check if sources listed in construction order array are of a utility are
    /// listed as belonging to that utility
    for (int u = 0; u < utilities.size(); ++u) {
        /// Create a vector with rof and demand triggered infrastructure for
        /// utility u.
        vector<int> demand_rof_infra_order =
                utilities[u]->getRof_infrastructure_construction_order();
        demand_rof_infra_order.insert(
                demand_rof_infra_order.begin(),
                utilities[u]->getDemand_infra_construction_order().begin(),
                utilities[u]->getDemand_infra_construction_order().end());
        /// Iterate over demand and rof combined infrastructure vector
        /// looking for sources declared as to be constructed that were not
        /// declared as belonging to utility u.
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

    /// Creates the data collector for the simulation.
    master_data_collector = new MasterDataCollector();

    /// Create the realization and ROF models.
    for (unsigned long r : realizations_to_run) {
        /// Create realization models by copying the water sources and utilities.
        vector<WaterSource *> water_sources_realization =
                Utils::copyWaterSourceVector(water_sources);
        vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization =
                Utils::copyDroughtMitigationPolicyVector(
                        drought_mitigation_policies);
        vector<Utility *> utilities_realization =
                Utils::copyUtilityVector(utilities);
        vector<MinEnvFlowControl *> min_env_flow_controls_realization =
                Utils::copyMinEnvFlowControlVector(min_env_flow_controls);

        master_data_collector->addRealization(
                water_sources_realization,
                drought_mitigation_policies_realization,
                utilities_realization,
                r);

        /// Store realization models in vector
        realization_models.push_back(new ContinuityModelRealization(
                water_sources_realization,
                water_sources_graph,
                water_sources_to_utilities,
                utilities_realization,
                drought_mitigation_policies_realization,
                min_env_flow_controls_realization,
                utilities_rdm.at(r),
                water_sources_rdm.at(r),
                policies_rdm.at(r),
                r));

        /// Create rof models by copying the water utilities and sources.
        vector<WaterSource *> water_sources_rof =
                Utils::copyWaterSourceVector(water_sources);
        vector<Utility *> utilities_rof = Utils::copyUtilityVector(utilities);
        vector<MinEnvFlowControl *> min_env_flow_controls_rof =
                Utils::copyMinEnvFlowControlVector(min_env_flow_controls);

        /// Store realization models in vector
        rof_models.push_back(new ContinuityModelROF(
                water_sources_rof,
                water_sources_graph,
                water_sources_to_utilities,
                utilities_rof,
                min_env_flow_controls_rof,
                utilities_rdm.at(r),
                water_sources_rdm.at(r),
                total_simulation_time,
                import_export_rof_tables == IMPORT_ROF_TABLES,
                r));

        /// Initialize rof models by connecting it to realization water sources.
        rof_models.back()->connectRealizationWaterSources(water_sources_realization);
        rof_models.back()->connectRealizationUtilities(utilities_realization);

        /// Link storage-rof tables of policies and rof models.
        for (DroughtMitigationPolicy *dmp :
                realization_models.back()->getDrought_mitigation_policies())
            dmp->setStorage_to_rof_table_(
                    rof_models.back()->getUt_storage_to_rof_table());
    }
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
    this->total_simulation_time = simulation.total_simulation_time;
    return *this;
}

MasterDataCollector* Simulation::runFullSimulation(unsigned long n_threads) {

    if (rof_tables_folder.length() == 0)
        rof_tables_folder = "rof_tables";

    /// Run realizations.
    int had_catch = 0;
    string error_m = "Error in realizations ";
#pragma omp parallel for num_threads(n_threads)// shared(had_catch)
    for (int r = 0; r < n_realizations; ++r) {
//        try {
            double start = omp_get_wtime();
            for (int w = 0; w < total_simulation_time; ++w) {
                // DO NOT change the order of the steps. This would mess up
                // important dependencies.
                /// Calculate long-term risk-of-failre if current week is first week of the year.
                if (Utils::isFirstWeekOfTheYear(w))
                    realization_models[r]->setLongTermROFs(
                            rof_models[r]->calculateLongTermROF(w), w);
                /// Calculate short-term risk-of-failure
                if (import_export_rof_tables == IMPORT_ROF_TABLES) {
                    realization_models[r]->setShortTermROFs(
                            rof_models[r]->calculateShortTermROFTable(w));
		        } else {
                    realization_models[r]->setShortTermROFs(
                            rof_models[r]->calculateShortTermROF(w));
                }
                /// Apply drought mitigation policies
                realization_models[r]->applyDroughtMitigationPolicies(w);
                /// Continuity calculations for current week
                realization_models[r]->continuityStep(w);
                /// Collect system data for output printing and objective calculations.
                if (import_export_rof_tables != EXPORT_ROF_TABLES)
                    master_data_collector->collectData(r);
                //printf("r%d w%d\n", r, w);
            }
            /// Export ROF tables for future simulations of the same problem with the same states-of-the-world.
            if (import_export_rof_tables == EXPORT_ROF_TABLES)
                rof_models[r]->printROFTable(rof_tables_folder);
//#pragma omp critical
//{
//            double end = omp_get_wtime();
//            std::cout << "Realization " << realizations_to_run[r] << ": "
//                      << end - start << std::endl;
//}

            delete realization_models[r];
            delete rof_models[r];
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
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
#else
        world_rank = 0;
#endif
	//printf(error_m.c_str());
	    error_m += ". Decision variables in sol_error_rank_" + to_string(world_rank) + ".";
        throw_with_nested(runtime_error(error_m.c_str()));
    }
    return master_data_collector;
}

void Simulation::setPrecomputed_rof_tables(const vector<vector<Matrix2D<double>>> &precomputed_rof_tables,
                                           vector<vector<double>> &table_storage_shift) {
    Simulation::table_storage_shift = table_storage_shift;
    this->rof_tables_folder = rof_tables_folder;

    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        for (int r = 0; r < n_realizations; ++r) {
            rof_models[r]->setROFTablesAndShifts(precomputed_rof_tables[r], table_storage_shift);
        }
    }
}

void Simulation::setRof_tables_folder(const string &rof_tables_folder) {
    Simulation::rof_tables_folder = rof_tables_folder;
}
