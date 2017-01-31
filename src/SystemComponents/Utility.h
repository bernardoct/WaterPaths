//
// Created by bernardo on 1/13/17.
//

#ifndef TRIANGLEMODEL_UTILITY_H
#define TRIANGLEMODEL_UTILITY_H


#include <map>
#include "WaterSources/Reservoir.h"
#include "../Utils/Constants.h"

class Utility {
private:
    double *demand_series;
    double total_storage_capacity;
    double total_stored_volume;
    vector<double> rof_records;
    const char *name;
    map<int, WaterSource *> water_sources;

public:
    Utility(char const *name, int id, char const *demand_file_name, int number_of_week_demands);

    Utility(Utility &utility);

    ~Utility();

    Utility &operator=(const Utility &utility);

    const int id;
    const int number_of_week_demands;

    double getDemand(int week, int reservoir_id);

    void updateTotalStoredVolume();

    const map<int, WaterSource *> &getWaterSource() const;

    void addWaterSource(WaterSource *water_source);

    double getStorageToCapacityRatio() const;

    void recordRof(double rof, int week);
};


#endif //TRIANGLEMODEL_UTILITY_H
