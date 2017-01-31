#include <iostream>
#include "SystemComponents/WaterSources/Reservoir.h"
#include "SystemComponents/Utility.h"
#include "Utils/Aux.h"
#include "Simulation.h"


int regionOneUtilitiesTwoReservoirsContinuityTest();

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
//    cout << r.getOutflow_previous_week() << " expected: " << 1.0 << endl;
//    cout << r.getAvailable_volume() << " expected: " << 10 << endl;
//
//    cout << endl << "END OF RESERVOIR TEST" << endl << "------------------------------------" << endl << endl;
//
//    return 1;
//}

int testReadCsv() {

    cout << "BEGINNING CSV READ TEST" << endl << endl;

    // 2D
    cout << "2-D " << endl;
    char const *file_name_2d = "/home/bernardo/ClionProjects/TriangleModel/TestFiles/inflowsLakeTest.csv";
    double **test_data_2d = Aux::parse2DCsvFile(file_name_2d, 2, 5);

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
    double *test_data_1d = Aux::parse1DCsvFile(file_name_1d, 52);

    for (int i = 0; i < 5; i++) {
        cout << test_data_1d[i] << endl;
    }

    cout << endl << "END OF CSV READ TEST" << endl << "------------------------------------" << endl << endl;

    return 0;
}

/**
 * This test checks is reservoirs and utilities are properly linked and sending and receiving water
 * to and from the right places.
 * @return
 */
int regionTwoUtilitiesTwoReservoirsContinuityTest() {

    cout << "BEGINNING 2 RESERVOIRS 2 UTILITIES READ TEST" << endl << endl;

    int streamflow_n_weeks = 52;
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

    vector<vector<int>> reservoir_connectivity_matrix = {
            {0,  1},
            {-1, 0},
    };

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {1, 0},
            {0, 1}
    };

    Simulation s(reservoirs, reservoir_connectivity_matrix, utilities, reservoir_utility_connectivity_matrix, 6, 1);
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
    int streamflow_n_weeks = 52;
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

    vector<vector<int>> reservoir_connectivity_matrix = {
            {0,  1},
            {-1, 0},
    };

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {1, 1},
    };

    Simulation s(reservoirs, reservoir_connectivity_matrix, utilities, reservoir_utility_connectivity_matrix, 6, 1);
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
//    u3.getWaterSource().at(0)->source_name[0] = 'a'; // remove const from source_name declaration in WaterSource.h
//
//    cout << u[0]->demand_series[0] << " " << u3.demand_series[0] << endl; // 2 30
//    cout << u[0]->total_stored_volume << " " << u3.total_stored_volume << endl; // 0 5
//    cout << u[0]->getWaterSource().at(0)->source_name << " " << u3.getWaterSource().at(0)->source_name << endl;
//}

void rofCalculationsTest() {

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

    ContinuityModelROF* crof = new ContinuityModelROF(Aux::copyWaterSourceVector(water_sources),
                                                      water_sources_adjacency_matrix,
                                                      Aux::copyUtilityVector(utilities),
                                                      water_sources_utility_adjacency_matrix,
                                                      SHORT_TERM_ROF, 0);
    double *rofs;
    crof->setWater_sources_realization(water_sources);
    rofs = crof->calculateROF(0); // beginning of 60th year.
    cout << rofs[0] << " " << rofs[1]; // The output should be split in 50 blocks of 52 rows and 2 columns.
    // The numbers in the columns are the storage/capacity ratio for
    // each of the two utilities. The last two numbers are the rofs,
    // which must be 0.0365385 0.0665385.

    cout << endl << "END OF ROF TEST" << endl << "---------------------"
            "---------------" << endl << endl;
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

    Simulation s(Aux::copyWaterSourceVector(water_sources),
                 water_sources_adjacency_matrix,
                 Aux::copyUtilityVector(utilities),
                 water_sources_utility_adjacency_matrix,
                 2, 1);

    s.runFullSimulation();
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
    ::simulationTest();


    return 0;
}