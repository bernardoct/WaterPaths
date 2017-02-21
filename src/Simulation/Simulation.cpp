//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "../Utils/Aux.h"
#include <iostream>
#include <algorithm>

Simulation::Simulation(vector<WaterSource *> &water_sources, Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> &utilities,
                       vector<DroughtMitigationPolicy *> &drought_mitigation_policies, const int total_simulation_time,
                       const int number_of_realizations, DataCollector *data_collector) :
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations), data_collector(data_collector),
        drought_mitigation_policies(drought_mitigation_policies) {

    // Sort water sources and utilities by their IDs.
    sort(water_sources.begin(), water_sources.end());
    sort(utilities.begin(), utilities.end());

    // Pass sum of minimum environmental inflows of upstream sources to intakes.
    double upstream_min_env_flow;
    for (WaterSource *ws : water_sources) {
        upstream_min_env_flow = 0;
        if (ws->source_type == INTAKE) {
            for (int &i : water_sources_graph.getUpstreamSources(ws->id))
                upstream_min_env_flow += water_sources[i]->min_environmental_outflow;
            dynamic_cast<Intake *>(ws)->setUpstream_min_env_flow(upstream_min_env_flow);
        }
    }

    // Creates the data collector for the simulation.
    this->data_collector = new DataCollector(utilities, water_sources, drought_mitigation_policies,
                                             number_of_realizations);

    // Creates the realization and ROF models.
    vector<WaterSource *> water_sources_realization;
    vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization;
    for (int r = 0; r < number_of_realizations; ++r) {
        // Creates realization models by copying the water sources and utilities.
        water_sources_realization = Aux::copyWaterSourceVector(water_sources);
//        drought_mitigation_policies_realization = ;
        realization_models.push_back(new ContinuityModelRealization(water_sources_realization,
                                                                    water_sources_graph,
                                                                    water_sources_to_utilities,
                                                                    Aux::copyUtilityVector(utilities),
                                                                    Aux::copyDroughtMitigationPolicyVector(
                                                                            drought_mitigation_policies),
                                                                    r));

        // Creates rof models by copying the water utilities and sources.
        rof_models.push_back(new ContinuityModelROF(Aux::copyWaterSourceVector(water_sources),
                                                    water_sources_graph,
                                                    water_sources_to_utilities,
                                                    Aux::copyUtilityVector(utilities),
                                                    SHORT_TERM_ROF, r));

        rof_models[r]->setWater_sources_realization(water_sources_realization);
        water_sources_realization.clear();
        drought_mitigation_policies_realization.clear();
    }
}

void Simulation::runFullSimulation() {

    int n_utilities = (int) realization_models[0]->getUtilities().size();
    vector<double> risks_of_failure_week((unsigned long) n_utilities, 0.0);
    for (int r = 0; r < number_of_realizations; ++r) {
        cout << "Realization: " << r << endl;
        for (int w = 0; w < total_simulation_time; ++w) {
            realization_models[r]->applyRestrictionsAndTransfers(w);
            realization_models[r]->continuityStep(w);
            risks_of_failure_week = rof_models[r]->calculateROF(w);
            realization_models[r]->setRisks_of_failure(risks_of_failure_week);
            data_collector->collectData(realization_models[r], w);
        }
    }

    data_collector->printUtilityOutput(true);
    data_collector->printReservoirOutput(true);
    data_collector->printPoliciesOutput(true);

}
