//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "ContinuityModel.h"

Region::Region(int realization_index,
               vector<Reservoir> reservoirs,
               vector<vector<int> > &water_sources_connectivity_matrix,
               vector<Utility> utilities,
               vector<vector<int> > &water_sources_utility_connectivity_matrix,
               const int total_simulation_time) : realization_index(realization_index),
                                                  reservoirs(reservoirs),
                                                  utilities(utilities),
                                                  total_simulation_time(total_simulation_time) {

    // Assign reservoirs to each utility.
    for (int i = 0; i < utilities.size(); i++) {
        for (int j = 0; j < reservoirs.size(); j++) {
            if (water_sources_utility_connectivity_matrix[i][j] == 1) {
                this->utilities[i].addReservoir(&this->reservoirs[j]);
            }
        }
    }

    // Convert reservoir adjacency matrix into adjacency list.
    for (int j = 0; j < reservoirs.size(); j++) {
        vector<int> v;
        for (int i = 0; i < reservoirs.size(); i++) {
            if (water_sources_connectivity_matrix[i][j] == 1) v.push_back(i);
        }
        if (v.size() == 0) v.push_back(-1);
        water_sources_adjacency_list.push_back(v);
    }

    // Populate reservoir to utility adjacency list.
    for (int i = 0; i < reservoirs.size(); ++i) {
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
        for (const map<int, Reservoir *>::value_type &r : u.getReservoirs()) {
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
void Region::continuityStep(int week) {
    double demands[reservoirs.size()] = {};
    double upstream_releases_inflows[reservoirs.size()] = {};

    for (int i = 0; i < reservoirs.size(); ++i) {
        for (int &j : water_sources_utility_adjacency_list[i]) {
            demands[i] += utilities[j].getDemand(week, i);
        }
        for (int &j : water_sources_adjacency_list[i]) {
            upstream_releases_inflows[i] += reservoirs[j].getOutflow_previous_week();
        }
    }

    for (int i = 0; i < reservoirs.size(); i++) {
        reservoirs[i].updateAvailableVolume(week, upstream_releases_inflows[i], demands[i]);
    }

    for (Utility &u : utilities) {
        u.updateTotalStoredVolume();
    }
}

void Region::runSimpleContinuitySimulation(int total_simulation_time) {

    for (Utility &u : utilities) {
        u.updateTotalStoredVolume();
    }

    for (int week = 0; week < total_simulation_time; ++week) {
        this->continuityStep(week);

        cout << "====== Week: " << week << " ======" << endl;

        for (Reservoir &r : reservoirs) {
            r.toString();
        }
    }
}


