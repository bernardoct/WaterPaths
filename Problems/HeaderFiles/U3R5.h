//
// Created by bernardoct on 4/26/17.
//

#ifndef TRIANGLEMODEL_U3R5_H
#define TRIANGLEMODEL_U3R5_H


#include "../../src/SystemComponents/Catchment.h"
#include "../../src/SystemComponents/Utility.h"
#include "../../src/DroughtMitigationInstruments/DroughtMitigationPolicy.h"
#include "../../src/Utils/DataCollector.h"
#include "../../src/Simulation/Simulation.h"
#include "Problem.h"

class U3R5 : public Problem {
public:
    U3R5(unsigned int seed);

    void run();

private:

    vector<Utility *> utilities;
    vector<WaterSource *> water_sources;
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    vector<vector<int>> reservoir_utility_connectivity_matrix;
    Graph water_sources_graph;
    Graph utilities_graph;
    DataCollector *data_collector;

public:
    const vector<Utility *> &getUtilities() const;

    const vector<WaterSource *> &getWater_sources() const;

    const vector<DroughtMitigationPolicy *> &getDrought_mitigation_policies() const;

    const vector<vector<int>> &getReservoir_utility_connectivity_matrix() const;

    const Graph &getWater_sources_graph() const;

    const Graph &getUtilities_graph() const;
};


#endif //TRIANGLEMODEL_U3R5_H
