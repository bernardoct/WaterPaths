//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Region.h"

Region::Region(int realization_index,
               vector<Reservoir> &reservoirs,
               vector<vector<int> > reservoir_connectivity_matrix_,
               vector<Utility> &utilities,
               vector<vector<int> > reservoir_utility_connectivity_matrix_,
               const int total_simulation_time) : realization_index(realization_index),
                                                  reservoirs(reservoirs),
                                                  reservoir_adjacency_matrix(reservoir_connectivity_matrix_),
                                                  utilities(utilities),
                                                  reservoir_utility_connectivity_matrix(
                                                          reservoir_utility_connectivity_matrix_),
                                                  total_simulation_time(total_simulation_time) {

    // Convert reservoir adjacency matrix into adjacency list.
    for (int i = 0; i < reservoirs.size(); i++) {
        vector<int> v;
        for (int j = 0; j < reservoirs.size(); j++) {
            if (reservoir_connectivity_matrix_[i][j] == 1) v.push_back(j);
        }
        reservoir_adjacency_list.push_back(v);
    }

    // Assign reservoirs to each utility.
    map<int, Reservoir> *reservoirs_belonging_to_utility;
    for (int i = 0; i < utilities.size(); i++) {
        reservoirs_belonging_to_utility = new map<int, Reservoir>;
        for (int j = 0; j < reservoirs.size(); j++) {
            if (reservoir_utility_connectivity_matrix_[i][j] == 1) {
                reservoirs_belonging_to_utility->insert(pair<int, Reservoir>(j, reservoirs[j]));
            }
        }
        utilities[i].assignReservoirs(*reservoirs_belonging_to_utility);
    }

    // Populate reservoir to utility adjacency matrix.
    for (int i = 0; i < reservoirs.size(); ++i) {
        vector<int> v;
        for (int j = 0; j < utilities.size(); ++j) {
            if (reservoir_utility_connectivity_matrix_[j][i] == 1) {
                v.push_back(utilities.at(j).id);
            }
        }
        reservoir_utility_adjacency_list.push_back(v);
    }

    // Print reservoir adjacency list. FOR TESTS ONLY.
    for (auto &row : reservoir_adjacency_list) {
        for (int &i : row) {
            cout << i << " ";
        }
        cout << endl;
    }


    // Print list of reservoir for each utility. FOR TESTS ONLY.
    for (Utility &u : utilities) {
        for (auto &r : u.getReservoirs()) {
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

void Region::continuity_step(int week) {
    double demands[reservoirs.size()];
    double inflows[reservoirs.size()];

    for (int i = 0; i < reservoirs.size(); ++i) {

    }
}
