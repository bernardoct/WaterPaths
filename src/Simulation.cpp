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

    // Creates the data collector for the simulation.
    this->data_collector = new DataCollector(utilities, water_sources, number_of_realizations);

    // Creates the realization and ROF models.
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
            realization_models[r]->continuityStep(w);

            rofs = rof_models[r]->calculateROF(w);
            realization_models[r]->setRisks_of_failure(rofs);

            this->collectData(realization_models[r]);
        }
    }

    data_collector->printUtilityOutput();
    data_collector->printReservoirOutput();

}

void Simulation::collectData(ContinuityModelRealization *continuity_model_realization) {

    Utility *u;
    WaterSource *ws;
    int r = continuity_model_realization->realization_id;
    for (int i = 0; i < continuity_model_realization->getUtilities().size(); ++i) {
        u = continuity_model_realization->getUtilities()[i];
        data_collector->addUtilityCombinedStorage(r, i, u->getStorageToCapacityRatio() *
                                                        u->getTotal_storage_capacity());
        data_collector->addUtilityRof(r, i, continuity_model_realization->getRisks_of_failure()[i]);
    }

    for (int i = 0; i < continuity_model_realization->getWater_sources().size(); ++i) {
        ws = continuity_model_realization->getWater_sources()[i];
        data_collector->addReservoirAvailableVolume(r, i, ws->getAvailable_volume());
        data_collector->addReservoirDemands(r, i, ws->getDemand());
        data_collector->addReservoirUpstreamSourcesInflow(r, i, ws->getUpstream_inflow_previous_week());
        data_collector->addReservoirOutflows(r, i, ws->getOutflow_previous_week());
        data_collector->addReservoirCatchmentInflow(r, i, ws->getCatchment_inflow_previous_week());
    }
}
