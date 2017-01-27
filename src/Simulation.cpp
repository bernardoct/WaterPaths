//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"

Simulation::Simulation(const vector<WaterSource *> &water_sources,
                       const vector<vector<int>> &water_sources_adjacency_matrix,
                       const vector<Utility *> &utilities,
                       const vector<vector<int>> &water_sources_utility_adjacency_matrix,
                       const int total_simulation_time, const int number_of_realizations) : water_sources(
        water_sources),
                                                                                            utilities(utilities),
                                                                                            total_simulation_time(
                                                                                                    total_simulation_time),
                                                                                            number_of_realizations(
                                                                                                    number_of_realizations) {

    ContinuityModelRealization *cmr;
    for (int r = 0; r < number_of_realizations; ++r) {
        cmr = new ContinuityModelRealization(water_sources, water_sources_adjacency_matrix,
                                             utilities, water_sources_utility_adjacency_matrix, r);
        realization_models.push_back(cmr);
    }
}

void Simulation::runFullSimulation() {

    for (int r = 0; r < number_of_realizations; ++r) {
        for (Utility *u : utilities) {
            u->updateTotalStoredVolume();
        }

        for (int w = 0; w < total_simulation_time; ++w) {
            realization_models[r]->continuityStep(w);
        }
    }

}

vector<double> calculateRisksOfFailure(ContinuityModel realization_regional_model) {

}