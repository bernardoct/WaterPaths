//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "ContinuityModel.h"

ContinuityModel::ContinuityModel(int realization_index,
                                 vector<Reservoir> water_sources,
                                 vector<vector<int> > &water_sources_connectivity_matrix,
                                 vector<Utility> utilities,
                                 vector<vector<int> > &water_sources_utility_connectivity_matrix,
                                 const int total_simulation_time) : realization_index(realization_index),
                                                                    water_sources(water_sources),
                                                                    utilities(utilities),
                                                                    total_simulation_time(total_simulation_time) {

    // Assign water_sources to each utility.
    for (int i = 0; i < utilities.size(); i++) {
        for (int j = 0; j < water_sources.size(); j++) {
            if (water_sources_utility_connectivity_matrix[i][j] == 1) {
                this->utilities[i].addWaterSource(&this->water_sources[j]);
            }
        }
    }

    // Convert reservoir adjacency matrix into adjacency list.
    for (int j = 0; j < water_sources.size(); j++) {
        vector<int> v;
        for (int i = 0; i < water_sources.size(); i++) {
            if (water_sources_connectivity_matrix[i][j] == 1) v.push_back(i);
        }
        if (v.size() == 0) v.push_back(-1);
        water_sources_adjacency_list.push_back(v);
    }

    // Populate reservoir to utility adjacency list.
    for (int i = 0; i < water_sources.size(); ++i) {
        vector<int> v;
        for (int j = 0; j < utilities.size(); ++j) {
            if (water_sources_utility_connectivity_matrix[j][i] == 1) {
                v.push_back(this->utilities.at(j).id);
            }
        }
        water_sources_utility_adjacency_list.push_back(v);
    }

    // Print reservoir adjacency list. FOR TESTS ONLY.
    cout << endl << "Reservoir connections." << endl;
    for (int k = 0; k < water_sources_adjacency_list.size(); ++k) {
        vector<int> row = water_sources_adjacency_list[k];
        cout << "Reservoir " << k << ": ";
        for (int &i : row) {
            cout << i << " ";
        }
        cout << endl;
    }


    // Print list of reservoir for each utility. FOR TESTS ONLY.
    cout << endl << "Reservoirs belonging to each utility." << endl;
    for (Utility &u : this->utilities) {
        cout << "Utility " << u.id << ": ";
        for (const map<int, WaterSource *>::value_type &r : u.getWaterSource()) {
            cout << r.second->id << " ";
        }
        cout << endl;
    }

    cout << endl;

    // Print list of utilities drawing from each reservoir. FOR TESTS ONLY.
    cout << endl << "Utilities drawing from reservoir." << endl;
    for (int i = 0; i < water_sources_utility_adjacency_list.size(); ++i) {
        cout << "Reservoir " << i << ": ";
        for (int &id : water_sources_utility_adjacency_list[i]) {
            cout << id << " ";
        }
        cout << endl;
    }
    cout << endl;

}

/**
 *
 * @param week
 */
void ContinuityModel::continuityStep(int week) {
    double demands[water_sources.size()] = {};
    double upstream_releases_inflows[water_sources.size()] = {};

    for (int i = 0; i < water_sources.size(); ++i) {
        for (int &j : water_sources_utility_adjacency_list[i]) {
            demands[i] += utilities[j].getDemand(week, i);
        }
        for (int &j : water_sources_adjacency_list[i]) {
            upstream_releases_inflows[i] += water_sources[j].getOutflow_previous_week();
        }
    }

    for (int i = 0; i < water_sources.size(); i++) {
        water_sources[i].updateAvailableVolume(week, upstream_releases_inflows[i], demands[i]);
    }

    for (Utility &u : utilities) {
        u.updateTotalStoredVolume();
    }
}

void ContinuityModel::runSimpleContinuitySimulation(int total_simulation_time) {

    for (Utility &u : utilities) {
        u.updateTotalStoredVolume();
    }

    for (int week = 0; week < total_simulation_time; ++week) {
        this->continuityStep(week);

        cout << "====== Week: " << week << " ======" << endl;

        for (WaterSource &r : water_sources) {
            r.toString();
        }
    }
}


