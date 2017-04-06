#include <iostream>
#include <sstream>
#include "SystemComponents/WaterSources/Reservoir.h"
#include "SystemComponents/Utility.h"
#include "Utils/Aux.h"
#include "Simulation/Simulation.h"
#include "Utils/QPSolver/QuadProg++.hh"
#include "DroughtMitigationInstruments/Transfers.h"


int regionOneUtilitiesTwoReservoirsContinuityTest();

void simulation3U5RTest();

using namespace std;
using namespace Constants;

//int reservoirAndCatchmentTest() {
//    cout << "BEGINNING RESERVOIR TEST" << endl << endl;
//
//    double **streamflows_test = Aux::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
//                                                            "inflowsLakeTest.csv", 2, 52);
//    Catchment catchment_test(streamflows_test[0]);
//
//    Reservoir r(string("Lake Michie"), 0, 0.5, catchment_test, ONLINE, 10.0);
//
//    r.continuityWaterSource(0, 2.5, 2.0);
//    cout << r.getTotal_outflow() << " expected: " << 1.0 << endl;
//    cout << r.getAvailable_volume() << " expected: " << 10 << endl;
//
//    cout << endl << "END OF RESERVOIR TEST" << endl << "------------------------------------" << endl << endl;
//
//    return 1;
//}


//int testReadCsv() {
//
//    cout << "BEGINNING CSV READ TEST" << endl << endl;
//
//    // 2D
//    cout << "2-D " << endl;
//    char const *file_name_2d = "/home/bernardo/ClionProjects/TriangleModel/TestFiles/inflowsLakeTest.csv";
//    double **test_data_2d = Aux::parse2DCsvFile(file_name_2d, 2, 5);
//
//    for (int i = 0; i < 2; i++) {
//        for (int j = 0; j < 4; j++) {
//            cout << test_data_2d[i][j] << " ";
//        }
//        cout << endl;
//    }
//    cout << endl;
//
//    // 1D
//    cout << "1-D " << endl;
//    char const *file_name_1d = "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv";
//    double *test_data_1d = Aux::parse1DCsvFile(file_name_1d, 52);
//
//    for (int i = 0; i < 5; i++) {
//        cout << test_data_1d[i] << endl;
//    }
//
//    cout << endl << "END OF CSV READ TEST" << endl << "------------------------------------" << endl << endl;
//
//    return 0;
//}

/**
 * This test checks is reservoirs and utilities are properly linked and sending and receiving water
 * to and from the right places.
 * @return
 */
int regionTwoUtilitiesTwoReservoirsContinuityTest() {

    cout << "BEGINNING 2 RESERVOIRS 2 UTILITIES READ TEST" << endl << endl;

    int streamflow_n_weeks = 2652;
    double **streamflows_test = Aux::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
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

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks, 0, 0);
    Utility u2("U2", 1, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks, 0, 0);

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

/**
 * This test is to check if the demand of a utility is properly split between reservoirs.
 * Another test will have to be created for when other water sources (i.g. intakes) are added.
 * @return
 */
int regionOneUtilitiesTwoReservoirsContinuityTest() {

    cout << "BEGINNING 2 RESERVOIRS 1 UTILITY READ TEST" << endl << endl;
    int streamflow_n_weeks = 2652;
    double **streamflows_test = Aux::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
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

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks, 0, 0);

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


//void catchmentCopy() {
//    cout << "BEGINNING CATCHMENT COPY READ TEST" << endl << endl;
//    int streamflow_n_weeks = 52;
//    double **streamflows_test = Aux::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
//                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);
//
//    Catchment c1(streamflows_test[0], streamflow_n_weeks);
//
//    Catchment c2(c1);
//
//    c2.setStreamflow(1, 70);
//
//    for (int i = 0; i < 5; ++i) {
//        cout << c1.getStreamflow(i) << endl;
//    }
//
//    cout << " " << endl;
//
//    for (int i = 0; i < 5; ++i) {
//        cout << c2.getStreamflow(i) << endl;
//    }
//
//    Catchment * c3 = new Catchment(c1);
//    cout << " " << endl;
//
//    c3->setStreamflow(1, 20);
//
//    for (int i = 0; i < 5; ++i) {
//        cout << c2.getStreamflow(i) << endl;
//    }
//
//    cout << " " << endl;
//
//    for (int i = 0; i < 5; ++i) {
//        cout << c3->getStreamflow(i) << endl;
//    }
//
//}
//
//void reservoirCopy() {
//    cout << "BEGINNING RESERVOIR COPY TEST" << endl << endl;
//    int streamflow_n_weeks = 52;
//    double **streamflows_test = Aux::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
//                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);
//
//    Catchment c1(streamflows_test[0], streamflow_n_weeks);
//    vector<Catchment *> catchments1;
//    catchments1.push_back(&c1);
//
//    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0);
////    Reservoir r2(r1);
//    Reservoir r2 = r1;
//
//    r2.catchments[0]->streamflows[0] = 50;
//
//    cout << r1.catchments[0]->getStreamflow(0) << endl; // should be 0.5
//    cout << r2.catchments[0]->getStreamflow(0) << endl; // should be 50
//
//    r1.continuityWaterSource(1, 2.0, 3.0);
//
//    r1.toString(); // Should show stored volume of 14.5.
//    r2.toString(); // Should show stored volume of 15.0.
//
//
//    cout << "END OF RESERVOIR COPY TEST" << endl << endl;
//}
//
//void utilityCopy() {
//    cout << "BEGINNING 2 RESERVOIRS 1 UTILITY READ TEST" << endl << endl;
//    int streamflow_n_weeks = 52;
//    double **streamflows_test = Aux::parse2DCsvFile("/home/bernardo/ClionProjects/TriangleModel/TestFiles/"
//                                                            "inflowsLakeTest.csv", 2, streamflow_n_weeks);
//
//    Catchment c1(streamflows_test[0], streamflow_n_weeks);
//    Catchment c2(streamflows_test[1], streamflow_n_weeks);
//
//    vector<Catchment *> catchments1;
//    vector<Catchment *> catchments2;
//    catchments1.push_back(&c1);
//    catchments1.push_back(&c2);
//    catchments2.push_back(&c2);
//
//
//    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0);
//    Reservoir r2("R2", 1, 0.5, catchments2, ONLINE, 10.0);
//
//    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks);
//
//    vector<WaterSource *> reservoirs;
//    reservoirs.push_back(&r1);
//    reservoirs.push_back(&r2);
//
//    vector<Utility *> utilities;
//    utilities.push_back(&u1);
//
//    vector<vector<int>> reservoir_connectivity_matrix = {
//            {0,  1},
//            {-1, 0},
//    };
//
//    vector<vector<int>> reservoir_utility_connectivity_matrix = {
//            {1, 1},
//    };
//
//    ContinuityModelRealization cmr(reservoirs, reservoir_connectivity_matrix,
//                               utilities, reservoir_utility_connectivity_matrix, 0);
//
//    vector<WaterSource* > ws = cmr.getWaterSources();
//    vector<Utility* > u = cmr.getUtilities();
//
////    Utility u3(*u[0]);
//    Utility u3 = u1;
//    u3.demand_series[0] = 30;
//    u3.total_stored_volume = 5;
//    u3.getWaterSource().at(0)->name[0] = 'a'; // remove const from name declaration in WaterSource.h
//
//    cout << u[0]->demand_series[0] << " " << u3.demand_series[0] << endl; // 2 30
//    cout << u[0]->total_stored_volume << " " << u3.total_stored_volume << endl; // 0 5
//    cout << u[0]->getWaterSource().at(0)->name << " " << u3.getWaterSource().at(0)->name << endl;
//}

//void rofCalculationsTest() {
//
//    cout << "BEGINNING ROF TEST" << endl << endl;
//
//    int streamflow_n_weeks = 52 * 70;
//    double **streamflows_test = Aux::parse2DCsvFile("../TestFiles/"
//                                                            "inflowsLong.csv", 2, streamflow_n_weeks);
//
//    Catchment c1(streamflows_test[0], streamflow_n_weeks);
//    Catchment c2(streamflows_test[1], streamflow_n_weeks);
//
//    vector<Catchment *> catchments1;
//    vector<Catchment *> catchments2;
//    catchments1.push_back(&c1);
//    catchments1.push_back(&c2);
//    catchments2.push_back(&c2);
//
//    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 400.0);
//    Reservoir r2("R2", 1, 3.0, catchments2, ONLINE, 350.0);
//
//    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks);
//    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks);
//
//    vector<WaterSource *> water_sources;
//    water_sources.push_back(&r1);
//    water_sources.push_back(&r2);
//
//    vector<Utility *> utilities;
//    utilities.push_back(&u1);
//    utilities.push_back(&u2);
//
//    vector<vector<int>> water_sources_adjacency_matrix = {
//            {0,  1},
//            {-1, 0},
//    };
//
//    vector<vector<int>> water_sources_utility_adjacency_matrix = {
//            {1, 0},
//            {0, 1}
//    };
//
//    ContinuityModelROF* crof = new ContinuityModelROF(Aux::copyWaterSourceVector(water_sources),
//                                                      water_sources_adjacency_matrix,
//                                                      Aux::copyUtilityVector(utilities),
//                                                      water_sources_utility_adjacency_matrix,
//                                                      SHORT_TERM_ROF, 0);
//    vector<double> risks_of_failure;
//    crof->setWater_sources_realization(water_sources);
//    risks_of_failure = crof->calculateROF(0); // beginning of 60th year.
//    cout << risks_of_failure[0] << " " << risks_of_failure[1]; // The output should be split in 50 blocks of 52 rows and 2 columns.
//    // The numbers in the columns are the storage/capacity ratio for
//    // each of the two utilities. The last two numbers are the risks_of_failure,
//    // which must be 0.0365385 0.0665385.
//
//    cout << endl << "END OF ROF TEST" << endl << "---------------------"
//            "---------------" << endl << endl;
//}

void testDemandSplit() {

}

void simulationTest() {
    cout << "BEGINNING ROF TEST" << endl << endl;

    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Aux::parse2DCsvFile("../TestFiles/"
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

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0, 0);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0, 0);

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

//void graphTest() {
//
///*
// *    5   6   7
// *   / \ / \ /
// *  4   3   2
// *       \ /
// *        1
// *        |
// *        0
// */
//
//// Create a graph given in the above diagram
//    Graph g(8);
//    g.addEdge(0, 1);
//    g.addEdge(1, 2);
//    g.addEdge(1, 3);
//    g.addEdge(4, 5);
//    g.addEdge(3, 5);
//    g.addEdge(3, 6);
//    g.addEdge(2, 6);
//    g.addEdge(2, 7);
//
//    cout << "Following is a Topological Sort of\n";
//    g.topologicalSort();
//
//
//    vector<int> upstream = g.getUpstreamSources(6);
//
//    for (int & n : upstream) {
//        cout << n << " ";
//    }
//    cout << "<-- should be 3 and 2" << endl;
//}

void simulation3U5RTest() {
    cout << "BEGINNING ROF TEST" << endl << endl;

    /// Read streamflows
    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Aux::parse2DCsvFile("../TestFiles/"
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
    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 100.0, 20);
    Reservoir r2("R2", 1, 3.0, catchments2, ONLINE, 375.0, 20);
    Reservoir r3("R3", 2, 2.0, catchments3, ONLINE, 400.0, 20);
    Reservoir r4("R4", 3, 3.0, catchments2, ONLINE, 550.0, 20);
    Reservoir r5("R5", 4, 2.0, catchments3, ONLINE, 500.0, 20);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&r1);
    water_sources.push_back(&r2);
    water_sources.push_back(&r3);
    water_sources.push_back(&r4);
    water_sources.push_back(&r5);

    /// Create catchments and corresponding vector
    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.03, 1);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.07, 1);
    Utility u3("U3", 2, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0.05, 1);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);
    utilities.push_back(&u3);


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

    /// Reservoir-utility connectivity matrix (i.e. each row represents reservoirs owned by one utility
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 2},
            {1, 4},
            {3}
    };

    /// Restriction policies
    vector<double> restriction_stage_multipliers1 = {0.9, 0.7};
    vector<double> restriction_stage_triggers1 = {0.02, 0.04};

    vector<double> restriction_stage_multipliers2 = {0.8, 0.6};
    vector<double> restriction_stage_triggers2 = {0.03, 0.05};

    Restrictions rp1(0, restriction_stage_multipliers1, restriction_stage_triggers1, vector<int>(1, 0));
    Restrictions rp2(1, restriction_stage_multipliers2, restriction_stage_triggers2, vector<int>(1, 1));
    Restrictions rp3(2, restriction_stage_multipliers1, restriction_stage_triggers1, vector<int>(1, 2));

    vector<DroughtMitigationPolicy *> restrictions = {&rp1, &rp2, &rp3};

    /// Data collector pointer
    DataCollector *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, restrictions, 104, 2,
                 data_collector);
    cout << "Beginning U3R5 simulation" << endl;
    s.runFullSimulation();
    cout << "Ending U3R5 simulation" << endl;
}

void simulation1U1R1ITest() {
    cout << "BEGINNING INTAKE TEST" << endl << endl;

    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Aux::parse2DCsvFile("../TestFiles/"
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

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, 0, 0);

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

void test_QP() {
    Matrix<double> G, CE, CI;
    Vector<double> g0, ce0, ci0, x;
    unsigned int n, m, p;
    double sum = 0.0;
    char ch;

//    Given:
//    G =  4 -2   g0^T = [6 0]
//        -2  4
//
//    Solve:
//    min f(x) = 1/2 x G x + g0 x
//    s.t.
//            x_1 + x_2 = 3
//    x_1 >= 0
//    x_2 >= 0
//    x_1 + x_2 >= 2
//
//    The solution is x^T = [1 2] and f(x) = 12

    n = 2;
    G.resize(n, n);
    {
        std::istringstream is("4, -2, -2, 4");

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                is >> G[i][j] >> ch;
    }

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

//    cout << "CE" << endl;
//    for (int i = 0; i < CE.nrows(); ++i) {
//        for (int j = 0; j < CE.ncols(); ++j)
//            cout << CE[i][j] << " ";
//        cout << endl;
//    }


    // ===== Carolina test

//    unsigned int nvar = 7;
//    G.resize(0, nvar, nvar);
//
//    G[0][0] = 1e-6;
//    G[1][1] = 1e-6;
//    G[2][2] = 1e-6;
//    G[3][3] = 1e-6;
//    G[4][4] = 2;
//    G[5][5] = 2;
//    G[6][6] = 2;
//
//    cout << "G" << endl;
//    for (int i = 0; i < G.nrows(); ++i) {
//        for (int j = 0; j < G.ncols(); ++j)
//            cout << G[i][j] << " ";
//        cout << endl;
//    }
//
//
//    cout << "g0" << endl;
//    g0.resize(0, nvar);
//    g0[4] = -1;
//    g0[5] = -9;//0;
//    g0[6] = -2;
//
//    for (int i = 0; i < nvar; ++i)
//        cout << g0[i] << " ";
//    cout << endl;
//
//    CE.resize(0, nvar, 4);
//    CE[2][0] = -1;
//    CE[1][0] = 1;
//    CE[4][0] = -1;
//    CE[0][1] = 1;
//    CE[2][1] = 1;
//    CE[3][1] = -1;
//    CE[5][1] = -1;
//    CE[3][2] = 1;
//    CE[6][2] = -1;
//    CE[0][3] = 1;
//    CE[1][3] = 1;
//
//    cout << "CE" << endl;
//    for (int i = 0; i < CE.nrows(); ++i) {
//        for (int j = 0; j < CE.ncols(); ++j)
//            cout << CE[i][j] << " ";
//        cout << endl;
//    }
//
//    ce0.resize(0, 4);
//    ce0[3] = -5;
//
//    cout << "ce0" << endl;
//    for (int i = 0; i < 4; ++i)
//        cout << ce0[i] << " ";
//    cout << endl;
//
//    CI.resize(0, nvar, nvar * 2);
//    CI[0][0] = 1;
//    CI[1][1] = 1;
//    CI[2][2] = 1;
//    CI[3][3] = 1;
//    CI[4][4] = 1;
//    CI[5][5] = 1;
//    CI[6][6] = 1;
//    CI[0][7] = -1;
//    CI[1][8] = -1;
//    CI[2][9] = -1;
//    CI[3][10] = -1;
//    CI[4][11] = -1;
//    CI[5][12] = -1;
//    CI[6][13] = -1;
//
//    cout << "CI" << endl;
//    for (int i = 0; i < CI.nrows(); ++i) {
//        for (int j = 0; j < CI.ncols(); ++j)
//            cout << CI[i][j] << " ";
//        cout << endl;
//    }
//
//    ci0.resize(0, nvar * 2);
//    ci0[0] = 3;
//    ci0[1] = 3;
//    ci0[2] = 1;
//    ci0[3] = 1;
//    ci0[4] = -0.5;
//    ci0[5] = -0.5;//0;
//    ci0[6] = -0.5;
//    ci0[7] = 3;
//    ci0[8] = 3;
//    ci0[9] = 1;
//    ci0[10] = 1;
//    ci0[11] = 5;
//    ci0[12] = 5;//0;
//    ci0[13] = 5;
//
//    cout << "ci0" << endl;
//    for (int i = 0; i < nvar * 2; ++i)
//        cout << ci0[i] << " ";
//    cout << endl;


    cout << "calling solver" << endl;
    std::cout << "f: " << solve_quadprog(G, g0, CE, ce0, CI, ci0, x) << std::endl;
    std::cout << "x: " << x << std::endl;
//    for (int i = 0; i < n; i++)
//    std::cout << x[i] << ' ';
//	  std::cout << std::endl;

    /* FOR DOUBLE CHECKING COST since in the solve_quadprog routine the matrix G is modified */

    {
        std::istringstream is("4, -2, -2, 4 ");

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                is >> G[i][j] >> ch;
    }

    std::cout << "Double checking cost: ";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            sum += x[i] * G[i][j] * x[j];
    sum *= 0.5;

    for (int i = 0; i < n; i++)
        sum += g0[i] * x[i];
    std::cout << sum << std::endl;

    // answer should be x1 = 1 and x2 = 2


}

void test_transfers() {

    vector<int> buyers_ids = {1, 2, 3};
    vector<double> buyers_transfers_capaities = {3, 3, 1, 1};
    vector<double> buyers_transfers_trigger = {0.05, 0.08, 0.02};
    vector<vector<double>> continuity_matrix = {
            {0, 1, 0, 1},
            {1, 0, 0, 1},
            {-1, 1, 0, 0},
            {0, -1, 1, 0},
            {-1, 0, 0, 0},
            {0, -1, 0, 0},
            {0, 0, -1, 0}
    };

    Transfers t(0, 0, 0, buyers_ids, buyers_transfers_capaities, buyers_transfers_trigger, continuity_matrix);
    vector<double> allocations = t.solve_QP(*(new vector<double>{0.5, 4.5, 1}), 5, 0.5);

    for (double & a : allocations) cout << a << " ";
    // answer: 3 2 1 0.5 1 3.5 0.5

}

int main() {
//
// Uncomment the lines below to run the tests.
//
//    ::reservoirAndCatchmentTest();
//    ::testReadCsv();
//    ::regionTwoUtilitiesTwoReservoirsContinuityTest(); // REVISE SPREADSHEET FOR THESE TWO TESTS.
//    ::regionOneUtilitiesTwoReservoirsContinuityTest();
//    ::catchmentCopy(); // Create a setStreamflow method in order to run this test.
//    ::reservoirCopy(); // Make vector catchments public in order to run this test.
//    ::utilityCopy();
//    ::rofCalculationsTest();
//    ::simulationTest();
//    ::graphTest();
    ::simulation3U5RTest();
//    ::simulation1U1R1ITest();
//    ::testLPSolver();
//    ::test_QP();
//    ::test_transfers();

    return 0;
}