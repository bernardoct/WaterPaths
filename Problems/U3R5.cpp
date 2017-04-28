//
// Created by bernardoct on 4/26/17.
//

#include <iostream>
#include "HeaderFiles/U3R5.h"
#include "../src/Utils/Utils.h"
#include "../src/DroughtMitigationInstruments/Transfers.h"
#include "Problem.cpp"

U3R5::U3R5(unsigned int seed) : Problem(seed) {
    srand(seed);

    /// Read streamflows
    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Utils::parse2DCsvFile("../TestFiles/"
                                                              "inflowsLong.csv", 2, streamflow_n_weeks);

    /// Create catchments and corresponding vectors
    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    vector<Catchment *> catchments3;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);
    catchments3.push_back(&c1);

    /// Create reservoirs and corresponding vector
    vector<double> construction_time_interval = {1.0, 4.0};
    Reservoir r1("R1", 0, 3.0, catchments1, 100.0, 20);
    Reservoir r2("R2", 1, 3.0, catchments2, 275.0, 20, 0.02, construction_time_interval, 5000, 20, 0.05);
    Reservoir r3("R3", 2, 2.0, catchments3, 400.0, 20);
    Reservoir r4("R4", 3, 3.0, catchments2, 550.0, 20);
    Reservoir r5("R5", 4, 2.0, catchments3, 900.0, 20);

    water_sources.push_back(&r1);
    water_sources.push_back(&r2);
    water_sources.push_back(&r3);
    water_sources.push_back(&r4);
    water_sources.push_back(&r5);


    /*
     * System connection diagram (water
     * flows from top to bottom)
     *
     *      0   1
     *       \ /
     *        2   4
     *         \ /
     *          3
     */

    water_sources_graph = Graph((int) water_sources.size());
    water_sources_graph.addEdge(0, 2);
    water_sources_graph.addEdge(1, 2);
    water_sources_graph.addEdge(2, 3);
    water_sources_graph.addEdge(4, 3);

    /// Create catchments and corresponding vector
    Utility u1((char *) "U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.03, 1);
    Utility u2((char *) "U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.07, 1, vector<int>(1, 1), 0.05);
    Utility u3((char *) "U3", 2, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.05, 1);

    utilities.push_back(&u1);
    utilities.push_back(&u2);
    utilities.push_back(&u3);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs). Must be sorted by utilities IDs.
    reservoir_utility_connectivity_matrix = {
            {0, 2},
            {1, 4},
            {3}
    };

    /// Restriction policies
    vector<double> restriction_stage_multipliers1 = {0.9, 0.7};
    vector<double> restriction_stage_triggers1 = {0.01, 0.04};

    vector<double> restriction_stage_multipliers2 = {0.8, 0.6};
    vector<double> restriction_stage_triggers2 = {0.03, 0.05};

    Restrictions rp1(0, restriction_stage_multipliers1, restriction_stage_triggers1);
    Restrictions rp2(1, restriction_stage_multipliers2, restriction_stage_triggers2);
    Restrictions rp3(2, restriction_stage_multipliers1, restriction_stage_triggers1);

    drought_mitigation_policies.push_back(&rp1);
    drought_mitigation_policies.push_back(&rp2);
    drought_mitigation_policies.push_back(&rp3);
//    drought_mitigation_policies = {&rp1, &rp3};

    /// Transfer policy
    /*
     *      0
     *     / \
     *  0 v   v 1
     *   /     \
     *  1--->---2
     *      2
     */

    vector<int> buyers_ids = {1, 2};
    vector<double> buyers_transfers_capacities = {3, 2.5, 2.0};
    vector<double> buyers_transfers_trigger = {0.04, 0.04};

    utilities_graph = Graph(3);
    utilities_graph.addEdge(0, 1);
    utilities_graph.addEdge(0, 2);
    utilities_graph.addEdge(1, 2);

    Transfers t(0, 0, 1, buyers_ids, buyers_transfers_capacities, buyers_transfers_trigger, utilities_graph,
                vector<double>(), vector<int>());
    drought_mitigation_policies.push_back(&t);

    /// Creates simulation object
    simulation = new Simulation(water_sources, water_sources_graph, reservoir_utility_connectivity_matrix, utilities,
                                drought_mitigation_policies,
                                992, 2, data_collector);
}

void U3R5::run() {
    cout << "Beginning U3R5 simulation" << endl;
    simulation->runFullSimulation(2);
    cout << "Ending U3R5 simulation" << endl;
}

const vector<Utility *> &U3R5::getUtilities() const {
    return utilities;
}

const vector<WaterSource *> &U3R5::getWater_sources() const {
    return water_sources;
}

const vector<DroughtMitigationPolicy *> &U3R5::getDrought_mitigation_policies() const {
    return drought_mitigation_policies;
}

const vector<vector<int>> &U3R5::getReservoir_utility_connectivity_matrix() const {
    return reservoir_utility_connectivity_matrix;
}

const Graph &U3R5::getWater_sources_graph() const {
    return water_sources_graph;
}

const Graph &U3R5::getUtilities_graph() const {
    return utilities_graph;
}

