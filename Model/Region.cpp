//
// Created by bernardo on 1/12/17.
//

#include <iostream>
#include "Region.h"

Region::Region(const vector<Reservoir> &reservoirs,
               int (&reservoir_connectivity_matrix_)[MAX_NUMBER_OF_RESERVOIRS][MAX_NUMBER_OF_RESERVOIRS],
               const int simulation_time) : reservoirs(reservoirs), simulation_time(simulation_time) {

    for (int i = 0; i < MAX_NUMBER_OF_RESERVOIRS; i++) {
        for (int j = 0; j < MAX_NUMBER_OF_RESERVOIRS; j++) {
            reservoir_connectivity_matrix[i][j] = reservoir_connectivity_matrix_[i][j];
        }
    }

    for (int i = 0; i < reservoirs.size(); i++) {
        for (int j = 0; j < reservoirs.size(); j++) {
            cout << reservoir_connectivity_matrix[i][j] << endl;
        }
    }

}

void Region::continuity_step() {

}
