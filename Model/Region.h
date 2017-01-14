//
// Created by bernardo on 1/12/17.
//

#ifndef TRIANGLEMODEL_REGION_H
#define TRIANGLEMODEL_REGION_H

#include "Reservoir.h"
#include "Constants.h"
#include <vector>

using namespace Constants;
using namespace std;

class Region {
public:
    Region(const vector<Reservoir> &reservoirs,
           int (&reservoir_connectivity_matrix_)[MAX_NUMBER_OF_RESERVOIRS][MAX_NUMBER_OF_RESERVOIRS],
           const int simulation_time);

    void continuity_step();

private:
    vector<Reservoir> reservoirs;
    int reservoir_connectivity_matrix[MAX_NUMBER_OF_RESERVOIRS][MAX_NUMBER_OF_RESERVOIRS];
    const int simulation_time;

};


#endif //TRIANGLEMODEL_REGION_H
