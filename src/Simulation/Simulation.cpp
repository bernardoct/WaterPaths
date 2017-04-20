//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "../Utils/Utils.h"
#include <iostream>
#include <algorithm>
#include <time.h>
#include <cmath>


Simulation::Simulation(vector<WaterSource *> &water_sources, Graph &water_sources_graph,
                       const vector<vector<int>> &water_sources_to_utilities, vector<Utility *> &utilities,
                       vector<DroughtMitigationPolicy *> &drought_mitigation_policies, const int total_simulation_time,
                       const int number_of_realizations, DataCollector *data_collector) :
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations), data_collector(data_collector),
        drought_mitigation_policies(drought_mitigation_policies) {

    /// Sort water sources and utilities by their IDs.
    sort(water_sources.begin(), water_sources.end(), std::greater<>());
    sort(utilities.begin(), utilities.end(), std::greater<>());

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
    this->data_collector = new DataCollector(utilities, water_sources, drought_mitigation_policies,
                                             number_of_realizations);

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
        rof_models[r]->setWater_sources_realization(water_sources_realization);

        /// Clear vectors for reuse in the setup of next realization.
        water_sources_realization.clear();
        drought_mitigation_policies_realization.clear();
    }
}

void Simulation::runFullSimulation() {

    int n_utilities = (int) realization_models[0]->getUtilities().size();
    vector<double> risks_of_failure_week((unsigned long) n_utilities, 0.0);
    time_t timer_i;
    time_t timer_f;
    double seconds;

    /// Run realizations.
    for (int r = 0; r < number_of_realizations; ++r) {
        cout << "Realization " << r << endl;
        time(&timer_i);
        for (int w = 0; w < total_simulation_time; ++w) {
            if (isFirstWeekOfTheYear(w)) realization_models[r]->setLongTermROFs(rof_models[r]->calculateROF(w, LONG_TERM_ROF), w);
            realization_models[r]->setShortTermROFs(rof_models[r]->calculateROF(w, SHORT_TERM_ROF));
            realization_models[r]->applyDroughtMitigationPolicies(w);
            realization_models[r]->continuityStep(w);
            data_collector->collectData(realization_models[r], w);
        }
        time(&timer_f);
        seconds = difftime(timer_f,timer_i);
        cout << seconds << "s" << endl;
    }

    /// Print output files.
    data_collector->printUtilityOutput(true);
    data_collector->printReservoirOutput(true);
    data_collector->printPoliciesOutput(true);
}

bool Simulation::isFirstWeekOfTheYear(int week) {
    return (week / WEEKS_IN_YEAR - (int) (week / WEEKS_IN_YEAR)) * WEEKS_IN_YEAR < 1.0;
}

vector<double> Simulation::calculateObjectives() {

    return vector<double>();
}

double Simulation::calculateReliabilityObjective(vector<Utility_t> utilities_data) {
    unsigned long n_realizations = utilities_data.size();
    unsigned long n_weeks = utilities_data[0].combined_storage.size();
    unsigned long n_years = (unsigned long) ceil(n_weeks / WEEKS_IN_YEAR);

    vector<int> failures_per_year(n_years, 0);
    vector<int> failed_weeks(n_weeks, 0);

    for (int w = 0; w < n_weeks; ++w) {
        for (int r = 0; r < n_realizations; ++r) {
            if (utilities_data[r].combined_storage[w] < STORAGE_CAPACITY_RATIO_FAIL)
                failed_weeks[w] = FAILURE;
        }
    }

    for (int y = 0; y < n_years; ++y) {
        for (int w = (int) round(n_years * WEEKS_IN_YEAR); w < (int) round((n_years + 1) * WEEKS_IN_YEAR); ++w) {
            if (failed_weeks[w] == FAILURE) {
                failures_per_year[y] += 1.0 / n_realizations;
            }
        }
    }

    return *max_element(failures_per_year.begin(), failures_per_year.end());
}