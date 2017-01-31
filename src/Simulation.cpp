//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "Utils/Aux.h"

Simulation::Simulation(const vector<WaterSource *> &water_sources,
                       const vector<vector<int>> &water_sources_adjacency_matrix,
                       const vector<Utility *> &utilities,
                       const vector<vector<int>> &water_sources_utility_adjacency_matrix,
                       const int total_simulation_time, const int number_of_realizations) :
        water_sources(water_sources),
        utilities(utilities),
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations) {

    for (int r = 0; r < number_of_realizations; ++r) {
        // Creates realization models by copying the water sources and utilities.
        realization_models.push_back(new ContinuityModelRealization(Aux::copyWaterSourceVector(water_sources),
                                                                    water_sources_adjacency_matrix,
                                                                    Aux::copyUtilityVector(utilities),
                                                                    water_sources_utility_adjacency_matrix, r));

        // Creates rof models by copying the water utilities and sources.
        rof_models.push_back(new ContinuityModelROF(Aux::copyWaterSourceVector(water_sources),
                                                    water_sources_adjacency_matrix,
                                                    Aux::copyUtilityVector(utilities),
                                                    water_sources_utility_adjacency_matrix,
                                                    SHORT_TERM_ROF, r));
    }
}

void Simulation::runFullSimulation() {

    double *rofs = new double[utilities.size()];
    for (int r = 0; r < number_of_realizations; ++r) {
        for (int w = 0; w < total_simulation_time; ++w) {
            realization_models[r]->continuityStep(w);
            rofs = rof_models[r]->calculateROF(w);
            for (int i = 0; i < utilities.size(); ++i) {
                utilities[i]->recordRof(rofs[i], w);
            }
        }
    }
    delete[] rofs;

}
