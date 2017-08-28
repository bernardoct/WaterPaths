#include <iostream>
#include <algorithm>
#include "SystemComponents/WaterSources/Reservoir.h"
#include "SystemComponents/Utility.h"
#include "Utils/Utils.h"
#include "Simulation/Simulation.h"
#include "Utils/QPSolver/QuadProg++.h"
#include "DroughtMitigationInstruments/Transfers.h"
#include "SystemComponents/WaterSources/ReservoirExpansion.h"
#include "SystemComponents/WaterSources/Quarry.h"
#include "SystemComponents/WaterSources/WaterReuse.h"
#include "Controls/SeasonalMinEnvFlowControl.h"
#include "Controls/StorageMinEnvFlowControl.h"
#include "Controls/InflowMinEnvFlowControl.h"
#include "Controls/FixedMinEnvFlowControl.h"
#include "Controls/Custom/JordanLakeMinEnvFlowControl.h"
#include "Controls/Custom/FallsLakeMinEnvFlowControl.h"
#include "SystemComponents/WaterSources/AllocatedReservoir.h"
#include "SystemComponents/WaterSources/SequentialJointTreatmentExpansion.h"
#include "SystemComponents/WaterSources/Relocation.h"
#include "DroughtMitigationInstruments/InsuranceStorageToROF.h"


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

    vector<Utility *> continuity_utilities;
    continuity_utilities.push_back(&u1);
    continuity_utilities.push_back(&u2);

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

    DataCollectorDeprecated *data_collector = nullptr;

    Simulation s(reservoirs, g, reservoir_utility_connectivity_matrix, continuity_utilities, restrictions, 52, 1, data_collector);
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

    vector<Utility *> continuity_utilities;
    continuity_utilities.push_back(&u1);

    Graph g((int) reservoirs.size());
    g.addEdge(0, 1);

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 1},
    };

    vector<double> restriction_stage_multipliers = {0.7, 0.9};
    vector<double> restriction_stage_triggers = {1.1, 1.1};

    Restrictions rp1(0, restriction_stage_triggers, restriction_stage_multipliers, vector<int>(1));

    vector<DroughtMitigationPolicy *> restrictions = {&rp1};

    DataCollectorDeprecated *data_collector = nullptr;

    Simulation s(reservoirs, g, reservoir_utility_connectivity_matrix, continuity_utilities, restrictions, 52, 1, data_collector);
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

    vector<Utility *> continuity_utilities;
    continuity_utilities.push_back(&u1);

    vector<vector<int>> reservoir_connectivity_matrix = {
            {0,  1},
            {-1, 0},
    };

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {1, 1},
    };

    ContinuityModelRealization cmr(reservoirs, reservoir_connectivity_matrix,
                               continuity_utilities, reservoir_utility_connectivity_matrix, 0);

    vector<WaterSource* > ws = cmr.getWaterSources();
    vector<Utility* > u = cmr.getUtilities();

//    Utility u3(*u[0]);
    Utility u3 = u1;
    u3.demand_series[0] = 30;
    u3.total_stored_volume = 5;
    u3.getWaterSource().at(0)->name[0] = 'a'; // remove const from name declaration in WaterSource.h

    cout << u[0]->demand_series[0] << " " << u3.demand_series[0] << endl; // 2 30
    cout << u[0]->total_stored_volume << " " << u3.total_stored_volume << endl; // 0 5
    cout << u[0]->getWaterSource().at(0)->name << " " << u3.getWaterSources().at(0)->name << endl;
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

    vector<WaterSource *> continuity_water_sources;
    continuity_water_sources.push_back(&r1);
    continuity_water_sources.push_back(&r2);

    vector<Utility *> continuity_utilities;
    continuity_utilities.push_back(&u1);
    continuity_utilities.push_back(&u2);

    vector<vector<int>> water_sources_adjacency_matrix = {
            {0,  1},
            {-1, 0},
    };

    vector<vector<int>> water_sources_utility_adjacency_matrix = {
            {1, 0},
            {0, 1}
    };

    ContinuityModelROF* crof = new ContinuityModelROF(Utils::copyWaterSourceVector(continuity_water_sources),
                                                      water_sources_adjacency_matrix,
                                                      Utils::copyUtilityVector(continuity_utilities),
                                                      water_sources_utility_adjacency_matrix,
                                                      SHORT_TERM_ROF, 0);
    vector<double> risks_of_failure;
    crof->setRealization_water_sources(continuity_water_sources);
    risks_of_failure = crof->calculateROF(0); // beginning of 60th year.
    cout << risks_of_failure[0] << " " << risks_of_failure[1]; // The output should be split in 50 blocks of 52 rows and 2 columns.
    // The numbers in the columns are the storage/capacity ratio for
    // each of the two continuity_utilities. The last two numbers are the risks_of_failure,
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

    vector<WaterSource *> continuity_water_sources;
    continuity_water_sources.push_back(&r1);
    continuity_water_sources.push_back(&r2);

    vector<Utility *> continuity_utilities;
    continuity_utilities.push_back(&u1);
    continuity_utilities.push_back(&u2);


    Graph g((int) continuity_water_sources.size());
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

    DataCollectorDeprecated *data_collector = nullptr;

    Simulation s(continuity_water_sources, g, reservoir_utility_connectivity_matrix, continuity_utilities, restrictions, 52, 1,
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

    vector<WaterSource *> continuity_water_sources;
    continuity_water_sources.push_back(&r1);
    continuity_water_sources.push_back(&i1);
    continuity_water_sources.push_back(&r2);

    vector<Utility *> continuity_utilities;
    continuity_utilities.push_back(&u1);


    Graph g((int) continuity_water_sources.size());
    g.addEdge(0, 1);
    g.addEdge(1, 2);

    vector<vector<int>> sources_to_utility_connectivity_matrix = {
            {0, 1, 2}
    };

    vector<DroughtMitigationPolicy *> restrictions;// = {&rp1, &rp2, &rp3};

    DataCollectorDeprecated *data_collector = nullptr;

    Simulation s(continuity_water_sources, g, sources_to_utility_connectivity_matrix, continuity_utilities, restrictions, 104, 2,
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
/*
void simulation3U5RInfraTest() {
    srand((unsigned int) time(NULL));

    cout << "BEGINNING ROF TEST" << endl << endl;

    /// Read streamflows
    int streamflow_n_weeks = 52 * 70;
    vector<vector<double>> streamflows_test = Utils::parse2DCsvFile("../TestFiles/"
                                                                            "inflowsLong.csv");
    vector<vector<double>> demands_test = Utils::parse2DCsvFile("../TestFiles/"
                                                                            "demandsLong.csv");

    vector<vector<double>> demand_to_wastewater_fraction_owasa_raleigh =
            Utils::parse2DCsvFile("../TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");
    vector<vector<double>> demand_to_wastewater_fraction_durham =
            Utils::parse2DCsvFile("../TestFiles/demand_to_wastewater_fraction_durham.csv");

    /// Create catchments and corresponding vectors
    Catchment c1(&streamflows_test, streamflow_n_weeks);
    Catchment c2(&streamflows_test, streamflow_n_weeks);

    vector<Catchment *> catchments1;
    vector<Catchment *> catchments2;
    vector<Catchment *> catchments3;
    catchments1.push_back(&c1);
    catchments1.push_back(&c2);
    catchments2.push_back(&c2);
    catchments3.push_back(&c1);

    /// Create reservoirs and corresponding vector
    vector<double> construction_time_interval_r2 = {1.0, 4.0};
    vector<double> construction_time_interval_rex6 = {1.0, 4.0};

    vector<MinEnvironFlowControl *> mefc;

    Quarry q1("Q1",
              0,
              catchments1,
              100.0,
              20,
              10);
    Reservoir r2("R2",
                 1,
                 catchments2,
                 275.0,
                 20,
                 nullptr,
                 nullptr,
                 0.02,
                 construction_time_interval_r2,
                 5000,
                 20,
                 0.05);
    Reservoir r3("R3",
                 2,
                 catchments3,
                 400.0,
                 20,
                 nullptr,
                 nullptr);
    Reservoir r4("R4",
                 3,
                 catchments2,
                 500.0,
                 20,
                 nullptr,
                 nullptr);
    Reservoir r5("R5",
                 4,
                 catchments3,
                 850.0,
                 20,
                 nullptr,
                 nullptr);
    ReservoirExpansion rex6("R6wx", 5, 3, 200.0, 0.03, construction_time_interval_rex6, 3000, 20, 0.05);
    WaterReuse wr1("WR5", 6, 0.5);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&q1);
    water_sources.push_back(&r2);
    water_sources.push_back(&r3);
    water_sources.push_back(&r4);
    water_sources.push_back(&r5);
    water_sources.push_back(&rex6);
    water_sources.push_back(&wr1);


//     *
//     * System connection diagram (water
//     * flows from top to bottom)
//     *
//     *      0   1
//     *       \ /
//     *        2   4
//     *         \ /
//     *          3
//     *

    Graph g((int) water_sources.size());
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(4, 3);

    /// Waste-water return
    /// Water sourc where waste-water is discharged.
    vector<int> wwtp_return_u1 = {3};
    vector<int> wwtp_return_u2 = {3, 2};
    vector<int> wwtp_return_u3 = {};
    vector<vector<double>> *empty_return_series = new vector<vector<double>>();

    WwtpDischargeRule wwtp_u1(&demand_to_wastewater_fraction_owasa_raleigh, &wwtp_return_u1);
    WwtpDischargeRule wwtp_u2(&demand_to_wastewater_fraction_durham, &wwtp_return_u2);
    WwtpDischargeRule wwtp_u3(empty_return_series, &wwtp_return_u3);

    /// Create catchments and corresponding vector
    Utility u1((char *) "U1", 0, &demands_test, streamflow_n_weeks, 0.03, 1, wwtp_u1);
    Utility u2((char *) "U2", 1,
               &demands_test,
               streamflow_n_weeks,
               0.07,
               1,
               vector<double>(),
               vector<double>(),
               wwtp_u2,
               vector<int>(1,
                           1),
               0.04);
    Utility u3((char *) "U3",
               2,
               &demands_test,
               streamflow_n_weeks,
               0.05,
               1,
               vector<double>(),
               vector<double>(),
               wwtp_u3,
               vector<int>(1,
                           5),
               0.04);

    vector<Utility *> utilities;
    utilities.push_back(&u1);
    utilities.push_back(&u2);
    utilities.push_back(&u3);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 2},
            {1, 4, 6},
            {3, 5}
    };

    /// Restriction policies
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;

    vector<double> restriction_stage_multipliers1 = {0.9, 0.7};
    vector<double> restriction_stage_triggers1 = {0.01, 0.04};

    vector<double> restriction_stage_multipliers2 = {0.8, 0.6};
    vector<double> restriction_stage_triggers2 = {0.03, 0.05};

    Restrictions rp1(0, restriction_stage_multipliers1, restriction_stage_triggers1);
    Restrictions rp2(1, restriction_stage_multipliers2, restriction_stage_triggers2);
    Restrictions rp3(2, restriction_stage_multipliers1, restriction_stage_triggers1);

    drought_mitigation_policies = {&rp1, &rp2, &rp3};
//    drought_mitigation_policies = {&rp1, &rp3};

    /// Transfer policy
     *
     *      0
     *     / \
     *  0 v   v 1
     *   /     \
     *  1--->---2
     *      2
     *

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

//    vector<double> insurance_triggers = {0.06, 0.08, 0.06};
//    vector<double> fixed_payouts = {1., 1., 1.};
//    InsurancePseudoROF in(0, insurance_triggers, 1.2, water_sources, utilities, g,
//                          reservoir_utility_connectivity_matrix,
//                          fixed_payouts);
//    drought_mitigation_policies.push_back(&in);

    double insurance_triggers[3] = {0.06, 0.05, 0.05};
    double fixed_payouts[3] = {1., 1., 1.};
    vector<int> insured_utilities = {0, 1, 2};
    InsuranceStorageToROF in(0,
                             water_sources,
                             g,
                             reservoir_utility_connectivity_matrix,
                             utilities,
                             mefc,
                             insurance_triggers,
                             1.2, fixed_payouts);
    drought_mitigation_policies.push_back(&in);

    /// Data collector pointer
    DataCollectorDeprecated *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources,
                 g,
                 reservoir_utility_connectivity_matrix,
                 utilities,
                 drought_mitigation_policies,
                 mefc,
                 992,
                 2,
                 data_collector);
    cout << "Beginning U3R5 simulation" << endl;
    s.runFullSimulation(2);
    cout << "Ending U3R5 simulation" << endl;
}
*/

struct infraRank {
    int id;
    double xreal;

    infraRank(int id, double xreal) {
        this->id = id;
        this->xreal = xreal;
    }
};

struct by_xreal {
    bool operator()(const infraRank &ir1, const infraRank &ir2) {
        return ir1.xreal < ir2.xreal;
    }
};

vector<int> vecInfraRankToVecInt(vector<infraRank> v) {
    vector<int> sorted;
    for (infraRank ir : v) {
        sorted.push_back(ir.id);
    }

    return sorted;
}

double checkAndFixInfraExpansionHighLowOrder(
        vector<int> *order, int id_low,
        int id_high, double capacity_low, double capacity_high) {

    long pos_low = distance(order->begin(),
                            find(order->begin(),
                                 order->end(),
                                 id_low));

    long pos_high = distance(order->begin(),
                             find(order->begin(),
                                  order->end(),
                                  id_high));

    if (pos_high < pos_low) {
        capacity_high += capacity_low;
        order->erase(order->begin() + pos_low);
    }

    return capacity_high;
}


void triangleTest(int n_threads, const double *x_real, int n_realizations,
                  int n_weeks, int sol_number, string output_directory) {
    srand((unsigned int) time(nullptr));

    // ===================== SET UP DECISION VARIABLES  =====================

    double Durham_restriction_trigger = x_real[0];
    double OWASA_restriction_trigger = x_real[1];
    double raleigh_restriction_trigger = x_real[2];
    double cary_restriction_trigger = x_real[3];
    double durham_transfer_trigger = x_real[4];
    double owasa_transfer_trigger = x_real[5];
    double raleigh_transfer_trigger = x_real[6];
    double OWASA_JLA = x_real[7];
    double Raleigh_JLA = x_real[8];
    double Durham_JLA = x_real[9];
    double Cary_JLA = x_real[10];
    double durham_annual_payment = x_real[11]; // contingency fund
    double owasa_annual_payment = x_real[12];
    double raleigh_annual_payment = x_real[13];
    double cary_annual_payment = x_real[14];
    double durham_insurance_use = x_real[15]; // insurance st_rof
    double owasa_insurance_use = x_real[16];
    double raleigh_insurance_use = x_real[17];
    double cary_insurance_use = x_real[18];
    double durham_insurance_payment = x_real[19];
    double owasa_insurance_payment = x_real[20];
    double raleigh_insurance_payment = x_real[21];
    double cary_insurance_payment = x_real[22];
    double durham_inftrigger = x_real[23];
    double owasa_inftrigger = x_real[24];
    double raleigh_inftrigger = x_real[25];
    double cary_inftrigger = x_real[26];
    double university_lake_expansion_ranking = x_real[27]; // 14
    double Cane_creek_expansion_ranking = x_real[28]; // 24
    double Stone_quarry_reservoir_expansion_shallow_ranking = x_real[29]; // 12
    double Stone_quarry_reservoir_expansion_deep_ranking = x_real[30]; // 13
    double Teer_quarry_expansion_ranking = x_real[31]; // 9
    double reclaimed_water_ranking_low = x_real[32]; // 18
    double reclaimed_water_high = x_real[33]; // 19
    double lake_michie_expansion_ranking_low = x_real[34]; // 15
    double lake_michie_expansion_ranking_high = x_real[35]; // 16
    double little_river_reservoir_ranking = x_real[36]; // 7
    double richland_creek_quarry_rank = x_real[37]; // 8
    double neuse_river_intake_rank = x_real[38]; // 10
    double reallocate_falls_lake_rank = x_real[39]; // 17
    double western_wake_treatment_plant_rank_OWASA_low = x_real[40]; // 20
    double western_wake_treatment_plant_rank_OWASA_high = x_real[41]; // 21
    double western_wake_treatment_plant_rank_durham_low = x_real[42]; // 20
    double western_wake_treatment_plant_rank_durham_high = x_real[43]; // 21
    double western_wake_treatment_plant_rank_raleigh_low = x_real[44]; // 20
    double western_wake_treatment_plant_rank_raleigh_high = x_real[45]; // 21
//    double caryupgrades_1 = x_real[46]; // not used: already built.
    double caryupgrades_2 = x_real[47];
    double caryupgrades_3 = x_real[48];
    double western_wake_treatment_plant_owasa_frac = x_real[49];
    double western_wake_treatment_frac_durham = x_real[50];
    double western_wake_treatment_plant_raleigh_frac = x_real[51];
    double falls_lake_reallocation = x_real[52];
    double durham_inf_buffer = x_real[53];
    double owasa_inf_buffer = x_real[54];
    double raleigh_inf_buffer = x_real[55];
    double cary_inf_buffer = x_real[56];

    vector<infraRank> durham_infra_order_raw = {
            infraRank(9,
                      Teer_quarry_expansion_ranking),
            infraRank(15,
                      lake_michie_expansion_ranking_low),
            infraRank(16,
                      lake_michie_expansion_ranking_high),
            infraRank(18,
                      reclaimed_water_ranking_low),
            infraRank(19,
                      reclaimed_water_high),
            infraRank(20,
                      western_wake_treatment_plant_rank_durham_low),
            infraRank(21,
                      western_wake_treatment_plant_rank_durham_high)
    };

    vector<infraRank> owasa_infra_order_raw = {
            infraRank(12,
                      Stone_quarry_reservoir_expansion_shallow_ranking),
            infraRank(13,
                      Stone_quarry_reservoir_expansion_deep_ranking),
            infraRank(14,
                      university_lake_expansion_ranking),
            infraRank(24,
                      Cane_creek_expansion_ranking),
            infraRank(20,
                      western_wake_treatment_plant_rank_OWASA_low),
            infraRank(21,
                      western_wake_treatment_plant_rank_OWASA_high)
    };

    vector<infraRank> raleigh_infra_order_raw = {
            infraRank(7,
                      little_river_reservoir_ranking),
            infraRank(8,
                      richland_creek_quarry_rank),
            infraRank(10,
                      neuse_river_intake_rank),
            infraRank(17,
                      reallocate_falls_lake_rank),
            infraRank(20,
                      western_wake_treatment_plant_rank_raleigh_low),
            infraRank(21,
                      western_wake_treatment_plant_rank_raleigh_high)
    };

    double added_storage_michie_expansion_low = 2500;
    double added_storage_michie_expansion_high = 5200;
    double reclaimed_capacity_low = 2.2 * 7;
    double reclaimed_capacity_high = 9.1 * 7;

    sort(durham_infra_order_raw.begin(),
         durham_infra_order_raw.end(),
         by_xreal());
    sort(owasa_infra_order_raw.begin(),
         owasa_infra_order_raw.end(),
         by_xreal());
    sort(raleigh_infra_order_raw.begin(),
         raleigh_infra_order_raw.end(),
         by_xreal());

    /// Create catchments and corresponding vector
    vector<int> rof_triggered_infra_order_durham =
            vecInfraRankToVecInt(durham_infra_order_raw);
    vector<int> rof_triggered_infra_order_owasa =
            vecInfraRankToVecInt(owasa_infra_order_raw);
    vector<int> rof_triggered_infra_order_raleigh =
            vecInfraRankToVecInt(raleigh_infra_order_raw);

    added_storage_michie_expansion_high =
            checkAndFixInfraExpansionHighLowOrder(
                    &rof_triggered_infra_order_durham,
                    15,
                    16,
                    added_storage_michie_expansion_low,
                    added_storage_michie_expansion_high);

    reclaimed_capacity_high =
            checkAndFixInfraExpansionHighLowOrder(
                    &rof_triggered_infra_order_durham,
                    18,
                    19,
                    reclaimed_capacity_low,
                    reclaimed_capacity_high);


    /// Normalize Jordan Lake Allocations in case they exceed 1.
    double sum_jla_allocations = OWASA_JLA + Durham_JLA + Cary_JLA +
                                 Raleigh_JLA;
    if (sum_jla_allocations > 1.) {
        OWASA_JLA /= sum_jla_allocations;
        Durham_JLA /= sum_jla_allocations;
        Cary_JLA /= sum_jla_allocations;
        Raleigh_JLA /= sum_jla_allocations;
    }

    /// Normalize West Jordan Lake WTP allocations.
    double sum_wjlwtp = western_wake_treatment_frac_durham +
                        western_wake_treatment_plant_owasa_frac +
                        western_wake_treatment_plant_raleigh_frac;
    western_wake_treatment_frac_durham /= sum_wjlwtp;
    western_wake_treatment_plant_owasa_frac /= sum_wjlwtp;
    western_wake_treatment_plant_raleigh_frac /= sum_wjlwtp;


    // ===================== SET UP PROBLEM COMPONENTS =====================

    cout << "BEGINNING TRIANGLE TEST" << endl << endl;
    cout << "Using " << n_threads << " cores." << endl;
//    cout << getexepath() << endl;

    /// Read streamflows
    int streamflow_n_weeks = 52 * (70 + 50);
    /*
    vector<vector<double>> streamflows_durham = Utils::parse2DCsvFile("../TestFiles/durhamInflowsLong.csv");
    vector<vector<double>> streamflows_flat = Utils::parse2DCsvFile("../TestFiles/flatInflowsLong.csv");
    vector<vector<double>> streamflows_swift = Utils::parse2DCsvFile("../TestFiles/swiftInflowsLong.csv");
    vector<vector<double>> streamflows_llr = Utils::parse2DCsvFile("../TestFiles/littleriverraleighInflowsLong.csv");
    vector<vector<double>> streamflows_crabtree = Utils::parse2DCsvFile("../TestFiles/crabtreeInflowsLong.csv");
    vector<vector<double>> streamflows_phils = Utils::parse2DCsvFile("../TestFiles/philsInflowsLong.csv");
    vector<vector<double>> streamflows_cane = Utils::parse2DCsvFile("../TestFiles/caneInflowsLong.csv");
    vector<vector<double>> streamflows_morgan = Utils::parse2DCsvFile("../TestFiles/morganInflowsLong.csv");
    vector<vector<double>> streamflows_haw = Utils::parse2DCsvFile("../TestFiles/hawInflowsLong.csv");
    vector<vector<double>> streamflows_clayton = Utils::parse2DCsvFile("../TestFiles/claytonInflowsLong.csv");
    vector<vector<double>> streamflows_lillington = Utils::parse2DCsvFile("../TestFiles/lillingtonInflowsLong.csv");

    vector<vector<double>> demand_cary = Utils::parse2DCsvFile("../TestFiles/demandsLongCary.csv");
    vector<vector<double>> demand_durham = Utils::parse2DCsvFile("../TestFiles/demandsLongDurham.csv");
    vector<vector<double>> demand_raleigh = Utils::parse2DCsvFile("../TestFiles/demandsLongRaleigh.csv");
    vector<vector<double>> demand_owasa = Utils::parse2DCsvFile("../TestFiles/demandsLongOWASA.csv");

    vector<vector<double>> evap_durham = Utils::parse2DCsvFile("../TestFiles/evapLongDurham.csv");
    vector<vector<double>> evap_jordan_lake = Utils::parse2DCsvFile("../TestFiles/evapLongCary.csv");
    vector<vector<double>> evap_falls_lake = Utils::parse2DCsvFile("../TestFiles/evapLongFalls.csv");
    vector<vector<double>> evap_owasa = Utils::parse2DCsvFile("../TestFiles/evapLongOWASA.csv");
    vector<vector<double>> evap_little_river = Utils::parse2DCsvFile("../TestFiles/evapLongRaleighOther.csv");
    vector<vector<double>> evap_wheeler_benson = Utils::parse2DCsvFile("../TestFiles/evapLongWB.csv");

    vector<vector<double>> demand_to_wastewater_fraction_owasa_raleigh =
            Utils::parse2DCsvFile("../TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");
    vector<vector<double>> demand_to_wastewater_fraction_durham =
            Utils::parse2DCsvFile("../TestFiles/demand_to_wastewater_fraction_durham.csv");

    vector<vector<double>> caryDemandClassesFractions = Utils::parse2DCsvFile
            ("../TestFiles/caryDemandClassesFractions.csv");
    vector<vector<double>> durhamDemandClassesFractions = Utils::parse2DCsvFile
            ("../TestFiles/durhamDemandClassesFractions.csv");
    vector<vector<double>> raleighDemandClassesFractions = Utils::parse2DCsvFile
            ("../TestFiles/raleighDemandClassesFractions.csv");
    vector<vector<double>> owasaDemandClassesFractions = Utils::parse2DCsvFile
            ("../TestFiles/owasaDemandClassesFractions.csv");

    vector<vector<double>> caryUserClassesWaterPrices = Utils::parse2DCsvFile
            ("../TestFiles/caryUserClassesWaterPrices.csv");
    vector<vector<double>> durhamUserClassesWaterPrices = Utils::parse2DCsvFile
            ("../TestFiles/durhamUserClassesWaterPrices.csv");
    vector<vector<double>> raleighUserClassesWaterPrices = Utils::parse2DCsvFile
            ("../TestFiles/raleighUserClassesWaterPrices.csv");
    vector<vector<double>> owasaUserClassesWaterPrices = Utils::parse2DCsvFile
            ("../TestFiles/owasaUserClassesWaterPrices.csv");

    vector<double> sewageFractions = Utils::parse1DCsvFile(
            "../TestFiles/sewageFractions.csv");
    */

//    vector<vector<double>> streamflows_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamInflowsLong.csv");
//    vector<vector<double>> streamflows_flat = Utils::parse2DCsvFile(output_directory + "/TestFiles/flatInflowsLong.csv");
//    vector<vector<double>> streamflows_swift = Utils::parse2DCsvFile(output_directory + "/TestFiles/swiftInflowsLong.csv");
//    vector<vector<double>> streamflows_llr = Utils::parse2DCsvFile(output_directory + "/TestFiles/littleriverraleighInflowsLong.csv");
//    vector<vector<double>> streamflows_crabtree = Utils::parse2DCsvFile(output_directory + "/TestFiles/crabtreeInflowsLong.csv");
//    vector<vector<double>> streamflows_phils = Utils::parse2DCsvFile(output_directory + "/TestFiles/philsInflowsLong.csv");
//    vector<vector<double>> streamflows_cane = Utils::parse2DCsvFile(output_directory + "/TestFiles/caneInflowsLong.csv");
//    vector<vector<double>> streamflows_morgan = Utils::parse2DCsvFile(output_directory + "/TestFiles/morganInflowsLong.csv");
//    vector<vector<double>> streamflows_haw = Utils::parse2DCsvFile(output_directory + "/TestFiles/hawInflowsLong.csv");
//    vector<vector<double>> streamflows_clayton = Utils::parse2DCsvFile(output_directory + "/TestFiles/claytonInflowsLong.csv");
//    vector<vector<double>> streamflows_lillington = Utils::parse2DCsvFile(output_directory + "/TestFiles/lillingtonInflowsLong.csv");
//
//    vector<vector<double>> demand_cary = Utils::parse2DCsvFile(output_directory + "/TestFiles/demandsLongCary.csv");
//    vector<vector<double>> demand_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/demandsLongDurham.csv");
//    vector<vector<double>> demand_raleigh = Utils::parse2DCsvFile(output_directory + "/TestFiles/demandsLongRaleigh.csv");
//    vector<vector<double>> demand_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/demandsLongOWASA.csv");
//
//    vector<vector<double>> evap_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/evapLongDurham.csv");
//    vector<vector<double>> evap_jordan_lake = Utils::parse2DCsvFile(output_directory + "/TestFiles/evapLongCary.csv");
//    vector<vector<double>> evap_falls_lake = Utils::parse2DCsvFile(output_directory + "/TestFiles/evapLongFalls.csv");
//    vector<vector<double>> evap_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/evapLongOWASA.csv");
//    vector<vector<double>> evap_little_river = Utils::parse2DCsvFile(output_directory + "/TestFiles/evapLongRaleighOther.csv");
//    vector<vector<double>> evap_wheeler_benson = Utils::parse2DCsvFile(output_directory + "/TestFiles/evapLongWB.csv");

    int max_lines = n_realizations;

    cout << "Reading inflows." << endl;
    vector<vector<double>> streamflows_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/durham_inflows.csv",
                                                                      max_lines);
    vector<vector<double>> streamflows_flat = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/falls_lake_inflows.csv",
                                                                    max_lines);
    vector<vector<double>> streamflows_swift = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/lake_wb_inflows.csv",
                                                                     max_lines);
    vector<vector<double>> streamflows_llr = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/little_river_raleigh_inflows.csv",
                                                                   max_lines);
    vector<vector<double>> streamflows_crabtree = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/crabtree_inflows.csv",
                                                                        max_lines);

    vector<vector<double>> streamflows_phils = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/stone_quarry_inflows.csv",
                                                                     max_lines);
    vector<vector<double>> streamflows_cane = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/cane_creek_inflows.csv",
                                                                    max_lines);
    vector<vector<double>> streamflows_morgan = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/university_lake_inflows.csv",
                                                                      max_lines);
    vector<vector<double>> streamflows_haw = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/jordan_lake_inflows.csv",
                                                                   max_lines);
    vector<vector<double>> streamflows_clayton = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/clayton_inflows.csv",
                                                                       max_lines);

    vector<vector<double>> streamflows_lillington = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows/lillington_inflows.csv",
                                                                          max_lines);

    cout << "Reading demands." << endl;
    vector<vector<double>> demand_cary = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/cary_demand.csv",
                                                               max_lines);
    vector<vector<double>> demand_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/durham_demand.csv",
                                                                 max_lines);
    vector<vector<double>> demand_raleigh = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/raleigh_demand.csv",
                                                                  max_lines);
    vector<vector<double>> demand_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/owasa_demand.csv",
                                                                max_lines);

    cout << "Reading evaporations." << endl;
    vector<vector<double>> evap_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation/durham_evap.csv",
                                                               max_lines);
    vector<vector<double>> evap_falls_lake = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation/falls_lake_evap.csv",
                                                                   max_lines);
    vector<vector<double>> evap_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation/owasa_evap.csv",
                                                              max_lines);
    vector<vector<double>> evap_little_river = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation/little_river_raleigh_evap.csv",
                                                                     max_lines);
    vector<vector<double>> evap_wheeler_benson = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation/wb_evap.csv",
                                                                       max_lines);
    vector<vector<double>> evap_jordan_lake = evap_owasa;

    cout << "Reading others." << endl;
    vector<vector<double>> demand_to_wastewater_fraction_owasa_raleigh =
            Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");
    vector<vector<double>> demand_to_wastewater_fraction_durham =
            Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_durham.csv");
//
    vector<vector<double>> caryDemandClassesFractions = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/caryDemandClassesFractions.csv");
    vector<vector<double>> durhamDemandClassesFractions = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/durhamDemandClassesFractions.csv");
    vector<vector<double>> raleighDemandClassesFractions = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/raleighDemandClassesFractions.csv");
    vector<vector<double>> owasaDemandClassesFractions = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/owasaDemandClassesFractions.csv");

    vector<vector<double>> caryUserClassesWaterPrices = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/caryUserClassesWaterPrices.csv");
    vector<vector<double>> durhamUserClassesWaterPrices = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/durhamUserClassesWaterPrices.csv");
    vector<vector<double>> raleighUserClassesWaterPrices = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/raleighUserClassesWaterPrices.csv");
    vector<vector<double>> owasaUserClassesWaterPrices = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/owasaUserClassesWaterPrices.csv");

    vector<vector<double>> owasaPriceSurcharges = Utils::parse2DCsvFile
            (output_directory + "/TestFiles/owasaPriceRestMultipliers.csv");

//    vector<double> sewageFractions = Utils::parse1DCsvFile(
//            output_directory + "/TestFiles/sewageFractions.csv");

    EvaporationSeries evaporation_durham(&evap_durham, streamflow_n_weeks);
    EvaporationSeries evaporation_jordan_lake(
            &evap_jordan_lake,
            streamflow_n_weeks);
    EvaporationSeries evaporation_falls_lake(
            &evap_falls_lake,
            streamflow_n_weeks);
    EvaporationSeries evaporation_owasa(&evap_owasa, streamflow_n_weeks);
    EvaporationSeries evaporation_little_river(
            &evap_little_river,
            streamflow_n_weeks);
    EvaporationSeries evaporation_wheeler_benson(
            &evap_wheeler_benson,
            streamflow_n_weeks);

    /// Create catchments and corresponding vectors
    //  Durham (Upper Neuse River Basin)
    Catchment durham_inflows(&streamflows_durham, streamflow_n_weeks);

    //  Raleigh (Lower Neuse River Basin)
    Catchment lower_flat_river(&streamflows_flat, streamflow_n_weeks);
    Catchment swift_creek(&streamflows_swift, streamflow_n_weeks);
    Catchment little_river_raleigh(&streamflows_llr, streamflow_n_weeks);
    Catchment crabtree_creek(&streamflows_crabtree, streamflow_n_weeks);

    // OWASA (Upper Cape Fear Basin)
    Catchment phils_reek(&streamflows_phils, streamflow_n_weeks);
    Catchment cane_creek(&streamflows_cane, streamflow_n_weeks);
    Catchment morgan_creek(&streamflows_morgan, streamflow_n_weeks);

    // Cary (Lower Cape Fear Basin)
    Catchment lower_haw_river(&streamflows_haw, streamflow_n_weeks);

    // Downstream Gages
    Catchment neuse_river_at_clayton(&streamflows_clayton, streamflow_n_weeks);
    Catchment cape_fear_river_at_lillington(
            &streamflows_lillington,
            streamflow_n_weeks);

    vector<Catchment *> catchment_durham;

    vector<Catchment *> catchment_flat;
    vector<Catchment *> catchment_swift;
    vector<Catchment *> catchment_little_river_raleigh;
    vector<Catchment *> catchment_crabtree;

    vector<Catchment *> catchment_phils;
    vector<Catchment *> catchment_cane;
    vector<Catchment *> catchment_morgan;

    vector<Catchment *> catchment_haw;

    vector<Catchment *> gage_clayton;
    vector<Catchment *> gage_lillington;

    catchment_durham.push_back(&durham_inflows);

    catchment_flat.push_back(&lower_flat_river);
    catchment_swift.push_back(&swift_creek);
    catchment_little_river_raleigh.push_back(&little_river_raleigh);
    catchment_crabtree.push_back(&crabtree_creek);

    catchment_phils.push_back(&phils_reek);
    catchment_cane.push_back(&cane_creek);
    catchment_morgan.push_back(&morgan_creek);

    catchment_haw.push_back(&lower_haw_river);

    gage_clayton.push_back(&neuse_river_at_clayton);
    gage_lillington.push_back(&cape_fear_river_at_lillington);

    /// Storage vs. area reservoir curves.
    vector<double> falls_lake_storage = {0, 23266, 34700};
    vector<double> falls_lake_area = {0.32 * 5734, 0.32 * 29000, 0.28 * 40434};
    vector<double> wheeler_benson_storage = {0, 2789.66};
    vector<double> wheeler_benson_area = {0, 0.3675 * 2789.66};
    vector<double> teer_storage = {0, 1315.0};
    vector<double> teer_area = {20, 50};
    vector<double> little_river_res_storage = {0, 3700};
    vector<double> little_river_res_area = {0, 0.3675 * 3700};

    DataSeries falls_lake_storage_area(&falls_lake_storage, &falls_lake_area);
    DataSeries wheeler_benson_storage_area(&wheeler_benson_storage,
                                           &wheeler_benson_area);
    DataSeries teer_storage_area(&teer_storage,
                                 &teer_area);
    DataSeries little_river_storage_area(&little_river_res_storage,
                                         &little_river_res_area);

    /// Minimum environmental flow rules (controls)
    vector<int> dlr_weeks = {0, 21, 47, 53};
    vector<double> dlr_releases = {3.877 * 7, 9.05, 3.877 * 7};
    vector<double> wb_storage = {0.3 * 2789.66, 0.6 * 2789.66, 2789.66};
    vector<double> wb_releases = {0.646 * 7, 1.29 * 7, 1.94 * 7};
    vector<double> ccr_inflows = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                  1.797 * 7};
    vector<double> ccr_releases = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                   1.797 * 7};
    int falls_controls_weeks[2] = {13, 43};
    double falls_base_releases[2] = {64.64 * 7, 38.78 * 7};
    double falls_min_gage[2] = {180 * 7, 130 * 7};

    SeasonalMinEnvFlowControl durham_min_env_control(0, &dlr_weeks,
                                                     &dlr_releases);
//    FixedMinEnvFlowControl falls_min_env_control(1, 38.78 * 7);
    FallsLakeMinEnvFlowControl falls_min_env_control(1,
                                                     10,
                                                     falls_controls_weeks,
                                                     falls_base_releases,
                                                     falls_min_gage,
                                                     crabtree_creek);

    StorageMinEnvFlowControl wheeler_benson_min_env_control(2,
                                                            vector<int>(1,
                                                                        2),
                                                            &wb_storage,
                                                            &wb_releases);
    FixedMinEnvFlowControl sq_min_env_control(3, 0);
    InflowMinEnvFlowControl ccr_min_env_control(4, vector<int>(1, 4),
                                                &ccr_inflows,
                                                &ccr_releases);
    FixedMinEnvFlowControl university_min_env_control(5, 0);
//    FixedMinEnvFlowControl jordan_min_env_control(6,
//                                                  25.8527 * 7);
    JordanLakeMinEnvFlowControl jordan_min_env_control(
            6, &cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
            290.84, 387.79, 30825.0, 14924.0);
    SeasonalMinEnvFlowControl little_river_min_env_control(7, &dlr_weeks,
                                                           &dlr_releases);
    FixedMinEnvFlowControl richland_min_env_control(8, 0);
    FixedMinEnvFlowControl teer_min_env_control(9, 0);
    FixedMinEnvFlowControl neuse_intake_min_env_control(10, 38.78 * 7);

//    vector<int> eno_weeks = {7, 16, 53};
//    vector<double> eno_releases = {6.49 * 7, 19.48 * 7, 6.49 * 7};
//    SeasonalMinEnvFlowControl eno_min_env_control(&eno_weeks, &eno_releases);

    vector<MinEnvironFlowControl *> min_env_flow_controls;
    min_env_flow_controls.push_back(&durham_min_env_control);
    min_env_flow_controls.push_back(&falls_min_env_control);
    min_env_flow_controls.push_back(&wheeler_benson_min_env_control);
    min_env_flow_controls.push_back(&sq_min_env_control);
    min_env_flow_controls.push_back(&ccr_min_env_control);
    min_env_flow_controls.push_back(&university_min_env_control);
    min_env_flow_controls.push_back(&jordan_min_env_control);
    min_env_flow_controls.push_back(&little_river_min_env_control);
    min_env_flow_controls.push_back(&richland_min_env_control);
    min_env_flow_controls.push_back(&teer_min_env_control);
    min_env_flow_controls.push_back(&neuse_intake_min_env_control);

    /// Create reservoirs and corresponding vector
    vector<double> construction_time_interval = {3.0, 5.0};
    vector<double> city_infrastructure_rof_triggers = {owasa_inftrigger,
                                                       durham_inftrigger,
                                                       cary_inftrigger,
                                                       raleigh_inftrigger};
    vector<double> bond_length = {25, 25, 25, 25};
    vector<double> bond_rate = {0.05, 0.05, 0.05, 0.05};

    /// Jordan Lake parameters
    double jl_supply_capacity = 14924.0;
    double jl_wq_capacity = 30825.0;
    double jl_storage_capacity = jl_wq_capacity + jl_supply_capacity;
    vector<int> jl_allocations_ids = {0, 1, 2, 3, WATER_QUALITY_ALLOCATION};
    vector<double> jl_allocation_fractions = {
            OWASA_JLA * jl_supply_capacity / jl_storage_capacity,
            Durham_JLA * jl_supply_capacity / jl_storage_capacity,
            Cary_JLA * jl_supply_capacity / jl_storage_capacity,
            Raleigh_JLA * jl_supply_capacity / jl_storage_capacity,
            jl_wq_capacity / jl_storage_capacity};
    vector<double> jl_treatment_allocation_fractions = {0.0, 0.0, 1.0, 0.0};

    /// Jordan Lake parameters
    double fl_supply_capacity = 14700.0;
    double fl_wq_capacity = 20000.0;
    double fl_storage_capacity = fl_wq_capacity + fl_supply_capacity;
    vector<int> fl_allocations_ids = {3, WATER_QUALITY_ALLOCATION};
    vector<double> fl_allocation_fractions = {
            fl_supply_capacity / fl_storage_capacity,
            fl_wq_capacity / fl_storage_capacity};
    vector<double> fl_treatment_allocation_fractions = {0.0, 0.0, 0.0, 1.0};

    // Existing Sources
    Reservoir durham_reservoirs("Lake Michie & Little River Res. (Durham)",
                                0,
                                catchment_durham,
                                6349.0,
                                ILLIMITED_TREATMENT_CAPACITY,
                                &evaporation_durham, 1069);
//    Reservoir falls_lake("Falls Lake", 1, catchment_flat,
//                         34700.0, 99999,
//                         &evaporation_falls_lake, &falls_lake_storage_area);
    AllocatedReservoir falls_lake("Falls Lake",
                                  1,
                                  catchment_flat,
                                  fl_storage_capacity,
                                  ILLIMITED_TREATMENT_CAPACITY,
                                  &evaporation_falls_lake,
                                  &falls_lake_storage_area,
                                  &fl_allocations_ids,
                                  &fl_allocation_fractions,
                                  &fl_treatment_allocation_fractions);

    Reservoir wheeler_benson_lakes("Wheeler-Benson Lakes", 2, catchment_swift,
                                   2789.66,
                                   ILLIMITED_TREATMENT_CAPACITY,
                                   &evaporation_wheeler_benson,
                                   &wheeler_benson_storage_area);
    Reservoir stone_quarry("Stone Quarry",
                           3,
                           catchment_phils,
                           200.0,
                           ILLIMITED_TREATMENT_CAPACITY,
                           &evaporation_owasa,
                           10);
    Reservoir ccr("Cane Creek Reservoir",
                  4,
                  catchment_cane,
                  2909.0,
                  ILLIMITED_TREATMENT_CAPACITY,
                  &evaporation_owasa,
                  500);
    Reservoir university_lake("University Lake", 5, catchment_morgan, 449.0,
                              ILLIMITED_TREATMENT_CAPACITY,
                              &evaporation_owasa,
                              212);
//    Reservoir jordan_lake("Jordan Lake", 6, catchment_haw,
//                          jl_supply_capacity, 448,
//                          &evaporation_jordan_lake, 13940);
    AllocatedReservoir jordan_lake("Jordan Lake",
                                   6,
                                   catchment_haw,
                                   jl_storage_capacity,
                                   448,
                                   &evaporation_jordan_lake,
                                   13940,
                                   &jl_allocations_ids,
                                   &jl_allocation_fractions,
                                   &jl_treatment_allocation_fractions);

    // other than Cary WTP for Jordan Lake, assume no WTP constraints - each
    // city can meet its daily demands with available treatment infrastructure

    double WEEKS_IN_YEAR = 0;

    // Potential Sources
    // The capacities listed here for expansions are what additional capacity is gained relative to existing capacity,
    // NOT the total capacity after the expansion is complete. For infrastructure with a high and low option, this means
    // the capacity for both is relative to current conditions - if Lake Michie is expanded small it will gain 2.5BG,
    // and a high expansion will provide 7.7BG more water than current. if small expansion happens, followed by a large
    // expansion, the maximum capacity through expansion is 7.7BG, NOT 2.5BG + 7.7BG.
    Reservoir little_river_reservoir("Little River Reservoir (Raleigh)", 7,
                                     catchment_little_river_raleigh, 3700.0,
                                     ILLIMITED_TREATMENT_CAPACITY,
                                     &evaporation_little_river,
                                     &little_river_storage_area,
                                     city_infrastructure_rof_triggers[3],
                                     construction_time_interval,
                                     17 *
                                     WEEKS_IN_YEAR,
                                     263.0,
                                     bond_length[3], bond_rate[3]);
    Quarry richland_creek_quarry("Richland Creek Quarry", 8, gage_clayton,
                                 4000.0,
                                 ILLIMITED_TREATMENT_CAPACITY,
                                 &evaporation_falls_lake,
                                 100.,
                                 city_infrastructure_rof_triggers[3],
                                 construction_time_interval,
                                 17 *
                                 WEEKS_IN_YEAR,
                                 400.0,
                                 bond_length[3],
                                 bond_rate[3],
                                 50 * 7);
    // diversions to Richland Creek Quarry based on ability to meet downstream flow target at Clayton, NC
    Quarry teer_quarry("Teer Quarry",
                       9,
                       vector<Catchment *>(),
                       1315.0,
                       ILLIMITED_TREATMENT_CAPACITY,
                       &evaporation_falls_lake,
                       &teer_storage_area,
                       city_infrastructure_rof_triggers[1],
                       construction_time_interval,
                       7 *
                       WEEKS_IN_YEAR,
                       22.6,
                       bond_length[1],
                       bond_rate[1],
                       15 * 7); //FIXME: MAX PUMPING CAPACITY?
    //Reservoir rNeuseRiverIntake("rNeuseRiverIntake", 10, 0, catchment_flat, 16.0, 99999, city_infrastructure_rof_triggers[3], construction_time_interval, 5000, 20, 0.05);
    Intake neuse_river_intake("Neuse River Intake", 10, catchment_flat, 16 * 7,
                              city_infrastructure_rof_triggers[3],
                              construction_time_interval,
                              17 * WEEKS_IN_YEAR,
                              225.5,
                              bond_length[3],
                              bond_rate[3]);
    // diversions to Teer Quarry for Durham based on inflows to downstream Falls Lake from the Flat River
    // FYI: spillage from Eno River also helps determine Teer quarry diversion, but Eno spillage isn't factored into
    // downstream water balance?

    vector<double> fl_relocation_fractions = {
            (fl_supply_capacity + falls_lake_reallocation) /
            fl_storage_capacity,
            (fl_wq_capacity - falls_lake_reallocation) / fl_storage_capacity};
    Relocation fl_reallocation("Falls Lake Reallocation",
                               17,
                               1,
                               &fl_relocation_fractions,
                               &fl_allocations_ids,
                               city_infrastructure_rof_triggers[3],
                               construction_time_interval,
                               12 * WEEKS_IN_YEAR,
                               68.2,
                               bond_length[3],
                               bond_rate[3]);
    ReservoirExpansion ccr_expansion("Cane Creek Reservoir Expansion",
                                     24,
                                     4,
                                     3000.0,
                                     city_infrastructure_rof_triggers[2],
                                     construction_time_interval,
                                     17 * WEEKS_IN_YEAR,
                                     127.0,
                                     bond_length[2], bond_rate[2]);
    ReservoirExpansion low_sq_expansion("Low Stone Quarry Expansion", 12, 3,
                                        1500.0,
                                        city_infrastructure_rof_triggers[2],
                                        construction_time_interval,
                                        23 * WEEKS_IN_YEAR,
                                        1.4,
                                        bond_length[2], bond_rate[2]);
    ReservoirExpansion high_sq_expansion("High Stone Quarry Expansion", 13, 3,
                                         2200.0,
                                         city_infrastructure_rof_triggers[2],
                                         construction_time_interval,
                                         23 * WEEKS_IN_YEAR,
                                         64.6,
                                         bond_length[2], bond_rate[2]);
    ReservoirExpansion univ_lake_expansion("University Lake Expansion",
                                           14,
                                           5,
                                           2550.0,
                                           city_infrastructure_rof_triggers[2],
                                           construction_time_interval,
                                           17 * WEEKS_IN_YEAR,
                                           107.0,
                                           bond_length[2], bond_rate[2]);
    ReservoirExpansion low_michie_expansion("Low Lake Michie Expansion",
                                            15,
                                            0,
                                            added_storage_michie_expansion_low,
                                            city_infrastructure_rof_triggers[1],
                                            construction_time_interval,
                                            17 * WEEKS_IN_YEAR,
                                            158.3,
                                            bond_length[1],
                                            bond_rate[1]);
    ReservoirExpansion high_michie_expansion("High Lake Michie Expansion",
                                             16,
                                             0,
                                             added_storage_michie_expansion_high,
                                             city_infrastructure_rof_triggers[1],
                                             construction_time_interval,
                                             17 * WEEKS_IN_YEAR,
                                             203.3,
                                             bond_length[1], bond_rate[1]);
    WaterReuse low_reclaimed("Low Reclaimed Water System",
                             18,
                             reclaimed_capacity_low,
                             city_infrastructure_rof_triggers[1],
                             construction_time_interval,
                             7 * WEEKS_IN_YEAR,
                             27.5,
                             bond_length[1],
                             bond_rate[1]);
    WaterReuse high_reclaimed("High Reclaimed Water System",
                              19,
                              reclaimed_capacity_high,
                              city_infrastructure_rof_triggers[1],
                              construction_time_interval,
                              7 * WEEKS_IN_YEAR,
                              104.4,
                              bond_length[1],
                              bond_rate[1]);

    WEEKS_IN_YEAR = Constants::WEEKS_IN_YEAR;

    vector<double> wjlwtp_treatment_capacity_fractions =
            {western_wake_treatment_plant_owasa_frac,
             western_wake_treatment_frac_durham,
             0.,
             western_wake_treatment_plant_raleigh_frac,
             0.};
    vector<double> cary_upgrades_treatment_capacity_fractions = {0., 0., 1.,
                                                                 0., 0.};

    vector<double> *shared_added_wjlwtp_treatment_pool = new vector<double>();
    vector<double> *shared_added_wjlwtp_price = new vector<double>();
    SequentialJointTreatmentExpansion low_wjlwtp("Low WJLWTP",
                                                 20,
                                                 6,
                                                 0,
                                                 33 * 7,
                                                 &wjlwtp_treatment_capacity_fractions,
                                                 shared_added_wjlwtp_treatment_pool,
                                                 shared_added_wjlwtp_price,
                                                 city_infrastructure_rof_triggers[1],
                                                 construction_time_interval,
                                                 12 * WEEKS_IN_YEAR,
                                                 243.3,
                                                 bond_length[1],
                                                 bond_rate[1]);
    SequentialJointTreatmentExpansion high_wjlwtp("High WJLWTP",
                                                  21,
                                                  6,
                                                  0,
                                                  54 * 7,
                                                  &wjlwtp_treatment_capacity_fractions,
                                                  shared_added_wjlwtp_treatment_pool,
                                                  shared_added_wjlwtp_price,
                                                  city_infrastructure_rof_triggers[1],
                                                  construction_time_interval,
                                                  12 * WEEKS_IN_YEAR,
                                                  316.8,
                                                  bond_length[1],
                                                  bond_rate[1]);
    SequentialJointTreatmentExpansion caryWtpUpgrade1("Cary WTP upgrade 1",
                                                      22,
                                                      6,
                                                      56, // (72*7 - 448 = 56)
                                             &cary_upgrades_treatment_capacity_fractions,
                                             caryupgrades_2 * 7,
                                                      construction_time_interval,
                                                      NONE,
                                                      243. / 2,
                                                      bond_length[1],
                                                      bond_rate[1]);
    SequentialJointTreatmentExpansion caryWtpUpgrade2("Cary WTP upgrade 2",
                                                      23,
                                                      6,
                                                      56, // (7*80 - 72*7 = 56)
                                             &cary_upgrades_treatment_capacity_fractions,
                                             caryupgrades_3 * 7,
                                                      construction_time_interval,
                                                      NONE,
                                                      316.8 / 2,
                                                      bond_length[1],
                                                      bond_rate[1]);
//    Reservoir low_wjlwtp_durham("Low WJLWTP (Durham)", 20, 0, catchment_haw,
//                                jl_storage_capacity * JL_allocation_fractions[1], 33.0 * JL_allocation_fractions[1],
//                                &evaporation_jordan_lake, 13940,
//                                city_infrastructure_rof_triggers[1], construction_time_interval,
//                                243.3 * JL_allocation_fractions[1], bond_length[1], bond_rate[1]);
//    Reservoir high_wjlwtp_durham("High WJLWTP (Durham)", 21, 0, catchment_haw,
//                                 jl_storage_capacity * JL_allocation_fractions[1], 54.0 * JL_allocation_fractions[1],
//                                 &evaporation_jordan_lake, 13940,
//                                 city_infrastructure_rof_triggers[1], construction_time_interval,
//                                 73.5 * JL_allocation_fractions[1], bond_length[1], bond_rate[1]);
//    Reservoir low_wjlwtp_owasa("Low WJLWTP (OWASA)", 22, 0, catchment_haw,
//                               jl_storage_capacity * JL_allocation_fractions[2], 33.0 * JL_allocation_fractions[2],
//                               &evaporation_jordan_lake, 13940,
//                               city_infrastructure_rof_triggers[2], construction_time_interval,
//                               243.3 * JL_allocation_fractions[2], bond_length[2], bond_rate[2]);
//    Reservoir high_wjlwtp_owasa("High WJLWTP (OWASA)", 23, 0, catchment_haw,
//                                jl_storage_capacity * JL_allocation_fractions[2], 54.0 * JL_allocation_fractions[2],
//                                &evaporation_jordan_lake, 13940,
//                                city_infrastructure_rof_triggers[2], construction_time_interval,
//                                73.5 * JL_allocation_fractions[2], bond_length[2], bond_rate[2]);
//    Reservoir low_wjlwtp_raleigh("Low WJLWTP (Raleigh)", 24, 0, catchment_haw,
//                                 jl_storage_capacity * JL_allocation_fractions[3], 33.0 * JL_allocation_fractions[3],
//                                 &evaporation_jordan_lake, 13940,
//                                 city_infrastructure_rof_triggers[3], construction_time_interval,
//                                 243.3 * JL_allocation_fractions[3], bond_length[3], bond_rate[3]);
//    Reservoir high_wjlwtp_raleigh("High WJLWTP (Raleigh)", 25, 0, catchment_haw,
//                                  jl_storage_capacity * JL_allocation_fractions[4], 54.0 * JL_allocation_fractions[3],
//                                  &evaporation_jordan_lake, 13940,
//                                  city_infrastructure_rof_triggers[3], construction_time_interval,
//                                  73.5 * JL_allocation_fractions[3], bond_length[3], bond_rate[3]);

    Reservoir dummy_endpoint("Dummy Node", 11, vector<Catchment *>(), 0, 0,
                             &evaporation_durham, 1, 1,
                             construction_time_interval,
                             0,
                             0,
                             0,
                             0);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&durham_reservoirs);
    water_sources.push_back(&falls_lake);
    water_sources.push_back(&wheeler_benson_lakes);
    water_sources.push_back(&stone_quarry);
    water_sources.push_back(&ccr);
    water_sources.push_back(&university_lake);
    water_sources.push_back(&jordan_lake);

    water_sources.push_back(&little_river_reservoir);
    water_sources.push_back(&richland_creek_quarry);
    water_sources.push_back(&teer_quarry);
    water_sources.push_back(&fl_reallocation);
    water_sources.push_back(&ccr_expansion);
    water_sources.push_back(&univ_lake_expansion);
    water_sources.push_back(&neuse_river_intake);
    water_sources.push_back(&low_sq_expansion);
    water_sources.push_back(&low_michie_expansion);
    water_sources.push_back(&low_reclaimed);
    water_sources.push_back(&high_sq_expansion);
    water_sources.push_back(&high_michie_expansion);
    water_sources.push_back(&high_reclaimed);

    water_sources.push_back(&caryWtpUpgrade1);
    water_sources.push_back(&caryWtpUpgrade2);
    water_sources.push_back(&low_wjlwtp);
    water_sources.push_back(&high_wjlwtp);

    water_sources.push_back(&dummy_endpoint);

    /*
     * System connection diagram (water
     * flows from top to bottom)
     * Potential projects and expansions
     * of existing sources in parentheses
     *
     *      3(12,13)   4(24)   5(14)            0(15,16)    (18,19)
     *         \         /      /                  |
     *          \       /      /                   |
     *           \     /      /                    |
     *           |    /      /                    (9)
     *           |   /      /                      \
     *           |   |     /                        \
     *           |   |    /                          1(17)              2   (7)
     *           |   |   /                             |                |    |
     *           |   |  /                              |                |    |
     *           6(20-25)                             (8)               |    |
     *              |                                   \               |    |
     *              |                                    \              |    |
     *        Lillington Gage                            (10)           |    |
     *              |                                      |            |    |
     *              |                                      |           /    /
     *              |                                      |          /    /
     *              |                                 Clayton Gage   /    /
     *              |                                      |        /    /
     *               \                                     |   -----    /
     *                \                                     \ /        /
     *                 \                                     |    -----
     *                  \                                    |   /
     *                   \                                   |  /
     *                    \                                   \/
     *                     -------                            /
     *                            \             --------------
     *                             \           /
     *                              \     -----
     *                               \   /
     *                                \ /
     *                                11
     */

    Graph g(12);
    g.addEdge(0, 9);
    g.addEdge(9, 1);
    g.addEdge(1, 8);
    g.addEdge(8, 10);
    g.addEdge(10, 11);
    g.addEdge(2, 11);
    g.addEdge(7, 11);

    g.addEdge(3, 6);
    g.addEdge(4, 6);
    g.addEdge(5, 6);
    g.addEdge(6, 11);

    vector<int> demand_triggered_infra_order_cary = {22, 23};

    int demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

    vector<int> cary_ws_return_id = {};
    vector<vector<double>> *cary_discharge_fraction_series =
            new vector<vector<double>>();
    WwtpDischargeRule wwtp_discharge_cary(
            cary_discharge_fraction_series,
            &cary_ws_return_id);
    vector<int> owasa_ws_return_id = {6};
    WwtpDischargeRule wwtp_discharge_owasa(
            &demand_to_wastewater_fraction_owasa_raleigh,
            &owasa_ws_return_id);
    vector<int> raleigh_ws_return_id = {8};
    WwtpDischargeRule wwtp_discharge_raleigh(
            &demand_to_wastewater_fraction_owasa_raleigh,
            &raleigh_ws_return_id);
    vector<int> durham_ws_return_id = {1, 6};
    WwtpDischargeRule wwtp_discharge_durham(
            &demand_to_wastewater_fraction_durham,
            &durham_ws_return_id);

    vector<vector<int>> wjlwtp_remove_from_to_build_list;// = {{21, 20}};

    Utility cary((char *) "Cary",
                 2,
                 &demand_cary,
                 demand_n_weeks,
                 cary_annual_payment,
                 &caryDemandClassesFractions,
                 &caryUserClassesWaterPrices,
                 wwtp_discharge_cary,
                 cary_inf_buffer,
                 vector<int>(),
                 demand_triggered_infra_order_cary,
                 0.05);
    Utility durham((char *) "Durham",
                   1,
                   &demand_durham,
                   demand_n_weeks,
                   durham_annual_payment,
                   &durhamDemandClassesFractions,
                   &durhamUserClassesWaterPrices,
                   wwtp_discharge_durham,
                   durham_inf_buffer,
                   rof_triggered_infra_order_durham,
                   vector<int>(),
                   0.05,
                   &wjlwtp_remove_from_to_build_list);
    Utility owasa((char *) "OWASA",
                  0,
                  &demand_owasa,
                  demand_n_weeks,
                  owasa_annual_payment,
                  &owasaDemandClassesFractions,
                  &owasaUserClassesWaterPrices,
                  wwtp_discharge_owasa,
                  owasa_inf_buffer,
                  rof_triggered_infra_order_owasa,
                  vector<int>(),
                  0.05,
                  &wjlwtp_remove_from_to_build_list);
    Utility raleigh((char *) "Raleigh",
                    3,
                    &demand_raleigh,
                    demand_n_weeks,
                    raleigh_annual_payment,
                    &raleighDemandClassesFractions,
                    &raleighUserClassesWaterPrices,
                    wwtp_discharge_raleigh,
                    raleigh_inf_buffer,
                    rof_triggered_infra_order_raleigh,
                    vector<int>(),
                    0.05,
                    &wjlwtp_remove_from_to_build_list);

    vector<Utility *> utilities;
    utilities.push_back(&cary);
    utilities.push_back(&durham);
    utilities.push_back(&owasa);
    utilities.push_back(&raleigh);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
//            {6},
//            {0, 9, 15, 16, 18, 19, 20, 21},
//            {3, 4, 5,  26, 12, 13, 14, 22, 23, 20, 21},
//            {1, 2, 7,  8,  17, 10, 24, 25, 20, 21}
            {3, 4,  5, 6,  12, 13, 14, 20, 21, 24}, //OWASA
            {0, 6,  9, 15, 16, 18, 19, 20, 21}, //Durham
            {6, 22, 23},                    //Cary
            {1, 2,  6, 7,  8,  17, 10, 20, 21}  //Raleigh
    };

    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    /// Restriction policies
    vector<double> initial_restriction_triggers = {OWASA_restriction_trigger,
                                                   Durham_restriction_trigger,
                                                   cary_restriction_trigger,
                                                   raleigh_restriction_trigger};

    vector<double> restriction_stage_multipliers_cary = {0.9, 0.8, 0.7, 0.6};
    vector<double> restriction_stage_triggers_cary = {initial_restriction_triggers[0],
                                                      initial_restriction_triggers[0] + 0.15,
                                                      initial_restriction_triggers[0] + 0.35,
                                                      initial_restriction_triggers[0] + 0.6};
    vector<double> restriction_stage_multipliers_durham = {0.9, 0.8, 0.7, 0.6};
    vector<double> restriction_stage_triggers_durham = {initial_restriction_triggers[1],
                                                        initial_restriction_triggers[1] + 0.15,
                                                        initial_restriction_triggers[1] + 0.35,
                                                        initial_restriction_triggers[1] + 0.6};
    vector<double> restriction_stage_multipliers_owasa = {0.9, 0.8, 0.7};
    vector<double> restriction_stage_triggers_owasa = {initial_restriction_triggers[2],
                                                       initial_restriction_triggers[2] + 0.15,
                                                       initial_restriction_triggers[2] + 0.35};
    vector<double> restriction_stage_multipliers_raleigh = {0.9, 0.8, 0.7, 0.6};
    vector<double> restriction_stage_triggers_raleigh = {initial_restriction_triggers[3],
                                                         initial_restriction_triggers[3] + 0.15,
                                                         initial_restriction_triggers[3] + 0.35,
                                                         initial_restriction_triggers[3] + 0.6};

    Restrictions restrictions_c(2,
                                restriction_stage_multipliers_cary,
                                restriction_stage_triggers_cary);
    Restrictions restrictions_d(1,
                                restriction_stage_multipliers_durham,
                                restriction_stage_triggers_durham);
    Restrictions restrictions_o(0,
                                restriction_stage_multipliers_owasa,
                                restriction_stage_triggers_owasa,
                                &owasaDemandClassesFractions,
                                &owasaUserClassesWaterPrices,
                                &owasaPriceSurcharges);
    Restrictions restrictions_r(3,
                                restriction_stage_multipliers_raleigh,
                                restriction_stage_triggers_raleigh);

    drought_mitigation_policies = {&restrictions_c, &restrictions_d,
                                   &restrictions_o, &restrictions_r};

    /// Transfer policy
    /*
     *      2
     *     / \
     *  0 v   v 1
     *   /     \
     *  3---><--1--><--0
     *      2       3
     */

    vector<int> buyers_ids = {0, 1, 3};
    //FIXME: CHECK IF TRANSFER CAPACITIES MATCH IDS IN BUYERS_IDS.
    vector<double> buyers_transfers_capacities = {10.8 * 7, 10.0 * 7, 11.5 * 7,
                                                  7.0 * 7};
    vector<double> buyers_transfers_trigger = {owasa_transfer_trigger,
                                               durham_transfer_trigger,
                                               raleigh_transfer_trigger};

    Graph ug(4);
    ug.addEdge(2,
               1);
    ug.addEdge(2,
               3);
    ug.addEdge(1, 3);
    ug.addEdge(1,
               0);

    Transfers t(4,
                2,
                6,
                35,
                buyers_ids,
                buyers_transfers_capacities,
                buyers_transfers_trigger,
                ug,
                vector<double>(),
                vector<int>());
    drought_mitigation_policies.push_back(&t);


    double insurance_triggers[4] = {owasa_insurance_use,
                                    durham_insurance_use, cary_insurance_use,
                                    raleigh_insurance_use}; //FIXME: Change per solution
    double fixed_payouts[4] = {owasa_insurance_payment,
                               durham_insurance_payment,
                               cary_insurance_payment,
                               raleigh_insurance_payment};
    vector<int> insured_utilities = {0, 1, 2, 3};
    InsuranceStorageToROF in(5,
                             water_sources,
                             g,
                             reservoir_utility_connectivity_matrix,
                             utilities, min_env_flow_controls,
                             insurance_triggers, 1.2, fixed_payouts);

    drought_mitigation_policies.push_back(&in);

    /// Data collector pointer
    MasterDataCollector *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources,
                 g,
                 reservoir_utility_connectivity_matrix,
                 utilities,
                 drought_mitigation_policies,
                 min_env_flow_controls,
                 n_weeks,
                 max_lines); //2385
    cout << "Beginning simulation." << endl;
//    s.runFullSimulation(n_threads);
    data_collector = s.runFullSimulation(n_threads);
    cout << "Ending simulation" << endl;

    /// Calculate objective values.
    data_collector->setOutputDirectory(output_directory);

    /// Print output files.
    string fu = "/TestFiles/output/Utilities";
    string fws = "/TestFiles/output/WaterSources";
    string fp = "/TestFiles/output/Policies";
    string fo = "/TestFiles/output/Objectives";
    string fpw = "/TestFiles/output/Pathways";

    data_collector->printUtilitiesOutputCompact(0,
                                                n_weeks,
                                                fu + "_s"
                                                + std::to_string(sol_number));
    data_collector->printWaterSourcesOutputCompact(0,
                                                   n_weeks,
                                                   fws + "_s"
                                                   + std::to_string(sol_number));
    data_collector->printPoliciesOutputCompact(0,
                                               n_weeks,
                                               fp + "_s"
                                               + std::to_string(sol_number));
    data_collector->printUtilitesOutputTabular(0,
                                               n_weeks,
                                               fu + "_s"
                                               + std::to_string(sol_number)
                                               + "_Tabular.out");
    data_collector->printWaterSourcesOutputTabular(0,
                                                   n_weeks,
                                                   fws + "_s"
                                                   + std::to_string(sol_number)
                                                   + "_Tabular.out");
    data_collector->printPoliciesOutputTabular(0,
                                               n_weeks,
                                               fp + "_s"
                                               + std::to_string(sol_number)
                                               + "_Tabular.out");
    data_collector->printObjectives(fo + "_s" + std::to_string(sol_number));
    data_collector->printPathways(fpw + "_s" + std::to_string(sol_number));
}

int main(int argc, char *argv[]) {

//    double x_real[57] = {0.963126, //Durham restriction trigger
//                         0.001, //OWASA restriction trigger
//                         0.0165026, //raleigh restriction trigger
//                         0.0158446, //cary restriction trigger
//                         0.00203824, //durham transfer trigger
//                         0.991772, //owasa transfer trigger
//                         0.0759845, //raleigh transfer trigger
//                         0.109251, //OWASA JLA
//                         0.0456465, //Raleigh JLA
//                         0.0503415, //Durham JLA
//                         0.527226, //Cary JLA
//                         0.0768186, //durham annual payment
//                         0.00101558, //owasa annual payment
//                         0.0209892, //raleigh annual payment
//                         0.0997788, //cary annual payment
//                         0.422693, //durham insurance use
//                         0.981162, //owasa insurance use
//                         0.687073, //raleigh insurance use
//                         0.953765, //cary insurance use
//                         0.00151499, //durham insurance payment
//                         0.0189007, //owasa insurance payment
//                         0.0166652, //raleigh insurance payment
//                         0.017396, //cary insurance payment
//                         0.001, //durham inftrigger
//                         0.893212, //owasa inftrigger
//                         0.0114582, //raleigh inftrigger
//                         0.671276, //cary inftrigger
//                         0.61143, //university lake expansion ranking
//                         0.493596, //Cane creek expansion ranking
//                         0.72242, //Quarry (stone?) reservoir expansion (Shallow) ranking
//                         0.874358, //Quarry (stone?) reservoir expansion (deep) ranking
//                         0.938602, //Teer quarry expansion ranking
//                         0.995383, //reclaimed water ranking (low)
//                         0.0152792, //reclaimed water (high)
//                         0.235675, //lake michie expansion ranking (low)
//                         0.993166, //lake michie expansion ranking (high)
//                         0.960062, //little river reservoir ranking
//                         0.467553, //richland creek quarry rank
//                         0.000729602, //neuse river intake rank
//                         1, //reallocate falls lake rank
//                         0.405893, //western wake treatment plant rank OWASA low
//                         0.942959, //western wake treatment plant rank OWASA high
//                         0.00085049, //western wake treatment plant rank durham low
//                         0.0321197, //western wake treatment plant rank durham high
//                         0.000928368, //western wake treatment plant rank raleigh low
//                         0.999997, //western wake treatment plant rank raleigh high
//                         64.0285, //caryupgrades 1
//                         2.0556, //caryugrades 2
//                         5.84962, //caryugrades 3
//                         0.0212207, //western wake treatment plant owasa frac
//                         0.324296, //western wake treatment frac durham
//                         0.169109, //western wake treatment plant raleigh frac
//                         6198.43, //falls lake reallocation
//                         19.7202, //durham inf buffer
//                         19.2861, //owasa inf buffer
//                         17.6756, //raleigh inf buffer
//                         19.7285, //cary inf buffer
//    };
//
//    triangleTest(atoi(argv[1]),
//                 x_real,
//                 atoi(argv[2]),
//                 atoi(argv[3]),
//                 0);


    vector<vector<double>> solutions = {{0.235683, 0.957114,  0.0012718, 0.00525047, 0.116066,   0.612199,  0.121213,  0.108594, 0.00179518,  0.0544075, 0.502307, 0.0987114,  0.0980314,   0.0358003,   0.0125665,  0.786909,  0.386127,   0.997554,  0.222908,   0.00205243,  0.0197651,   0.0104979,   0.00773583,  0.653973,  0.574098,   0.0323844, 0.413747,  0.438334,   0.618137,   0.22823,   0.56228,   0.684427,   0.55896,   0.156023,   0.0390492,  0.304467,  0.770643,  0.470128,  0.595877,    0.0253206,  0.595334,    0.308886,  0.898796,   0.0417517, 0.304528,    0.00410753, 38.4899, 1.38236,    35.8909, 0.0041206,  0.948228,  0.440818,  5254.81, 1.48724,  13.7251,  0.987849,  11.628,  0.061, 0.1099,     0,         0.10684,    0.994, 5281.4,  0.002, 0.106304,  0,         0.107339,   0.001, 0.864829, 0.006, 3.96849,     0,           0.0370268,   1,     116.92,  0, 1.42662,    0, 0.0125665,  0, 0, 0.002, 0.128465, 0.000558253, 132.945, 43.9894},
//                                        {0.963126, 0.001,     0.0165026, 0.0158446,  0.00203824, 0.991772,  0.0759845, 0.109251, 0.0456465,   0.0503415, 0.527226, 0.0768186,  0.00101558,  0.0209892,   0.0997788,  0.422693,  0.981162,   0.687073,  0.953765,   0.00151499,  0.0189007,   0.0166652,   0.017396,    0.001,     0.893212,   0.0114582, 0.671276,  0.61143,    0.493596,   0.72242,   0.874358,  0.938602,   0.995383,  0.0152792,  0.235675,   0.993166,  0.960062,  0.467553,  0.000729602, 1,          0.405893,    0.942959,  0.00085049, 0.0321197, 0.000928368, 0.999997,   64.0285, 2.0556,     5.84962, 0.0212207,  0.324296,  0.169109,  6198.43, 19.7202,  19.2861,  17.6756,   19.7285, 0.058, 3.92464,    0,         0.0873155,  0.003, 2395.11, 0,     9.69898,   0.0280344, 0.00170735, 0.016, 362.21,   0.001, 8.0009,      0,           0.0321086,   1,     1333.33, 0, 0.0997788,  0, 0.0997788,  0, 0, 0.002, 121.642,  127.662,     310.513, 0},
//                                        {0.227722, 0.575275,  0.109927,  0.00139361, 0.001,      0.844748,  0.337973,  0.112489, 0.128759,    0.0520237, 0.494136, 0.0972585,  0.0260743,   0.0982021,   0.00234227, 0.922387,  0.842419,   0.96624,   0.00395969, 0.00303341,  0.000479263, 0.000383809, 0.00352352,  0.940809,  0.0372846,  0.0336397, 0.818991,  0.0893652,  0.528996,   0.0176774, 0.0393964, 0.00892876, 0.0549363, 0.725628,   0.0875306,  1,         0,         0.0291458, 0.993429,    0,          0.00769271,  0.313794,  1,          0.301423,  0.997181,    0.995054,   20.2422, 9.2664,     0,       0.23463,    0.238761,  0.766322,  4271.45, 8.2961,   11.7053,  15.2889,   19.1739, 0,     1.28399,    0.132702,  0.106724,   0.744, 5473.35, 0,     3.31699,   0.0827393, 0.0260758,  0,     1508.95,  0.006, 6.42546,     0,           0.0983278,   0.03,  3924.98, 0, 2.05167,    0, 0.00234227, 0, 0, 0.002, 38.879,   38.2375,     266.921, 91.4507},
//                                        {0.963359, 0.926596,  0.0502836, 0.983699,   0.00194041, 0.991772,  0.187029,  0.109819, 0.0456465,   0.0652111, 0.527094, 0.0768186,  0.0510191,   0.0134981,   0.00258657, 0.447327,  0.981162,   0.85083,   0.577307,   0.00153725,  0.00350112,  0.0162274,   0.017396,    0.001,     0.958824,   0.0295699, 0.670653,  0.610675,   0.155632,   0.720563,  0.819904,  0.0674213,  0.513945,  0.51064,    0.251839,   0.993166,  0.964767,  0.464513,  0.258309,    1,          0.362925,    0.952302,  0.6032,     0.0311266, 0.406073,    0.982373,   64.0285, 12.6114,    98.4587, 0.0212207,  0.324296,  0.169109,  8024.87, 4.66392,  16.6369,  17.5996,   9.66627, 0.039, 3.87965,    0,         0.0785421,  0.002, 2827.49, 0.002, 9.74898,   0,         0.0511078,  0.001, 362.21,   0.001, 7.38101,     0.0136176,   0.0238132,   0.049, 1432.62, 0, 0.00258657, 0, 0.00258657, 0, 0, 0.002, 117.224,  127.662,     253.907, 0},
//                                        {0.987259, 0.146724,  0.867494,  0.0954814,  0.225695,   0.965923,  0.9897,    0.100053, 0.000560031, 0.0501186, 0.397694, 0.00382846, 0.0978767,   0,           0.0956413,  0.998188,  0.0160301,  0.0682279, 0.0251313,  0.0190543,   0.014153,    0.0195638,   0.00014023,  0.0427691, 0.819338,   0.001,     0.941356,  0.00131233, 0.981421,   0.227166,  0.437991,  0.0778532,  0.0421505, 0.659074,   0.420817,   0.999324,  0.804981,  0.999516,  0,           0.999133,   0.218419,    0.699086,  0.980095,   0.985773,  0.964133,    0.862574,   99.6931, 9.67493,    3.91126, 0,          0.0416194, 0.42699,   9686.94, 11.5998,  4.29231,  0.867289,  17.7223, 0.199, 2.60242,    0.0178437, 0.0312227,  0.022, 780.147, 0.002, 9.79509,   0,         0.117145,   0.003, 0,        0.018, 5.32428,     0.000484661, 0.172759,    0.005, 24.2108, 0, 0.0956413,  0, 0.0956413,  0, 0, 0.221, 75.0457,  124.032,     211.47,  0},
//                                        {0.956332, 0.0281718, 0.171866,  0.164298,   0.0947485,  0.394821,  0.358088,  0.1413,   0.0119134,   0.0573915, 0.439011, 0.035531,   0.0281041,   0.0137373,   0.00131697, 0.664868,  0.679696,   0.72101,   0.142764,   0.00126505,  0.0198763,   0.00751601,  0.00667631,  0.946846,  0.332245,   0.173587,  0.333471,  0.342432,   0.210152,   0.282641,  0.343086,  0.596842,   0.845471,  0.114764,   0.150276,   0.297834,  0.0892597, 0.0142597, 0.389644,    0.974539,   0.253545,    0.406721,  0.336399,   0.337633,  0.981207,    0.779364,   33.7658, 17.9897,    29.0387, 0.0592621,  0.521111,  0.237271,  6085.91, 5.38569,  1.80288,  12.8317,   13.7247, 0.044, 2.23091,    0.0988624, 0.0386905,  0.002, 5117.03, 0,     0.738095,  0,         0.028158,   0.002, 636.657,  0.031, 1.4213,      0,           0.0276184,   0.112, 717.531, 0, 1.59509,    0, 0.00131697, 0, 0, 0.012, 44.2267,  5.02958,     37.9394, 58.95},
//                                        {0.213242, 0.957456,  0.001,     0.014346,   0.0308731,  1,         0.001,     0.1,      0.0286538,   0.050506,  0.355,    0.0984041,  0.0553887,   0.045043,    0.0641898,  0.796475,  0.82814,    0.936268,  0.0107759,  0.00049036,  0.000380244, 0.00410129,  0.000618023, 0.485685,  0.536184,   0.155526,  0.434502,  0.0181794,  0.959672,   0.425474,  0.179502,  0.833427,   0.607688,  0.0415346,  0.998072,   1,         0.776926,  0.640959,  0.936398,    0,          0.964533,    0.0455533, 0.0147245,  0.413175,  0.815876,    0,          46.756,  0.00705836, 34.3618, 0.258988,   0.464695,  0.961345,  1307.21, 8.49927,  0.469376, 10.3101,   4.4794,  0.061, 0.288175,   0,         0.101007,   0.987, 4714.15, 0.001, 1.41629,   0,         0.0553901,  0.001, 291.979,  0.009, 2.88216,     0,           0.0499763,   1,     1553.7,  0, 0.902129,   0, 0.0641898,  0, 0, 0.002, 6.73422,  13.5605,     86.701,  24.4949},
//                                        {0.637165, 0.391442,  0.216902,  0.0141022,  0.218586,   0.139415,  0.773235,  0.111371, 0.0705888,   0.0793894, 0.698873, 0.0448003,  0.0557323,   0.0618128,   0.0828703,  0.798672,  0.340406,   0.776776,  0.0754045,  0.000691945, 0.0148453,   0.00578075,  0.000303185, 0.968614,  0.853621,   0.229876,  0.0250273, 0.463147,   0.629967,   0.416301,  0.974619,  0.713866,   0.793756,  0.98866,    0.4711,     0.991528,  0.718284,  0.335585,  0.187557,    0.465011,   0.624512,    0.975401,  0.196236,   0.759695,  0.977654,    0.971889,   39.1866, 41.958,     43.6919, 0.011297,   0.14344,   0.0427252, 6289.62, 19.4131,  11.3672,  18.994,    13.9727, 0.097, 0.546183,   0,         0.0505955,  0.119, 4127.2,  0,     0.168194,  0,         0.0599814,  0.001, 143.479,  0.064, 2.65019,     0,           0.0769483,   0.189, 571.384, 0, 1.36103,    0, 0.0828703,  0, 0, 0.002, 10.8591,  0.855233,    71.3198, 39.7319},
//                                        {0.204617, 0.782593,  0.143398,  0.812864,   0.00130046, 0.147777,  0.3555,    0.100847, 0.0751492,   0.0586454, 0.545935, 0.00190498, 0.0194359,   0.00444258,  0.0604289,  0.901991,  0.361042,   0.468268,  0.120629,   0.0102173,   0.0135123,   0.0155166,   0.0096144,   0.629715,  0.803701,   0.01404,   0.978855,  0.261139,   0.700673,   0.215063,  0.644297,  0.00576218, 0.999998,  0.964935,   0.448316,   0.345653,  0.988104,  0.979561,  0.144019,    0,          4.59E-05,    0.862553,  1,          0.834257,  0.686418,    0.858814,   34.5707, 45.0132,    24.258,  0.0676262,  0.327742,  0.482743,  7087.42, 17.4624,  9.40489,  13.944,    6.77655, 0.019, 1.64891,    0.218459,  0.0511343,  0.353, 5168.34, 0,     0.966823,  0.0401832, 0.0202145,  0,     676.688,  0.006, 7.66888,     0.0479632,   0.0140475,   0.012, 2540.53, 0, 1.25948,    0, 0.0604289,  0, 0, 0.002, 52.4388,  10.8247,     304.593, 46.7289},
//                                        {1,        0.0975529, 0.994004,  1,          0.219184,   0.001,     0.978949,  0.100374, 1.68E-05,    0.085291,  0.386652, 0.00127242, 0.0108928,   0.000470029, 1.47E-05,   0.568639,  1,          0.999775,  0.0125505,  7.63E-05,    0.02,        2.44E-05,    0,           1,         1,          0.754007,  0.001,     0.337299,   0.392341,   0.583173,  0.993303,  0.185567,   0.999212,  0.107216,   0.650935,   0.995951,  0,         0.140965,  0.00422652,  0.326903,   0.000210541, 0.622021,  0.478786,   0.738667,  0.259441,    0.527232,   48.3636, 1.65466,    66.7519, 0.854622,   0.907369,  0.773318,  874.703, 0.264668, 1.20404,  14.1261,   19.4923, 0.284, 0.00168624, 0.171303,  0.00177216, 0.075, 3173.19, 0,     0.0109011, 0,         0.0109014,  0.001, 15.1984,  0.208, 0.000534143, 0,           0.000544364, 0.085, 29.7345, 0, 0.50699,    0, 1.47E-05,   0, 0, 0.002, 0,        0,           0,       16.222},
//                                        {0.478091, 0.457106,  0.253359,  0.12932,    0.00181276, 0.624704,  0.0917486, 0.1,      0.0354824,   0.0560185, 0.505234, 0.016382,   0.0109499,   0.0406246,   0.00738595, 0.990024,  0.822043,   0.80878,   0.378137,   0.0183669,   0.000725868, 0.013581,    0.00546901,  0.990486,  0.48098,    0.0178758, 0.336346,  0.0737334,  0.683975,   0.836078,  0.83933,   0.349991,   0.945626,  0.0258009,  0.0624992,  0.85351,   0.468034,  0.0648724, 0.318511,    0.0189154,  0.88124,     0.790452,  0.606059,   0.925709,  0.97274,     0.92459,    33.1866, 26.8981,    47.3028, 0.400912,   0.206258,  0.15786,   1528.2,  6.67242,  7.72843,  0.418608,  7.11962, 0.039, 0.956043,   0.16434,   0.0346776,  0.075, 5370.7,  0,     5.14842,   0.092142,  0.0109511,  0,     1458.68,  0.005, 4.98383,     0,           0.0413785,   0.008, 1304.28, 0, 1.29976,    0, 0.00738595, 0, 0, 0.002, 26.021,   50.5782,     172.793, 52.4695},
//                                        {0.361534, 0.031173,  0.131796,  0.0259032,  0.0150257,  0.300164,  0.633682,  0.132403, 0.159287,    0.0785021, 0.450229, 0.0692351,  0.0618414,   0.0426829,   0.0107992,  0.23084,   0.654709,   0.979665,  0.586328,   0.010156,    0.00896616,  0.00588052,  0.0130867,   0.534346,  0.755635,   0.0282451, 0.552062,  0.0318356,  0.00664154, 0.74869,   0.640439,  0.395368,   0.942629,  0.767446,   0.676389,   0.125787,  0.599715,  0.837438,  0.993364,    0.0349039,  0.48468,     0.361503,  0.288433,   0.909647,  0.153435,    0.155666,   33.6754, 27.3316,    41.9432, 0.161393,   0.0515612, 0.42644,   4481.17, 10.4348,  19.8721,  2.08513,   2.86297, 0.034, 0.57127,    0.110428,  0.38088,    0.641, 6163.27, 0,     0.718126,  0,         0.0661518,  0.008, 492.16,   0.021, 3.98427,     0,           0.0438567,   0.144, 1181.63, 0, 1.37686,    0, 0.0107992,  0, 0, 0.002, 6.53311,  6.92599,     142.833, 55.2348},
//                                        {0.246816, 0.159641,  0.0274182, 0.0519887,  0.0487342,  0.0236299, 0.539164,  0.102159, 0.000552833, 0.0541339, 0.484821, 0.00998436, 0.0573459,   0.0719507,   0.0224662,  0.986971,  0.00865723, 0.996347,  0.786469,   0,           0.0198913,   0.0156322,   0.000144894, 0.328474,  0.00537971, 0.001,     0.392501,  0.872269,   0.0650932,  0.562198,  0.94501,   0.573858,   0.99377,   0.823308,   0.00460325, 0.0745423, 0.960852,  0.056454,  0.918275,    0.00763002, 0.228613,    0.924567,  0.96297,    0.960988,  0.681048,    0.00432544, 26.4928, 7.1628,     47.9383, 0.00103193, 0.639974,  0.0257815, 7394.28, 5.78545,  2.20224,  0.0274661, 17.8088, 0.002, 1.71132,    0.23521,   0.00998436, 0.631, 5160.4,  0,     0.0748574, 0,         0.0730965,  0.001, 20.7384,  0.003, 5.25781,     0,           0.0721447,   0.445, 20.987,  0, 1.64365,    0, 0.0224662,  0, 0, 0.004, 37.6443,  0.0687359,   201.447, 74.6601},
//                                        {0.313605, 0.963891,  0.986368,  0.323881,   0.186088,   0.191571,  0.515854,  0.101364, 0.0129181,   0.0510563, 0.472494, 0.0471882,  0.000845308, 0.0124923,   0.0216897,  0.974904,  0.565303,   0.915758,  0.198576,   0.00130016,  0.0070447,   0.00127564,  0.000257897, 0.784223,  0.664544,   0.0941687, 0.422979,  0.78138,    0.967996,   0.83967,   0.317715,  0.472037,   0.326302,  0.653722,   0.989218,   0.0197417, 0.0392785, 0.0805909, 0.57369,     0.0598481,  0.130062,    0.713929,  0.620679,   0.434099,  0.641253,    0.538731,   40.1818, 46.7622,    2.08783, 0.0163495,  0.18079,   0.122235,  9163.68, 18.0018,  8.72855,  15.124,    16.3406, 0.042, 0.909072,   0,         0.0524857,  0.336, 3684.45, 0,     0.219822,  0.0232191, 0.00134813, 0,     267.746,  0.046, 3.41878,     0,           0.0134909,   0.002, 659.126, 0, 1.10237,    0, 0.0216897,  0, 0, 0.059, 24.7549,  2.23868,     130.761, 34.5344},
//                                        {0.964084, 0.957237,  0.3402,    0.508516,   0.0886362,  0.271732,  0.380416,  0.116606, 0.0720546,   0.0511521, 0.521052, 0.0583203,  0.00112482,  0.00707201,  0.0993984,  0.845939,  0.843715,   0.405772,  0.692136,   0.0102958,   0.00623187,  0.0164233,   0.0196851,   0.974996,  0.660625,   0.0518138, 0.445297,  0.596673,   0.509064,   0.469549,  0.653757,  0.00433695, 0.822447,  0.61638,    0.355138,   0.845227,  0.236853,  0.641729,  0.755669,    0,          0.529056,    0.529334,  0.651159,   0.506703,  0.813917,    0.804946,   36.1739, 3.7736,     43.326,  0.0621908,  0.168455,  0.198017,  9098.5,  11.8163,  15.9788,  8.2912,    15.968,  0.061, 0.850997,   0,         0.102845,   0.002, 4806.45, 0,     0.808397,  0.0865315, 0.00138925, 0,     1122.87,  0.022, 3.87098,     0.0433573,   0.0305339,   0.043, 2649.63, 0, 1.40953,    0, 0.0993984,  0, 0, 0.002, 21.1709,  7.81594,     136.9,   48.05},
//                                        {0.94907,  0.987128,  0.32142,   0.164361,   0.0631331,  0.517248,  0.315407,  0.257379, 0.0918029,   0.0509198, 0.521006, 0.0178659,  0.0374205,   0.0272608,   0.0714269,  0.0582526, 0.97904,    0.992166,  0.404572,   5.34E-05,    0.0138762,   0.00136882,  0.00351665,  0.476592,  0.397933,   0.358438,  0.220057,  0.433845,   0.0213737,  0.0369478, 0.702728,  0.862642,   0.832649,  0.00531276, 0.6222,     0.748522,  0.774214,  0.119259,  0.96785,     0.969568,   0.434788,    0.943323,  0.988651,   0.966413,  0.301413,    0.127815,   67.1893, 75.2889,    19.6886, 0.0176694,  0.367224,  0.219651,  133.503, 11.3229,  7.00285,  6.66865,   8.5734,  0.217, 0.709567,   0.0301964, 0.0198414,  0.216, 2225.69, 0.002, 0.14981,   0,         0.0375377,  0,     168.256,  0.132, 0.346115,    0,           0.0291458,   0.371, 2073.83, 0, 0.0714269,  0, 0.0714269,  0, 0, 0.002, 15.1114,  0.962563,    9.01851, 0}
    };

    int first_sol = (argc < 6 ? 0 : atoi(argv[4]));
    int last_sol = (argc < 6 ? (int) solutions.size() : atoi(argv[5]));

    cout << "Solutions " << first_sol << " to " << last_sol << endl;

    for (int i = first_sol; i < last_sol; ++i) {
        cout << endl << endl << endl << "Running solution " << i << endl;
        triangleTest(atoi(argv[1]), solutions[i].data(), atoi(argv[2]),
                     atoi(argv[3]), i, (argc < 6 ? argv[4] : argv[6]));
    }

    return 0;
}
