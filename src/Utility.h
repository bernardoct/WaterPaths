//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include "WaterSources/Reservoir.h"
#include "Constants.h"

class Utility {
private:
    double *demand_series;
    double total_storage_capacity;
    double total_stored_volume;
    double risk_of_failure;
    const char *name;
    map<int, WaterSource *> water_sources;

public:
    Utility(char const *name, int id, char const *demand_file_name, int number_of_weeks);

    double getDemand(int week, int reservoir_id);

    const int id;

    void updateTotalStoredVolume();

    const map<int, WaterSource *> &getWaterSource() const;

    void addWaterSource(WaterSource *water_source);
};


#endif //TRIANGLEMODEL_UTILITY_H
