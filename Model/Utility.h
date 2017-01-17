//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include "Reservoir.h"
#include "Constants.h"

class Utility {
private:
    double *demand_series;
    double total_storage_capacity;
    double current_total_demand;
    double total_stored_volume;
    const char *name;
    map<int, Reservoir> reservoirs;

public:
    const map<int, Reservoir> &getReservoirs() const;

public:
    Utility(char const *name, int id, char const *demand_file_name, int number_of_weeks);

    double getDemand(int reservoir_id);

    const int id;

    void updateCurrentDemandAndTotalStoredVolume(int week, double inflow);

    void assignReservoirs(map<int, Reservoir> reservoirs_);
};


#endif //TRIANGLEMODEL_UTILITY_H
