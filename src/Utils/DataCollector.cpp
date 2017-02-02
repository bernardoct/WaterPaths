//
// Created by bct52 on 2/1/17.
//

#include <iostream>
#include <iomanip>
#include "DataCollector.h"


DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                             int number_of_relizations) {
    for (int r = 0; r < number_of_relizations; ++r) {
        utilities_t.push_back(vector<Utility_t>());
        reservoir_t.push_back(vector<Reservoir_t>());
        for (Utility *u : utilities) {
            utilities_t[r].push_back(*new Utility_t(u->id, u->getTotal_storage_capacity(), u->name));
        }
        for (WaterSource *ws : water_sources) {
            reservoir_t[r].push_back(*new Reservoir_t(ws->id, ws->getCapacity(), ws->name));
        }
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

void DataCollector::addReservoirUpstreamSourcesInflow(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].total_upstream_sources_inflows.push_back(volume);
}

void DataCollector::addReservoirOutflows(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].outflows.push_back(volume);
}

void DataCollector::addReservoirDemands(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].demands.push_back(volume);
}

void DataCollector::addReservoirCatchmentInflow(int realization_index, int reservoir_index, double volume) {
    reservoir_t[realization_index][reservoir_index].total_catchments_inflow.push_back(volume);
}

void DataCollector::printUtilityOutput() {
    for (int r = 0; r < utilities_t.size(); ++r) {
        cout << endl;
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            cout << utilities_t[r][i].name << " " << endl;
        }
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            cout << "St_vol rof ";
        }
        cout << endl;
        for (int w = 0; w < utilities_t[0][0].rof.size(); ++w) {

            for (int i = 0; i < utilities_t[r].size(); ++i) {
                cout << utilities_t[r][i].combined_storage[w] << " " << utilities_t[r][i].rof[w] << " ";
            }
            cout << endl;
        }
    }

}

void DataCollector::printReservoirOutput() {
    for (int r = 0; r < reservoir_t.size(); ++r) {
        cout << endl;
        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            cout << reservoir_t[r][i].name << " " << endl;
        }
        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            cout
                    << " Av_volume"
                    << "    Demands"
                    << "  Sour_infl"
                    << "  Catc_infl"
                    << "   Outflows";
        }
        cout << endl;
        for (int w = 0; w < reservoir_t[0][0].available_volume.size(); ++w) {

            for (int i = 0; i < reservoir_t[r].size(); ++i) {
                cout
                        << setw(10) << setprecision(4) << reservoir_t[r][i].available_volume[w] << " "
                        << setw(10) << setprecision(4) << reservoir_t[r][i].demands[w] << " "
                        << setw(10) << setprecision(4) << reservoir_t[r][i].total_upstream_sources_inflows[w] << " "
                        << setw(10) << setprecision(4) << reservoir_t[r][i].total_catchments_inflow[w] << " "
                        << setw(10) << setprecision(4) << reservoir_t[r][i].outflows[w] << " ";
            }
            cout << endl;
        }
    }

}
