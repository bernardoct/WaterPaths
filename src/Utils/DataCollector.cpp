//
// Created by bct52 on 2/1/17.
//

#include "DataCollector.h"


DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources) {
    // now this is a vector of vectors (realization id and element id). Fix accordingly.
    for (Utility *u : utilities) {
        utilities_t.push_back(*new Utility_t(u->id, u->getTotal_storage_capacity()));
    }
    for (WaterSource *r : water_sources) {
        reservoir_t.push_back(*new Reservoir_t(r->id, r->getCapacity()));
    }
}


void DataCollector::addUtilityRof(int realization_index, int utility_index, double rof) {
    utilities_t[realization_index][utility_index].rof.push_back(rof);
}

void DataCollector::addUtilityCombinedStorage(int realization_index, int utility_index, double volume) {
    utilities_t[realization_index][utility_index].combined_storage.push_back(volume);
}

void DataCollector::addReservoirAvailableVolume(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].available_volume.push_back(volume);
}

void DataCollector::addReservoirInflow(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].inflows.push_back(volume);
}

void DataCollector::addReservoirOutflows(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].outflows.push_back(volume);
}

void DataCollector::addReservoirDemands(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].demands.push_back(volume);
}
