//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "../Utils/Utils.h"
#include <ctime>


Simulation::Simulation(vector<WaterSource *> &water_sources, Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> &utilities,
                       vector<DroughtMitigationPolicy *> &drought_mitigation_policies, const int total_simulation_time,
                       const int number_of_realizations, DataCollector *data_collector) :
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations), data_collector(data_collector),
        drought_mitigation_policies(drought_mitigation_policies) {

    /// Sort water sources and utilities by their IDs.
    //FIXME: THERE IS A STUPID MISTAKE HERE IN THE SORT FUNCTION THAT IS PREVENTING IT FROM WORKING UNDER WINDOWS AND LINUX.
#ifdef _WIN32
    sort(continuity_water_sources.begin(), continuity_water_sources.end(), std::greater<>());
    sort(continuity_utilities.begin(), continuity_utilities.end(), std::greater<>());
#else
    sort(water_sources.begin(), water_sources.end());
    sort(utilities.begin(), utilities.end());
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

    /// Creates the realization and ROF models.
    vector<WaterSource *> water_sources_realization;
    vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization;
    for (int r = 0; r < number_of_realizations; ++r) {
        /// Creates realization models by copying the water sources and utilities.
        water_sources_realization = Utils::copyWaterSourceVector(water_sources);
        realization_models.push_back(new ContinuityModelRealization(water_sources_realization,
                                                                    water_sources_graph,
                                                                    water_sources_to_utilities,
                                                                    Utils::copyUtilityVector(utilities),
                                                                    Utils::copyDroughtMitigationPolicyVector(
                                                                            drought_mitigation_policies),
                                                                    r));

        /// Creates rof models by copying the water utilities and sources.
        rof_models.push_back(new ContinuityModelROF(Utils::copyWaterSourceVector(water_sources), water_sources_graph,
                                                    water_sources_to_utilities, Utils::copyUtilityVector(utilities), r));

        /// Initializes rof models.
        rof_models[r]->setRealization_water_sources(water_sources_realization);

        /// Clear vectors for reuse in the setup of next realization.
        water_sources_realization.clear();
        drought_mitigation_policies_realization.clear();
    }
}

Simulation::~Simulation() {

}

/**
 * Assignment constructor
 * @param simulation
 * @return
 * @todo implement assignment constructor.
 */
Simulation &Simulation::operator=(const Simulation &simulation) {
    this->number_of_realizations = simulation.number_of_realizations;
    this->total_simulation_time = simulation.total_simulation_time;

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
        cout << "Realization " << r << endl;
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
    cout << seconds << "s" << endl;

    /// Calculate objective values.
    data_collector->calculateObjectives();

    /// Print output files.
    data_collector->printUtilityOutput("Utilities.out");
    data_collector->printReservoirOutput("WaterSources.out");
    data_collector->printPoliciesOutput("Policies.out");
    data_collector->printObjectives("Objectives.out");
}
