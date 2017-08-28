//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "../Utils/Utils.h"
#include <ctime>
#include <algorithm>

Simulation::Simulation(
        vector<WaterSource *> &water_sources, Graph &water_sources_graph,
        const vector<vector<int>> &water_sources_to_utilities,
        vector<Utility *> &utilities,
        const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
        vector<MinEnvironFlowControl *> &min_env_flow_controls,
        const int total_simulation_time, const int number_of_realizations) :
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations) {

    /// Sort water sources and utilities by their IDs.
    //FIXME: THERE IS A STUPID MISTAKE HERE IN THE SORT FUNCTION THAT IS PREVENTING IT FROM WORKING UNDER WINDOWS AND LINUX.
    std::sort(water_sources.begin(), water_sources.end(), WaterSource::compare);
#ifdef _WIN32
    sort(continuity_utilities.begin(), continuity_utilities.end(), std::greater<>());
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
                         "progression unit progression." << endl;
            __throw_invalid_argument("Improper ID sequencing");
        }
    }

    for (int u = 1; u < utilities.size(); ++u) {
        if (utilities[u]->id != utilities[u - 1]->id + 1) {
            cout << "The IDs of utilities " << utilities[u]->id << " "
                    "and " << utilities[u - 1]->id << " do not follow a "
                         "progression unit progression." << endl;
            __throw_invalid_argument("Improper ID sequencing");
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
                throw std::invalid_argument("Utility's construction order and "
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
//    data_collector = new DataCollectorDeprecated(
//            utilities,
//            water_sources,
//            drought_mitigation_policies,
//            number_of_realizations,
//            water_sources_graph);

    master_data_collector = new MasterDataCollector();

    /// Create the realization and ROF models.
    for (unsigned int r = 0; r < number_of_realizations; ++r) {
        /// Create realization models by copying the water sources and utilities.
        vector<WaterSource *> water_sources_realization =
                Utils::copyWaterSourceVector(water_sources);
        vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization =
                Utils::copyDroughtMitigationPolicyVector(
                        drought_mitigation_policies);
        vector<Utility *> utilities_realization =
                Utils::copyUtilityVector(utilities);

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
                min_env_flow_controls,
                r));

        /// Create rof models by copying the water utilities and sources.
        vector<WaterSource *> water_sources_rof =
                Utils::copyWaterSourceVector(water_sources);
        vector<Utility *> utilities_rof = Utils::copyUtilityVector(utilities);

        /// Store realization models in vector
        rof_models.push_back(new ContinuityModelROF(water_sources_rof,
                                                    water_sources_graph,
                                                    water_sources_to_utilities,
                                                    utilities_rof,
                                                    min_env_flow_controls,
                                                    r));

        /// Initialize rof models.
        rof_models[r]->setRealization_water_sources(water_sources_realization);

        /// Link storage-rof tables of policies and rof models.
        for (DroughtMitigationPolicy *dmp :
                realization_models[r]->getDrought_mitigation_policies())
            dmp->setStorage_to_rof_table_(
                    rof_models[r]->getStorage_to_rof_table());
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
    this->number_of_realizations = simulation.number_of_realizations;
    this->total_simulation_time = simulation.total_simulation_time;
    return *this;
}

MasterDataCollector *Simulation::runFullSimulation(int num_threads) {

    int n_utilities = (int) realization_models[0]->getUtilities().size();
    vector<double> risks_of_failure_week((unsigned long) n_utilities, 0.0);
    time_t timer_i;
    time_t timer_f;
    double seconds;

    /// Run realizations.
    time(&timer_i);
    int count = 0;
    std::cout << "Simulated time: " << total_simulation_time << endl;
    std::cout << "Number of realizations: " << number_of_realizations << endl;
    std::cout << "Beginning realizations loop." << endl;
#pragma omp parallel for num_threads(num_threads)
    for (int r = 0; r < number_of_realizations; ++r) {
        try {
            count++;
            time_t timer_ir, timer_fr;
            time(&timer_ir);
            for (int w = 0; w < total_simulation_time; ++w) {
                // DO NOT change the order of the steps. This would mess up
                // important dependencies.
                if (Utils::isFirstWeekOfTheYear(w))
                    realization_models[r]->setLongTermROFs(rof_models[r]->calculateROF(w, LONG_TERM_ROF), w);
                realization_models[r]->setShortTermROFs(rof_models[r]->calculateROF(w, SHORT_TERM_ROF));
                realization_models[r]->applyDroughtMitigationPolicies(w);
                realization_models[r]->continuityStep(w);
                master_data_collector->collectData(r);
            }
            time(&timer_fr);
            std::cout << "Realization " << count << ": "
                      << difftime(timer_fr, timer_ir) << std::endl;

        } catch (const std::exception &e) {
            cout << "Error in realization " << r << endl;
            e.what();
        }
    }
    time(&timer_f);
    seconds = difftime(timer_f, timer_i);
    std::cout << "Calculations: " << seconds << "s" << std::endl;

    time(&timer_f);
    seconds = difftime(timer_f,
                       timer_i);
    std::cout << "Total: " << seconds << "s" << std::endl;

    return master_data_collector;
}
