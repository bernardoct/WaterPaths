//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Region.h"

Region::Region(int realization_index,
               vector<Reservoir> reservoirs,
               vector<vector<int> > reservoir_connectivity_matrix_,
               vector<Utility> utilities,
               vector<vector<int> > reservoir_utility_connectivity_matrix_,
               const int total_simulation_time) : realization_index(realization_index),
                                                  reservoirs(reservoirs),
                                                  reservoir_adjacency_matrix(reservoir_connectivity_matrix_),
                                                  utilities(utilities),
                                                  reservoir_utility_connectivity_matrix(
                                                          reservoir_utility_connectivity_matrix_),
                                                  total_simulation_time(total_simulation_time) {

    // Assign reservoirs to each utility.
    for (int i = 0; i < utilities.size(); i++) {
        for (int j = 0; j < reservoirs.size(); j++) {
            if (reservoir_utility_connectivity_matrix_[i][j] == 1) {
                this->utilities[i].addReservoir(reservoirs[j]);
            }
        }
    }

    // Convert reservoir adjacency matrix into adjacency list.
    for (int i = 0; i < reservoirs.size(); i++) {
        vector<int> v;
        for (int j = 0; j < reservoirs.size(); j++) {
            if (reservoir_connectivity_matrix_[i][j] == 1) v.push_back(j);
        }
        reservoir_adjacency_list.push_back(v);
    }

    // Populate reservoir to utility adjacency list.
    for (int i = 0; i < reservoirs.size(); ++i) {
        vector<int> v;
        for (int j = 0; j < utilities.size(); ++j) {
            if (reservoir_utility_connectivity_matrix_[j][i] == 1) {
                v.push_back(this->utilities.at(j).id);
            }
        }
        reservoir_utility_adjacency_list.push_back(v);
    }

    // Print reservoir adjacency list. FOR TESTS ONLY.
    for (vector<int> &row : reservoir_adjacency_list) {
        for (int &i : row) {
            cout << i << " ";
        }
        cout << endl;
    }


    // Print list of reservoir for each utility. FOR TESTS ONLY.
    for (Utility &u : this->utilities) {
        for (const map<int, Reservoir>::value_type &r : u.getReservoirs()) {
            cout << r.second.id << " ";
        }
        cout << endl;
    }

    cout << endl;

    // Print list of utilities drawing from each reservoir. FOR TESTS ONLY.
    for (vector<int> &v : reservoir_utility_adjacency_list) {
        for (int &id : v) {
            cout << id << " ";
        }
        cout << endl;
    }

}

void Region::continuityStep(int week) {
    double demands[reservoirs.size()] = {};
    double inflows[reservoirs.size()] = {};

    for (Utility &u : utilities) {
        u.updateCurrentDemandAndTotalStoredVolume(week);
    }

    for (int i = 0; i < reservoirs.size(); ++i) {
        for (int &j : reservoir_utility_adjacency_list[i]) {
            demands[i] += utilities[j].getDemand(i);
        }
        for (int &j : reservoir_adjacency_list[i]) {
            inflows[i] += reservoirs[j].getRelease_previous_week();
        }
    }

    for (int i = 0; i < reservoirs.size(); i++) {
        reservoirs[i].applyContinuity(week, inflows[i], demands[i]);
    }
}

void Region::runSimpleContinuitySimulation(int total_simulation_time) {
    for (int week = 0; week < total_simulation_time; ++week) {
        this->continuityStep(week);
    }
}


