//
// Created by bernardo on 2/1/17.
//

#include "DataColector.h"

DataColector::DataColector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources) {
    for (Utility *u : utilities) {
        utilities_t.push_back(*new Utility_t(u->id, u->getTotal_storage_capacity()));
    }
    for (WaterSource *r : water_sources) {
        reservoir_t.push_back(*new Reservoir_t(r->id, r->getCapacity()));
    }
}


void DataColector::setUtilityRof(int i, double rof) {

}

void DataColector::setUtilityCombinedStorage(int i, double volume) {

}

void DataColector::setReservoirAvailableVolume(int i, double volume) {

}

