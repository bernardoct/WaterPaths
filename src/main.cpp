#include <iostream>
#include "SystemComponents/WaterSources/Reservoir.h"
#include "SystemComponents/Utility.h"
#include "Utils/Aux.h"
#include "Simulation/Simulation.h"


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
//    r.updateAvailableVolume(0, 2.5, 2.0);
//    cout << r.getTotal_inflow() << " expected: " << 1.0 << endl;
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

    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0);
    Reservoir r2("R2", 1, 0.5, catchments2, ONLINE, 10.0);

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks);
    Utility u2("U2", 1, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks);

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

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, 1);
    Restrictions rp2(0, restriction_stage_triggers, restriction_stage_multipliers, 2);

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


    Reservoir r1("R1", 0, 1.0, catchments1, ONLINE, 15.0);
    Reservoir r2("R2", 1, 0.5, catchments2, ONLINE, 10.0);

    Utility u1("U1", 0, "/home/bernardo/ClionProjects/TriangleModel/TestFiles/demands.csv", streamflow_n_weeks);

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

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, 1);

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
//    r1.updateAvailableVolume(1, 2.0, 3.0);
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

    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 500.0);
    Reservoir r2("R2", 1, 3.0, catchments2, ONLINE, 575.0);

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks);

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

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, 1);
    Restrictions rp2(0, restriction_stage_triggers, restriction_stage_multipliers, 2);

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

    int streamflow_n_weeks = 52 * 70;
    double **streamflows_test = Aux::parse2DCsvFile("../TestFiles/"
                                                            "inflowsLong.csv", 2, streamflow_n_weeks);

    Catchment c1(streamflows_test[0], streamflow_n_weeks);
    Catchment c2(streamflows_test[1], streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    vector<Catchment *> catchments3;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);
    catchments3.push_back(&c1);

    Reservoir r1("R1", 0, 3.0, catchments1, ONLINE, 200.0);
    Reservoir r2("R2", 1, 3.0, catchments2, ONLINE, 275.0);
    Reservoir r3("R3", 2, 2.0, catchments3, ONLINE, 400.0);
    Reservoir r4("R4", 3, 3.0, catchments2, ONLINE, 400.0);
    Reservoir r5("R5", 4, 2.0, catchments3, ONLINE, 300.0);

    Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks);
    Utility u2("U2", 1, "../TestFiles/demandsLong.csv", streamflow_n_weeks);
    Utility u3("U3", 2, "../TestFiles/demandsLong.csv", streamflow_n_weeks);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&r1);
    water_sources.push_back(&r2);
    water_sources.push_back(&r3);
    water_sources.push_back(&r4);
    water_sources.push_back(&r5);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);
    utilities.push_back(&u3);


    /*
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

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 2},
            {1, 4},
            {3}
    };

    vector<double> restriction_stage_multipliers1 = {0.7, 0.9};
    vector<double> restriction_stage_triggers1 = {0.02, 0.04};

    vector<double> restriction_stage_multipliers2 = {0.6, 0.8};
    vector<double> restriction_stage_triggers2 = {0.03, 0.05};

    Restrictions rp1(1, restriction_stage_multipliers1, restriction_stage_triggers1, 1);
    Restrictions rp2(2, restriction_stage_multipliers2, restriction_stage_triggers2, 2);
    Restrictions rp3(3, restriction_stage_multipliers1, restriction_stage_triggers1, 3);

    vector<DroughtMitigationPolicy *> restrictions = {&rp1, &rp2, &rp3};

    DataCollector *data_collector = nullptr;

    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, restrictions, 104, 2,
                 data_collector);
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
//    ::regionTwoUtilitiesTwoReservoirsContinuityTest(); // REVISE SPREADSHEET FOR THESE TWO TESTS.
//    ::regionOneUtilitiesTwoReservoirsContinuityTest();
//    ::catchmentCopy(); // Create a setStreamflow method in order to run this test.
//    ::reservoirCopy(); // Make vector catchments public in order to run this test.
//    ::utilityCopy();
//    ::rofCalculationsTest();
//    ::simulationTest();
//    ::graphTest();
    ::simulation3U5RTest();

    return 0;
}