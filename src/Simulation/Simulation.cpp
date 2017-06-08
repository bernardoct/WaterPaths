//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "../Utils/Utils.h"
#include <ctime>
#include <algorithm>


Simulation::Simulation(vector<WaterSource *> &water_sources, Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> &utilities,
                       const vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                       const int total_simulation_time,
                       const int number_of_realizations, DataCollector *data_collector) :
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations), data_collector(data_collector) {

    /// Sort water sources and utilities by their IDs.
    //FIXME: THERE IS A STUPID MISTAKE HERE IN THE SORT FUNCTION THAT IS PREVENTING IT FROM WORKING UNDER WINDOWS AND LINUX.
    std::sort(water_sources.begin(), water_sources.end(), WaterSource::compare);
#ifdef _WIN32
    sort(continuity_utilities.begin(), continuity_utilities.end(), std::greater<>());
#else
    std::sort(utilities.begin(), utilities.end());
#endif

    /// Pass sum of minimum environmental inflows of upstream sources to intakes.
    double upstream_min_env_flow;
    for (WaterSource *ws : water_sources) {
        upstream_min_env_flow = 0;
        if (ws->source_type == INTAKE) {
            for (int &i : water_sources_graph.getUpstream_sources(ws->id))
                upstream_min_env_flow += water_sources[i]->min_environmental_outflow;
            dynamic_cast<Intake *>(ws)->setUpstream_min_env_flow(upstream_min_env_flow);
        }
    }

    /// Creates the data collector for the simulation.
    data_collector = new DataCollector(utilities, water_sources, drought_mitigation_policies,
                                       number_of_realizations, water_sources_graph);
    this->data_collector = data_collector;

    /// Create the realization and ROF models.
    for (int r = 0; r < number_of_realizations; ++r) {
        /// Create realization models by copying the water sources and utilities.
        vector<WaterSource *> water_sources_realization = Utils::copyWaterSourceVector(water_sources);
        vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization =
                Utils::copyDroughtMitigationPolicyVector(drought_mitigation_policies);
        vector<Utility *> utilities_realization = Utils::copyUtilityVector(utilities);

        /// Store realization models in vector
        realization_models.push_back(new ContinuityModelRealization(water_sources_realization,
                                                                    water_sources_graph,
                                                                    water_sources_to_utilities,
                                                                    utilities_realization,
                                                                    drought_mitigation_policies_realization,
                                                                    r));

        /// Create rof models by copying the water utilities and sources.
        vector<WaterSource *> water_sources_rof = Utils::copyWaterSourceVector(water_sources);
        vector<Utility *> utilities_rof = Utils::copyUtilityVector(utilities);

        /// Store realization models in vector
        rof_models.push_back(new ContinuityModelROF(water_sources_rof,
                                                    water_sources_graph,
                                                    water_sources_to_utilities,
                                                    utilities_rof,
                                                    r));

        /// Initialize rof models.
        rof_models[r]->setRealization_water_sources(water_sources_realization);

        /// Link storage-rof tables of policies and rof models.
        for (DroughtMitigationPolicy *dmp : realization_models[r]->getDrought_mitigation_policies())
            dmp->setStorage_to_rof_table_(rof_models[r]->getStorage_to_rof_table());
    }
}

Simulation::~Simulation() {}

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

void Simulation::runFullSimulation(int num_threads) {

    int n_utilities = (int) realization_models[0]->getUtilities().size();
    vector<double> risks_of_failure_week((unsigned long) n_utilities, 0.0);
    time_t timer_i;
    time_t timer_f;
    double seconds;

    /// Run realizations.
    time(&timer_i);
#pragma omp parallel for num_threads(num_threads)
    for (int r = 0; r < number_of_realizations; ++r) {
        std::cout << "Realization " << r << std::endl;
        for (int w = 0; w < total_simulation_time; ++w) {
            // DO NOT change the order of the steps. This would destroy dependencies.
            if (Utils::isFirstWeekOfTheYear(w))
                realization_models[r]->setLongTermROFs(rof_models[r]->calculateROF(w, LONG_TERM_ROF), w);
            realization_models[r]->setShortTermROFs(rof_models[r]->calculateROF(w, SHORT_TERM_ROF));
            realization_models[r]->applyDroughtMitigationPolicies(w);
            realization_models[r]->continuityStep(w);
            data_collector->collectData(realization_models[r]);
        }
    }
    time(&timer_f);
    seconds = difftime(timer_f, timer_i);
    std::cout << seconds << "s" << std::endl;

    /// Calculate objective values.
    data_collector->calculateObjectives();

    /// Print output files.
    data_collector->printUtilityOutput("Utilities.out");
    data_collector->printReservoirOutput("WaterSources.out");
    data_collector->printPoliciesOutput("Policies.out");
    data_collector->printObjectives("Objectives.out");
}
