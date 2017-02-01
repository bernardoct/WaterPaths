//
// Created by bernardo on 2/1/17.
//

#ifndef TRIANGLEMODEL_DATACOLECTOR_H
#define TRIANGLEMODEL_DATACOLECTOR_H

#include <vector>
#include "SystemComponents/Utility.h"
#include "SystemComponents/WaterSources/Reservoir.h"
#include <vector>

struct Utility_t {
    Utility_t(int id, double capacity) : id(id), capacity(capacity) {};

    double *rof;
    double *combined_storage;
    double capacity;
    int id;
};

struct Reservoir_t {
    Reservoir_t(int id, double capacity) : id(id), capacity(capacity) {};

    double *available_volume;
    double capacity;
    int id;
};

class DataColector {

private:
    vector<Utility_t> utilities_t;
    vector<Reservoir_t> reservoir_t;

public:
    DataColector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources);

    void setUtilityRof(int i, double rof);

    void setUtilityCombinedStorage(int i, double volume);

    void setReservoirAvailableVolume(int i, double volume);
};


#endif //TRIANGLEMODEL_DATACOLECTOR_H
