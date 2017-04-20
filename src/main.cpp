#include <iostream>
#include <algorithm>
#include "SystemComponents/WaterSources/Reservoir.h"
#include "SystemComponents/Utility.h"
#include "Utils/Utils.h"
#include "Simulation/Simulation.h"
#include "Utils/QPSolver/QuadProg++.hh"
#include "DroughtMitigationInstruments/Transfers.h"


int regionOneUtilitiesTwoReservoirsContinuityTest();

void simulation3U5RTest();

using namespace std;
using namespace Constants;
/*
int reservoirAndCatchmentTest() {
    cout << "BEGINNING RESERVOIR TEST" << endl << endl;

    double **streamflows_test = Utils::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
                                                            "inflowsLakeTest.csv", 2, 52);
    Catchment catchment_test(streamflows_test[0]);

    Reservoir r(string("Lake Michie"), 0, 0.5, catchment_test, ONLINE, 10.0);

    r.continuityWaterSource(0, 2.5, 2.0);
    cout << r.getTotal_outflow() << " expected: " << 1.0 << endl;
    cout << r.getAvailable_volume() << " expected: " << 10 << endl;

    cout << endl << "END OF RESERVOIR TEST" << endl << "------------------------------------" << endl << endl;

    return 1;
}*/

/*
int testReadCsv() {

    cout << "BEGINNING CSV READ TEST" << endl << endl;

    // 2D
    cout << "2-D " << endl;
    char const *file_name_2d = "/home/bernardo/ClionProjects/TriangleModel/TestFiles/inflowsLakeTest.csv";
    double **test_data_2d = Utils::parse2DCsvFile(file_name_2d, 2, 5);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            cout << test_data_2d[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    // 1D
    cout << "1-D " << endl;
    char const *file_name_1d = "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv";
    double *test_data_1d = Utils::parse1DCsvFile(file_name_1d, 52);

    for (int i = 0; i < 5; i++) {
        cout << test_data_1d[i] << endl;
    }

    cout << endl << "END OF CSV READ TEST" << endl << "------------------------------------" << endl << endl;

    return 0;
}
*/

/**
 * This test checks is reservoirs and utilities are properly linked and sending and receiving water
 * to and from the right places.
 * @return
 */
/*
int regionTwoUtilitiesTwoReservoirsContinuityTest() {

    cout << "BEGINNING 2 RESERVOIRS 2 UTILITIES READ TEST" << endl << endl;

    int streamflow_n_weeks = 2652;
    double **streamflows_test = Utils::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);

    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0, 0);
    Reservoir r2("R2", 1, 0.5, catchments2, ONLINE, 10.0, 0);

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks, 0, 0,
               vector<int>());
    Utility u2("U2", 1, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks, 0, 0,
               vector<int>());

    vector<WaterSource *> reservoirs;
    reservoirs.push_back(&r1);
    reservoirs.push_back(&r2);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);

    Graph g((int) reservoirs.size());
    g.addEdge(0, 1);

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0},
            {1}
    };

    vector<double> restriction_stage_multipliers = {0.7, 0.9};
    vector<double> restriction_stage_triggers = {1.1, 1.1};

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, vector<int>(1));
    Restrictions rp2(0, restriction_stage_triggers, restriction_stage_multipliers, vector<int>(2));

    vector<DroughtMitigationPolicy *> restrictions = {&rp1, &rp2};

    DataCollector *data_collector = nullptr;

    Simulation s(reservoirs, g, reservoir_utility_connectivity_matrix, utilities, restrictions, 52, 1, data_collector);
    s.runFullSimulation();

    cout << endl << "END OF 2 RESERVOIRS 2 UTILITIES TEST" << endl << "---------------------"
            "---------------" << endl << endl;

    return 0;
}
 */

/**
 * This test is to check if the demand of a utility is properly split between reservoirs.
 * Another test will have to be created for when other water sources (i.g. intakes) are added.
 * @return
 */
/*
int regionOneUtilitiesTwoReservoirsContinuityTest() {

    cout << "BEGINNING 2 RESERVOIRS 1 UTILITY READ TEST" << endl << endl;
    int streamflow_n_weeks = 2652;
    double **streamflows_test = Utils::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);


    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0, 0);
    Reservoir r2("R2", 1, 0.5, catchments2, ONLINE, 10.0, 0);

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks, 0, 0,
               vector<int>());

    vector<WaterSource *> reservoirs;
    reservoirs.push_back(&r1);
    reservoirs.push_back(&r2);

    vector<Utility *> utilities;
    utilities.push_back(&u1);

    Graph g((int) reservoirs.size());
    g.addEdge(0, 1);

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 1},
    };

    vector<double> restriction_stage_multipliers = {0.7, 0.9};
    vector<double> restriction_stage_triggers = {1.1, 1.1};

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, vector<int>(1));

    vector<DroughtMitigationPolicy *> restrictions = {&rp1};

    DataCollector *data_collector = nullptr;

    Simulation s(reservoirs, g, reservoir_utility_connectivity_matrix, utilities, restrictions, 52, 1, data_collector);
    s.runFullSimulation();



    cout << "Check results with spreadsheet ContinuityTest.ods" << endl;
    cout << endl << "END OF 2 RESERVOIRS 1 UTILITY TEST" << endl << "---------------------"
            "---------------" << endl << endl;

    return 0;
}
 */

/*
void catchmentCopy() {
    cout << "BEGINNING CATCHMENT COPY READ TEST" << endl << endl;
    int streamflow_n_weeks = 52;
    double **streamflows_test = Utils::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);

    Catchment c2(c1);

    c2.setStreamflow(1, 70);

    for (int i = 0; i < 5; ++i) {
        cout << c1.getStreamflow(i) << endl;
    }

    cout << " " << endl;

    for (int i = 0; i < 5; ++i) {
        cout << c2.getStreamflow(i) << endl;
    }

    Catchment * c3 = new Catchment(c1);
    cout << " " << endl;

    c3->setStreamflow(1, 20);

    for (int i = 0; i < 5; ++i) {
        cout << c2.getStreamflow(i) << endl;
    }

    cout << " " << endl;

    for (int i = 0; i < 5; ++i) {
        cout << c3->getStreamflow(i) << endl;
    }
}
 */

/*
void reservoirCopy() {
    cout << "BEGINNING RESERVOIR COPY TEST" << endl << endl;
    int streamflow_n_weeks = 52;
    double **streamflows_test = Utils::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    vector<Catchment *> catchments1;
    catchments1.push_back(&c1);

    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0);
//    Reservoir r2(r1);
    Reservoir r2 = r1;

    r2.catchments[0]->streamflows[0] = 50;

    cout << r1.catchments[0]->getStreamflow(0) << endl; // should be 0.5
    cout << r2.catchments[0]->getStreamflow(0) << endl; // should be 50

    r1.continuityWaterSource(1, 2.0, 3.0);

    r1.toString(); // Should show stored volume of 14.5.
    r2.toString(); // Should show stored volume of 15.0.


    cout << "END OF RESERVOIR COPY TEST" << endl << endl;
}
 */

/*
void utilityCopy() {
    cout << "BEGINNING 2 RESERVOIRS 1 UTILITY READ TEST" << endl << endl;
    int streamflow_n_weeks = 52;
    double **streamflows_test = Utils::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);


    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0);
    Reservoir r2("R2", 1, 0.5, catchments2, ONLINE, 10.0);

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks);

    vector<WaterSource *> reservoirs;
    reservoirs.push_back(&r1);
    reservoirs.push_back(&r2);

    vector<Utility *> utilities;
    utilities.push_back(&u1);

    vector<vector<int>> reservoir_connectivity_matrix = {
            {0,  1},
            {-1, 0},
    };

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {1, 1},
    };

    ContinuityModelRealization cmr(reservoirs, reservoir_connectivity_matrix,
                               utilities, reservoir_utility_connectivity_matrix, 0);

    vector<WaterSource* > ws = cmr.getWaterSources();
    vector<Utility* > u = cmr.getUtilities();

//    Utility u3(*u[0]);
    Utility u3 = u1;
    u3.demand_series[0] = 30;
    u3.total_stored_volume = 5;
    u3.getWaterSource().at(0)->name[0] = 'a'; // remove const from name declaration in WaterSource.h

    cout << u[0]->demand_series[0] << " " << u3.demand_series[0] << endl; // 2 30
    cout << u[0]->total_stored_volume << " " << u3.total_stored_volume << endl; // 0 5
    cout << u[0]->getWaterSource().at(0)->name << " " << u3.getWaterSource().at(0)->name << endl;
}
 */

/*
void rofCalculationsTest() {

    cout << "BEGINNING ROF TEST" << endl << endl;

    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Utils::parse2DCsvFile("../TestFiles/"
                                                            "inflowsLong.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);

    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 400.0);
    Reservoir r2("R2", 1, 3.0, catchments2, ONLINE, 350.0);

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&r1);
    water_sources.push_back(&r2);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);

    vector<vector<int>> water_sources_adjacency_matrix = {
            {0,  1},
            {-1, 0},
    };

    vector<vector<int>> water_sources_utility_adjacency_matrix = {
            {1, 0},
            {0, 1}
    };

    ContinuityModelROF* crof = new ContinuityModelROF(Utils::copyWaterSourceVector(water_sources),
                                                      water_sources_adjacency_matrix,
                                                      Utils::copyUtilityVector(utilities),
                                                      water_sources_utility_adjacency_matrix,
                                                      SHORT_TERM_ROF, 0);
    vector<double> risks_of_failure;
    crof->setWater_sources_realization(water_sources);
    risks_of_failure = crof->calculateROF(0); // beginning of 60th year.
    cout << risks_of_failure[0] << " " << risks_of_failure[1]; // The output should be split in 50 blocks of 52 rows and 2 columns.
    // The numbers in the columns are the storage/capacity ratio for
    // each of the two utilities. The last two numbers are the risks_of_failure,
    // which must be 0.0365385 0.0665385.

    cout << endl << "END OF ROF TEST" << endl << "---------------------"
            "---------------" << endl << endl;
}*/

/*
void simulationTest() {
    cout << "BEGINNING ROF TEST" << endl << endl;

    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Utils::parse2DCsvFile("../TestFiles/"
                                                            "inflowsLong.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);

    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 500.0, 0);
    Reservoir r2("R2", 1, 3.0, catchments2, ONLINE, 575.0, 0);

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0, 0, vector<int>());
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0, 0, vector<int>());

    vector<WaterSource *> water_sources;
    water_sources.push_back(&r1);
    water_sources.push_back(&r2);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);


    Graph g((int) water_sources.size());
    g.addEdge(0, 1);

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0},
            {1}
    };

    vector<double> restriction_stage_multipliers = {0.7, 0.9};
    vector<double> restriction_stage_triggers = {1.1, 1.1};

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, vector<int>(1));
    Restrictions rp2(0, restriction_stage_triggers, restriction_stage_multipliers, vector<int>(2));

    vector<DroughtMitigationPolicy *> restrictions = {&rp1, &rp2};

    DataCollector *data_collector = nullptr;

    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, restrictions, 52, 1,
                 data_collector);
    cout << "Beginning simulation" << endl;
    s.runFullSimulation();
    cout << "Ending simulation" << endl;
}
*/
/*
void WaterSourceGraphTest() {

///*
// *    5   6   7
// *   / \ / \ /
// *  4   3   2
// *       \ /
// *        1
// *        |
// *        0
// *

// Create a WaterSourceGraph given in the above diagram
    Graph g(8);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(4, 5);
    g.addEdge(3, 5);
    g.addEdge(3, 6);
    g.addEdge(2, 6);
    g.addEdge(2, 7);

    cout << "Following is a Topological Sort of\n";
    g.getTopological_order();


//    vector<int> upstream = g.findUpstreamSources(6);

//    for (int & n : upstream) {
//        cout << n << " ";
//    }
    cout << "<-- should be 3 and 2" << endl;
}*/


void simulation3U5RTest() {
    cout << "BEGINNING ROF TEST" << endl << endl;

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
    Reservoir r1("R1", 0, 3.0, catchments1, 100.0, 20);
    Reservoir r2("R2", 1, 3.0, catchments2, 375.0, 20);
    Reservoir r3("R3", 2, 2.0, catchments3, 250.0, 20);
    Reservoir r4("R4", 3, 3.0, catchments2, 550.0, 20);
    Reservoir r5("R5", 4, 2.0, catchments3, 500.0, 20);

    vector<WaterSource *> water_sources;
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

    Graph g((int) water_sources.size());
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(4, 3);

    /// Create catchments and corresponding vector
    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.03, 1, vector<int>(), 0);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.07, 1, vector<int>(), 0);
    Utility u3("U3", 2, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.05, 1, vector<int>(), 0);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);
    utilities.push_back(&u3);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 2},
            {1, 4},
            {3}
    };

    /// Restriction policies
    vector<double> restriction_stage_multipliers1 = {0.9, 0.7};
    vector<double> restriction_stage_triggers1 = {0.01, 0.04};

    vector<double> restriction_stage_multipliers2 = {0.8, 0.6};
    vector<double> restriction_stage_triggers2 = {0.03, 0.05};

    Restrictions rp1(0, restriction_stage_multipliers1, restriction_stage_triggers1, vector<int>(1, 0));
    Restrictions rp2(1, restriction_stage_multipliers2, restriction_stage_triggers2, vector<int>(1, 1));
    Restrictions rp3(2, restriction_stage_multipliers1, restriction_stage_triggers1, vector<int>(1, 2));

    vector<DroughtMitigationPolicy *> drought_mitigation_policies = {&rp1, &rp2, &rp3};

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
    vector<double> buyers_transfers_capacities = {2, 1.5, 1.0};
    vector<double> buyers_transfers_trigger = {0.04, 0.04};

    Graph ug(3);
    ug.addEdge(0, 1);
    ug.addEdge(0, 2);
    ug.addEdge(1, 2);

    Transfers t(0, 0, 5, buyers_ids, buyers_transfers_capacities, buyers_transfers_trigger, ug,
                vector<double>(), vector<int>());
    drought_mitigation_policies.push_back(&t);

    /// Data collector pointer
    DataCollector *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, drought_mitigation_policies,
                 104, 2, data_collector);
    cout << "Beginning U3R5 simulation" << endl;
    s.runFullSimulation();
    cout << "Ending U3R5 simulation" << endl;
}

/*
void simulation1U1R1ITest() {
    cout << "BEGINNING INTAKE TEST" << endl << endl;

    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Utils::parse2DCsvFile("../TestFiles/"
                                                            "inflowsLong.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    vector<Catchment *> catchments3;
    catchments1.push_back(&c1);
    catchments2.push_back(&c2);
    catchments3.push_back(&c2);

    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 200.0, 0);
    Intake i1("I1", 1, 1.0, catchments2, OFFLINE, 0);
    Reservoir r2("R1", 2, 1.0, catchments1, ONLINE, 200.0, 0);

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0, 0, vector<int>());

    vector<WaterSource *> water_sources;
    water_sources.push_back(&r1);
    water_sources.push_back(&i1);
    water_sources.push_back(&r2);

    vector<Utility *> utilities;
    utilities.push_back(&u1);


    Graph g((int) water_sources.size());
    g.addEdge(0, 1);
    g.addEdge(1, 2);

    vector<vector<int>> sources_to_utility_connectivity_matrix = {
            {0, 1, 2}
    };

    vector<DroughtMitigationPolicy *> restrictions;// = {&rp1, &rp2, &rp3};

    DataCollector *data_collector = nullptr;

    Simulation s(water_sources, g, sources_to_utility_connectivity_matrix, utilities, restrictions, 104, 2,
                 data_collector);
    cout << "Beginning 1U1R1I simulation" << endl;
    s.runFullSimulation();
    cout << "Ending 1U1R1I simulation" << endl;
}
*/
/*
void test_QP() {
    Matrix<double> G, CE, CI;
    Vector<double> g0, ce0, ci0, x;
    unsigned int n, m, p;
    double sum = 0.0;
    char ch;

//    Given:
//    H =  4 -2   f^T = [6 0]
//        -2  4
//
//    Solve:
//    min f(allocations_aux) = 1/2 allocations_aux H allocations_aux + f allocations_aux
//    s.t.
//            x_1 + x_2 = 3
//    x_1 >= 0
//    x_2 >= 0
//    x_1 + x_2 >= 2
//
//    The solution is allocations_aux^T = [1 2] and f(allocations_aux) = 12

    n = 2;
    G.resize(n, n);
    {
        std::istringstream is("4, -2, -2, 4");

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                is >> G[i][j] >> ch;
    }
    Matrix<double> G2 = G;

    g0.resize(n);
    {
        std::istringstream is("6.0, 0.0 ");

        for (int i = 0; i < n; i++)
            is >> g0[i] >> ch;
    }

    m = 1;
    CE.resize(n, m);
    {
        std::istringstream is("1.0, 1.0 ");

        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                is >> CE[i][j] >> ch;
    }

    ce0.resize(m);
    {
        std::istringstream is("-3.0 ");

        for (int j = 0; j < m; j++)
            is >> ce0[j] >> ch;
    }

    p = 3;
    CI.resize(n, p);
    {
        std::istringstream is("1.0, 0.0, 1.0, 0.0, 1.0, 1.0 ");

        for (int i = 0; i < n; i++)
            for (int j = 0; j < p; j++)
                is >> CI[i][j] >> ch;
    }

    ci0.resize(p);
    {
        std::istringstream is("0.0, 0.0, -2.0 ");

        for (int j = 0; j < p; j++)
            is >> ci0[j] >> ch;
    }
    x.resize(n);


    // ===== Carolina test

    unsigned int nvar = 7;
    G.resize(0, nvar, nvar);
    G[0][0] = 1e-6;
    G[1][1] = 1e-6;
    G[2][2] = 1e-6;
    G[3][3] = 1e-6;
    G[4][4] = 2;
    G[5][5] = 2;
    G[6][6] = 2;

    g0.resize(0, nvar);
    g0[4] = -1;
    g0[5] = -9;//0;
    g0[6] = -2;

    CE.resize(0, nvar, 4);
    CE[2][1] = -1;
    CE[1][1] = 1;
    CE[4][1] = -1;
    CE[0][2] = 1;
    CE[2][2] = 1;
    CE[3][2] = -1;
    CE[5][2] = -1;
    CE[3][3] = 1;
    CE[6][3] = -1;
    CE[0][0] = -1;
    CE[1][0] = -1;

    ce0.resize(0, 4);
    ce0[0] = -5;

    CI.resize(0, nvar, nvar * 2);
    CI[0][0] = 1;
    CI[1][1] = 1;
    CI[2][2] = 1;
    CI[3][3] = 1;
    CI[4][4] = 1;
    CI[5][5] = 1;
    CI[6][6] = 1;
    CI[0][7] = -1;
    CI[1][8] = -1;
    CI[2][9] = -1;
    CI[3][10] = -1;
    CI[4][11] = -1;
    CI[5][12] = -1;
    CI[6][13] = -1;

    ci0.resize(0, nvar * 2);
    ci0[0] = 3;
    ci0[1] = 3;
    ci0[2] = 1;
    ci0[3] = 1;
    ci0[4] = -0.5;
    ci0[5] = -0.5;//0;
    ci0[6] = -0.5;
    ci0[7] = 3;
    ci0[8] = 3;
    ci0[9] = 1;
    ci0[10] = 1;
    ci0[11] = 5;
    ci0[12] = 5;//0;
    ci0[13] = 5;


//    print_matrix("H", G);
//    print_vector("f", g0);
//    print_matrix("Aeq", CE);
//    print_vector("beq", ce0);
//    print_matrix("A", CI);
//    print_vector("b", ci0);

    cout << "calling solver" << endl;
    solve_quadprog(G, g0, CE, ce0, CI, ci0, x);

//    print_vector("allocations_aux", x);

//    Aeq.resize(0, 1, 2);
//    Aeq[0][0] = 1;
//    Aeq[0][1] = 1;
//
//    beq[0] = 3;
//
//    A.resize(0, 1, 2);
//    A[0][0] = 1;
//    A[0][1] = 1;
//
//    b.resize(0, 1);
//    b[0] = 2;
//
//    Vector<double> lb;
//    Vector<double> ub;
//
//    lb.resize(0, 2);
//    ub.resize(0, 2);
//    ub[0] = -1e6;
//    ub[1] = -1e6;
//
//    allocations_aux.resize(0, 2);

//    print_matrix("H", G2);
//    print_vector("f", f);
//    print_matrix("Aeq", Aeq);
//    print_vector("beq", beq);
//    print_matrix("A", A);
//    print_vector("b", b);
//    cout << endl;
//    solve_quadprog_matlab_syntax(G2, f, Aeq, beq, A, b, lb, ub, allocations_aux);
//    print_vector("allocations_aux", allocations_aux);
    // supposed to be [1, 2]



}
*/
/*
void test_transfers() {

//    WaterSourceGraph gu(4, 0);
//    gu.addEdge(3, 0);
//    gu.addEdge(3, 1);
//    gu.addEdge(0, 1);
//    gu.addEdge(1, 2);
//
//    vector<vector<double>> cm = gu.getadjacencyMatrix();
//    for (auto &r : cm) {
//        for (double &c : r)
//            cout << c << " ";
//        cout << endl;
//    }


//    *
//     *      0
//     *     / \
//     *  0 v   v 1
//     *   /     \
//     *  1--->---2
//     *   \  2
//     *  3 v
//     *     \
//     *      3
//     *
//     *

    vector<int> buyers_ids = {1, 2, 3};
    vector<double> buyers_transfers_capacities = {3, 3, 1, 1};
    vector<double> buyers_transfers_trigger = {0.05, 0.08, 0.02};
//    vector<vector<double>> continuity_matrix = {
//            {-1, -1, 0, 0, 1, 0, 0, 0},
//            {0, 1, -1, 0, 0, -1, 0, 0},
//            {1, 0, 1, -1, 0, 0, -1, 0},
//            {0, 0, 0, 1, 0, 0, 0, -1}
//    };

    Graph ug(4);
    ug.addEdge(0, 1);
    ug.addEdge(0, 2);
    ug.addEdge(1, 2);
    ug.addEdge(1, 3);

    Transfers t(0, 3, 0, buyers_ids, buyers_transfers_capacities, buyers_transfers_trigger, ug,
                vector<double>(), vector<int>());
    vector<double> allocations = t.solve_QP(*(new vector<double>{0.5, 4.5, 1}), 10, 0.5);

    for (double & a : allocations) cout << a << " ";
    cout << endl;
    // answer: 3 2 1 0.5 1 3.5 0.5
}*/

/*
void test_getcontinuityMatrix() {
    Graph g(4);
    g.addEdge(0, 2);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);

//    vector<vector<double>> expected_continuity_matrix = {
//            {-1, -1, 0, 0, 1, 0, 0, 0},
//            {0, 1, -1, 0, 0, -1, 0, 0},
//            {1, 0, 1, -1, 0, 0, -1, 0},
//            {0, 0, 0, 1, 0, 0, 0, -1}
//    };

    vector<vector<double>> continuity_matrix = g.getContinuityMatrix();

    for (auto r : continuity_matrix) {
        for (double v : r)
            cout << v << " ";
        cout << endl;
    }
}
*/

void simulation3U5RInfraTest() {
    srand((unsigned int) time(NULL));

    cout << "BEGINNING ROF TEST" << endl << endl;

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

    vector<WaterSource *> water_sources;
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

    Graph g((int) water_sources.size());
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(4, 3);

    /// Create catchments and corresponding vector
    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.03, 1);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.07, 1, vector<int>(1, 1), 0.05);
    Utility u3("U3", 2, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.05, 1);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);
    utilities.push_back(&u3);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 2},
            {1, 4},
            {3}
    };

    /// Restriction policies
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;

    vector<double> restriction_stage_multipliers1 = {0.9, 0.7};
    vector<double> restriction_stage_triggers1 = {0.01, 0.04};

    vector<double> restriction_stage_multipliers2 = {0.8, 0.6};
    vector<double> restriction_stage_triggers2 = {0.03, 0.05};

    Restrictions rp1(0, restriction_stage_multipliers1, restriction_stage_triggers1, vector<int>(1, 0));
    Restrictions rp2(1, restriction_stage_multipliers2, restriction_stage_triggers2, vector<int>(1, 1));
    Restrictions rp3(2, restriction_stage_multipliers1, restriction_stage_triggers1, vector<int>(1, 2));

    drought_mitigation_policies = {&rp1, &rp2, &rp3};

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

    Graph ug(3);
    ug.addEdge(0, 1);
    ug.addEdge(0, 2);
    ug.addEdge(1, 2);

    Transfers t(0, 0, 1, buyers_ids, buyers_transfers_capacities, buyers_transfers_trigger, ug,
                vector<double>(), vector<int>());
    drought_mitigation_policies.push_back(&t);

    /// Data collector pointer
    DataCollector *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, drought_mitigation_policies,
                 992, 1 , data_collector);
    cout << "Beginning U3R5 simulation" << endl;
    s.runFullSimulation();
    cout << "Ending U3R5 simulation" << endl;
}

int main() {
//
// Uncomment the lines below to run the tests.
//
//    ::reservoirAndCatchmentTest();
//    ::testReadCsv();
//    ::regionTwoUtilitiesTwoReservoirsContinuityTest();
//    ::regionOneUtilitiesTwoReservoirsContinuityTest();
//    ::catchmentCopy(); // Create a setStreamflow method in order to run this test.
//    ::reservoirCopy(); // Make vector catchments public in order to run this test.
//    ::utilityCopy();
//    ::rofCalculationsTest();
//    ::simulationTest();
//    ::graphTest();
//    ::simulation3U5RTest();
//    ::simulation1U1R1ITest();
//    ::testLPSolver();
//    ::test_QP();
//    ::test_transfers();
//    ::test_getcontinuityMatrix();
//    ::simulation3U5RInfraTest();

    return 0;
}