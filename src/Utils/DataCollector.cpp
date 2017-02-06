//
// Created by bct52 on 2/1/17.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include "DataCollector.h"


DataCollector::DataCollector(const vector<Utility *> &utilities, const vector<WaterSource *> &water_sources,
                             int number_of_realizations) {
    for (int r = 0; r < number_of_realizations; ++r) {
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

void DataCollector::collectData(ContinuityModelRealization *continuity_model_realization) {

    Utility *u;
    WaterSource *ws;
    int r = continuity_model_realization->realization_id;
    for (int i = 0; i < continuity_model_realization->getUtilities().size(); ++i) {
        u = continuity_model_realization->getUtilities()[i];
        this->addUtilityCombinedStorage(r, i, u->getStorageToCapacityRatio() *
                                              u->getTotal_storage_capacity());
        this->addUtilityRof(r, i, continuity_model_realization->getRisks_of_failure()[i]);
    }

    for (int i = 0; i < continuity_model_realization->getWater_sources().size(); ++i) {
        ws = continuity_model_realization->getWater_sources()[i];
        this->addReservoirAvailableVolume(r, i, ws->getAvailable_volume());
        this->addReservoirDemands(r, i, ws->getDemand());
        this->addReservoirUpstreamSourcesInflow(r, i, ws->getUpstream_source_inflow());
        this->addReservoirOutflows(r, i, ws->getTotal_inflow());
        this->addReservoirCatchmentInflow(r, i, ws->getCatchment_upstream_catchment_inflow());
    }
}

void DataCollector::printReservoirOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < reservoir_t.size(); ++r) {
        outStream << endl;
        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            outStream << reservoir_t[r][i].name << " " << endl;
        }
        for (int i = 0; i < reservoir_t[r].size(); ++i) {
            outStream
//                    << " Av_volume"
//                    << "    Demands"
//                    << "   Up_spill"
//                    << "  Catc_infl"
//                    << "   Outflows";
                    << "Av_volume,"
                    << "Demands,"
                    << "Up_spill,"
                    << "Catc_infl,"
                    << "Outflows,";
        }
        outStream << endl;
        for (int w = 0; w < reservoir_t[0][0].available_volume.size(); ++w) {

            for (int i = 0; i < reservoir_t[r].size(); ++i) {
                outStream
//                        << setw(10) << setprecision(4) << reservoir_t[r][i].available_volume[w]
//                        << setw(10) << setprecision(4) << reservoir_t[r][i].demands[w]
//                        << setw(10) << setprecision(4) << reservoir_t[r][i].total_upstream_sources_inflows[w]
//                        << setw(10) << setprecision(4) << reservoir_t[r][i].total_catchments_inflow[w]
//                        << setw(10) << setprecision(4) << reservoir_t[r][i].outflows[w];
                        << setprecision(4) << reservoir_t[r][i].available_volume[w] << ", "
                        << setprecision(4) << reservoir_t[r][i].demands[w] << ", "
                        << setprecision(4) << reservoir_t[r][i].total_upstream_sources_inflows[w] << ", "
                        << setprecision(4) << reservoir_t[r][i].total_catchments_inflow[w] << ", "
                        << setprecision(4) << reservoir_t[r][i].outflows[w] << ", ";
            }
            outStream << endl;
        }
    }

    outStream.close();
}

void DataCollector::printUtilityOutput(bool toFile, string fileName) {

    std::ofstream outStream;
    outStream.open(output_directory + fileName);

    for (int r = 0; r < utilities_t.size(); ++r) {
        outStream << endl;
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << utilities_t[r][i].name << " " << endl;
        }
        for (int i = 0; i < utilities_t[r].size(); ++i) {
            outStream << "St_vol, rof, ";
        }
        outStream << endl;
        for (int w = 0; w < utilities_t[0][0].rof.size(); ++w) {

            for (int i = 0; i < utilities_t[r].size(); ++i) {
                outStream << utilities_t[r][i].combined_storage[w] << ", " << utilities_t[r][i].rof[w] << ", ";
            }
            outStream << endl;
        }
    }

    outStream.close();
}

