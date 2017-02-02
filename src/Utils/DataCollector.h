//
// Created by bct52 on 2/1/17.
//

#ifndef TRIANGLEMODEL_DATACOLLECTOR_H
#define TRIANGLEMODEL_DATACOLLECTOR_H

#include <vector>
#include "../SystemComponents/Utility.h"

struct Utility_t {
    Utility_t(int id, double capacity, string name) : id(id), capacity(capacity) {};

    vector<double> rof;
    vector<double> combined_storage;
    double capacity;
    int id;
    string name;
};

struct Reservoir_t {
    Reservoir_t(int id, double capacity, string name) : id(id), capacity(capacity) {};

    vector<double> available_volume;
    vector<double> total_upstream_sources_inflows;
    vector<double> demands;
    vector<double> outflows;
    vector<double> total_catchments_inflow;
    double capacity;
    int id;
    string name;
};

class DataCollector {

private:
    vector<vector<Utility_t>> utilities_t;
    vector<vector<Reservoir_t>> reservoir_t;

public:
    DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                  int number_of_relizations);

    void addUtilityRof(int realization_index, int utility_index, double rof);

    void addUtilityCombinedStorage(int realization_index, int utility_index, double volume);

    void addReservoirUpstreamSourcesInflow(int realization_index, int utility_index, double volume);

    void addReservoirOutflows(int realization_index, int utility_index, double volume);

    void addReservoirDemands(int realization_index, int utility_index, double volume);

    void addReservoirAvailableVolume(int realization_index, int reservoir_index, double volume);

    void addReservoirCatchmentInflow(int realization_index, int reservoir_index, double volume);

    void printUtilityOutput();

    void printReservoirOutput();
};


#endif //TRIANGLEMODEL_DATACOLLECTOR_H
