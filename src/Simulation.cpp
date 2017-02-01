//
// Created by bernardo on 1/25/17.
//

#include "Simulation.h"
#include "Utils/Aux.h"
#include <iostream>

Simulation::Simulation(const vector<WaterSource *> &water_sources,
                       const vector<vector<int>> &water_sources_adjacency_matrix,
                       const vector<Utility *> &utilities,
                       const vector<vector<int>> &water_sources_utility_adjacency_matrix,
                       const int total_simulation_time, const int number_of_realizations,
                       DataCollector * data_collector) :
        total_simulation_time(total_simulation_time),
        number_of_realizations(number_of_realizations), data_collector(data_collector) {

    vector<WaterSource *> water_sources_realization;

    for (int r = 0; r < number_of_realizations; ++r) {
        // Creates realization models by copying the water sources and utilities.
        water_sources_realization = Aux::copyWaterSourceVector(water_sources);
        realization_models.push_back(new ContinuityModelRealization(water_sources_realization,
                                                                    water_sources_adjacency_matrix,
                                                                    Aux::copyUtilityVector(utilities),
                                                                    water_sources_utility_adjacency_matrix, r));

        // Creates rof models by copying the water utilities and sources.
        rof_models.push_back(new ContinuityModelROF(Aux::copyWaterSourceVector(water_sources),
                                                    water_sources_adjacency_matrix,
                                                    Aux::copyUtilityVector(utilities),
                                                    water_sources_utility_adjacency_matrix,
                                                    SHORT_TERM_ROF, r));

        rof_models[r]->setWater_sources_realization(water_sources_realization);
        water_sources_realization.clear();
    }
}

void Simulation::runFullSimulation() {

    int n_utilities = (int) realization_models[0]->getUtilities().size();
    vector<double> rofs((unsigned long) n_utilities, 0.0);
    for (int r = 0; r < number_of_realizations; ++r) {
        cout << "Realization: " << r << endl;
        for (int w = 0; w < total_simulation_time; ++w) {
//            cout << "week: " << w << " ";
            realization_models[r]->continuityStep(w);
            rofs = rof_models[r]->calculateROF(w);

            for (int i = 0; i < n_utilities; ++i) {
                cout << realization_models[r]->getUtilities()[i]->getStorageToCapacityRatio()
                     << " " << rofs[i] << " ";
            }
        }
    }

}

void collectData(ContinuityModelRealization continuity_model_realization) {

}
