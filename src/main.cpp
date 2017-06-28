#include <iostream>
#include <algorithm>
#include "SystemComponents/WaterSources/Reservoir.h"
#include "SystemComponents/Utility.h"
#include "Utils/Utils.h"
#include "Simulation/Simulation.h"
#include "Utils/QPSolver/QuadProg++.hh"
#include "DroughtMitigationInstruments/Transfers.h"
#include "SystemComponents/WaterSources/ReservoirExpansion.h"
#include "SystemComponents/WaterSources/Quarry.h"
#include "DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "SystemComponents/WaterSources/WaterReuse.h"


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

    DataCollector *data_collector = nullptr;

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

    DataCollector *data_collector = nullptr;

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

    DataCollector *data_collector = nullptr;

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

    DataCollector *data_collector = nullptr;

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

    Quarry q1("Q1", 0, 3.0, catchments1, 100.0, 20, 10);
    Reservoir r2("R2", 1, 3.0, catchments2, 275.0, 20, nullptr, nullptr,
                 0.02, construction_time_interval_r2, 5000, 20, 0.05);
    Reservoir r3("R3", 2, 2.0, catchments3, 400.0, 20, nullptr, nullptr);
    Reservoir r4("R4", 3, 3.0, catchments2, 500.0, 20, nullptr, nullptr);
    Reservoir r5("R5", 4, 2.0, catchments3, 850.0, 20, nullptr, nullptr);
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



    vector<int> wwtp_return_u1 = {3};
    vector<int> wwtp_return_u2 = {3, 2};
    vector<int> wwtp_return_u3 = {};
    vector<vector<double>> *empty_return_series = new vector<vector<double>>();

    WwtpDischargeRule wwtp_u1(&demand_to_wastewater_fraction_owasa_raleigh, &wwtp_return_u1);
    WwtpDischargeRule wwtp_u2(&demand_to_wastewater_fraction_durham, &wwtp_return_u2);
    WwtpDischargeRule wwtp_u3(empty_return_series, &wwtp_return_u3);
    /// Create catchments and corresponding vector
    Utility u1((char *) "U1", 0, &demands_test, streamflow_n_weeks, 0.03, 1, wwtp_u1);
    Utility u2((char *) "U2", 1, &demands_test, streamflow_n_weeks, 0.07, 1, vector<int>(1, 1), 0.04, wwtp_u2);
    Utility u3((char *) "U3", 2, &demands_test, streamflow_n_weeks, 0.05, 1, vector<int>(1, 5), 0.04, wwtp_u3);

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

//    vector<double> insurance_triggers = {0.06, 0.08, 0.06};
//    vector<double> fixed_payouts = {1., 1., 1.};
//    InsurancePseudoROF in(0, insurance_triggers, 1.2, water_sources, utilities, g,
//                          reservoir_utility_connectivity_matrix,
//                          fixed_payouts);
//    drought_mitigation_policies.push_back(&in);

    double insurance_triggers[3] = {0.06, 0.05, 0.05};
    double fixed_payouts[3] = {1., 1., 1.};
    vector<int> insured_utilities = {0, 1, 2};
    InsuranceStorageToROF in(0, water_sources, g, reservoir_utility_connectivity_matrix, utilities, insurance_triggers,
                             1.2, fixed_payouts);
    drought_mitigation_policies.push_back(&in);

    /// Data collector pointer
    DataCollector *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, drought_mitigation_policies,
                 992, 2, data_collector);
    cout << "Beginning U3R5 simulation" << endl;
    s.runFullSimulation(2);
    cout << "Ending U3R5 simulation" << endl;
}

void triangleTest() {
    srand((unsigned int) time(NULL));

    cout << "BEGINNING TRIANGLE TEST" << endl << endl;

    /// Read streamflows
    int streamflow_n_weeks = 52 * (70 + 50);
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

    EvaporationSeries evaporation_durham(&evap_durham, streamflow_n_weeks);
    EvaporationSeries evaporation_jordan_lake(&evap_jordan_lake, streamflow_n_weeks);
    EvaporationSeries evaporation_falls_lake(&evap_falls_lake, streamflow_n_weeks);
    EvaporationSeries evaporation_owasa(&evap_owasa, streamflow_n_weeks);
    EvaporationSeries evaporation_little_river(&evap_little_river, streamflow_n_weeks);
    EvaporationSeries evaporation_wheeler_benson(&evap_wheeler_benson, streamflow_n_weeks);

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
    Catchment cape_fear_river_at_lillington(&streamflows_lillington, streamflow_n_weeks);

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

    vector<int> cary_ws_return_id = {};
    vector<vector<double>> *cary_discharge_fraction_series = new vector<vector<double>>();
    WwtpDischargeRule wwtp_discharge_cary(cary_discharge_fraction_series, &cary_ws_return_id);
    vector<int> owasa_ws_return_id = {6};
    WwtpDischargeRule wwtp_discharge_owasa(&demand_to_wastewater_fraction_owasa_raleigh, &owasa_ws_return_id);
    vector<int> raleigh_ws_return_id = {8};
    WwtpDischargeRule wwtp_discharge_raleigh(&demand_to_wastewater_fraction_owasa_raleigh, &raleigh_ws_return_id);
    vector<int> durham_ws_return_id = {1, 6}; //FIXME: CHECK IF THIS IS RIGHT.
    WwtpDischargeRule wwtp_discharge_durham(&demand_to_wastewater_fraction_durham, &durham_ws_return_id);

    /// Storage vs. area reservoir curves.
    vector<double> falls_lake_storage = {-25734, 3266, 14700};
    vector<double> falls_lake_area = {0, 0.32 * 29000, 0.28 * 40434};
    vector<double> wheeler_benson_storage = {0, 2789.66};
    vector<double> wheeler_benson_area = {0, 0.3675 * 2789.66};
    vector<double> teer_storage = {0, 1315.0};
    vector<double> teer_area = {20, 50};
    vector<double> little_river_res_storage = {0, 3700};
    vector<double> little_river_res_area = {0, 0.3675 * 3700};

    DataSeries falls_lake_storage_area(&falls_lake_storage, &falls_lake_area);
    DataSeries wheeler_benson_storage_area(&wheeler_benson_storage, &wheeler_benson_area);
    DataSeries little_river_storage_area(&little_river_res_storage, &little_river_res_area);

    /// Create reservoirs and corresponding vector
    vector<double> construction_time_interval = {3.0, 5.0};
//    vector<double> construction_time_interval = {300.0, 300.001};
    //Cary  Dur  OWA   Ral
    vector<double> JL_allocation_fractions = {0.3, 0.1, 0.05, 0.0};
    double JL_supply_capacity = 14924.0;
    vector<double> city_infrastructure_rof_triggers = {0.02, 0.02, 0.02, 0.02};
    vector<double> bond_length = {25, 25, 25, 25};
    vector<double> bond_rate = {0.04, 0.04, 0.04, 0.04};

    // Existing Sources
    Reservoir durham_reservoirs("Lake Michie & Little River Res. (Durham)", 0, 9.05, catchment_durham, 6349.0,
                                99999, &evaporation_durham, 1069);
    Reservoir falls_lake("Falls Lake", 1, 38.78 * 7, catchment_flat, 14700.0, 99999, &evaporation_falls_lake,
                         &falls_lake_storage_area);
    Reservoir wheeler_benson_lakes("Wheeler-Benson Lakes", 2, 2.0 * 7, catchment_swift, 2789.66, 99999,
                                   &evaporation_wheeler_benson, &wheeler_benson_storage_area);
    Reservoir stone_quarry("Stone Quarry", 3, 9.05 * 7, catchment_phils, 200.0, 99999, &evaporation_owasa, 10);
    Reservoir ccr("Cane Creek Reservoir", 4, 0.1422 * 7, catchment_cane, 2909.0, 99999, &evaporation_owasa, 500);
    Reservoir university_lake("University Lake", 5, 0, catchment_morgan, 449.0, 99999, &evaporation_owasa, 212);
    Reservoir jordan_lake("Jordan Lake", 6, 25.8527 * 7, catchment_haw, JL_supply_capacity * JL_allocation_fractions[0],
                          448, &evaporation_jordan_lake, 13940);

    // other than Cary WTP for Jordan Lake, assume no WTP constraints - each city can meet its daily demands
    // with available treatment infrastructure

    // Potential Sources
    // The capacities listed here for expansions are what additional capacity is gained relative to existing capacity,
    // NOT the total capacity after the expansion is complete. For infrastructure with a high and low option, this means
    // the capacity for both is relative to current conditions - if Lake Michie is expanded small it will gain 2.5BG,
    // and a high expansion will provide 7.7BG more water than current. if small expansion happens, followed by a large
    // expansion, the maximum capacity through expansion is 7.7BG, NOT 2.5BG + 7.7BG.
    Reservoir little_river_reservoir("Little River Reservoir (Raleigh)", 7, 9.05, catchment_little_river_raleigh,
                                     3700.0, 99999, &evaporation_little_river, &little_river_storage_area,
                                     city_infrastructure_rof_triggers[3], construction_time_interval, 263.0,
                                     bond_length[3], bond_rate[3]);
    Quarry richland_creek_quarry("Richland Creek Quarry", 8, 38.78 * 7, gage_clayton, 4000.0, 99999,
                                 50 * 7, //FIXME: MAX PUMPING CAPACITY?
                                 city_infrastructure_rof_triggers[3], construction_time_interval, 400.0,
                                 bond_length[3], bond_rate[3]);
    // diversions to Richland Creek Quarry based on ability to meet downstream flow target at Clayton, NC
    Quarry teer_quarry("Teer Quarry", 9, 9.05, vector<Catchment *>(), 1315.0, 99999, 15 * 7,
                       city_infrastructure_rof_triggers[1],
                       construction_time_interval, 22.6, bond_length[1], bond_rate[1]); //FIXME: MAX PUMPING CAPACITY?
    // diversions to Teer Quarry for Durham based on inflows to downstream Falls Lake from the Flat River
    // FYI: spillage from Eno River also helps determine Teer quarry diversion, but Eno spillage isn't factored into
    // downstream water balance?
    ReservoirExpansion fl_reallocation("Falls Lake Reallocation", 17, 1, 4100.0, city_infrastructure_rof_triggers[3],
                                       construction_time_interval, 68.2, bond_length[3], bond_rate[3]);
    ReservoirExpansion ccr_expansion("Cane Creek Reservoir Expansion", 26, 4, 3000.0,
                                     city_infrastructure_rof_triggers[2], construction_time_interval, 127.0,
                                     bond_length[2], bond_rate[2]);
    ReservoirExpansion low_sq_expansion("Low Stone Quarry Expansion", 12, 3, 1500.0,
                                        city_infrastructure_rof_triggers[2], construction_time_interval, 1.4,
                                        bond_length[2], bond_rate[2]);
    ReservoirExpansion high_sq_expansion("High Stone Quarry Expansion", 13, 3, 2200.0,
                                         city_infrastructure_rof_triggers[2], construction_time_interval, 64.6,
                                         bond_length[2], bond_rate[2]);
    ReservoirExpansion ul_expansion("University Lake Expansion", 14, 5, 2550.0, city_infrastructure_rof_triggers[2],
                                    construction_time_interval, 107.0, bond_length[2], bond_rate[2]);
    ReservoirExpansion low_lm_expansion("Low Lake Michie Expansion", 15, 0, 2500.0, city_infrastructure_rof_triggers[1],
                                        construction_time_interval, 158.3, bond_length[1], bond_rate[1]);
    ReservoirExpansion high_lm_expansion("High Lake Michie Expansion", 16, 0, 7700.0,
                                         city_infrastructure_rof_triggers[1], construction_time_interval, 203.3,
                                         bond_length[1], bond_rate[1]);

    //Reservoir rNeuseRiverIntake("rNeuseRiverIntake", 10, 0, catchment_flat, 16.0, 99999, city_infrastructure_rof_triggers[3], construction_time_interval, 5000, 20, 0.05);
    Intake neuse_river_intake("Neuse River Intake", 10, 38.78 * 7, catchment_flat, 16 * 7,
                              city_infrastructure_rof_triggers[3],
                              construction_time_interval, 225.5, bond_length[3], bond_rate[3]);
    WaterReuse low_reclaimed("Low Reclaimed Water System", 18, 2.2, city_infrastructure_rof_triggers[1],
                             construction_time_interval, 27.5, bond_length[1], bond_rate[1]);
    WaterReuse high_reclaimed("High Reclaimed Water System", 19, 11.3, city_infrastructure_rof_triggers[1],
                              construction_time_interval, 104.4, bond_length[1], bond_rate[1]);

    //FIXME: ONCE ONE UTILITY DECIDES TO BUILD THE WJLWTP, ALL OTHERS PAY FOR IT TOO, RIGHT? IF SO, WHAT'S THE PROPORTION?
    //FIXME: FIX AREAS.
    Reservoir low_wjlwtp_durham("Low WJLWTP (Durham)", 20, 0, catchment_haw,
                                JL_supply_capacity * JL_allocation_fractions[1], 33.0 * JL_allocation_fractions[1],
                                &evaporation_jordan_lake, 13940,
                                city_infrastructure_rof_triggers[1], construction_time_interval,
                                243.3 * JL_allocation_fractions[1], bond_length[1], bond_rate[1]);
    Reservoir high_wjlwtp_durham("High WJLWTP (Durham)", 21, 0, catchment_haw,
                                 JL_supply_capacity * JL_allocation_fractions[1], 54.0 * JL_allocation_fractions[1],
                                 &evaporation_jordan_lake, 13940,
                                 city_infrastructure_rof_triggers[1], construction_time_interval,
                                 73.5 * JL_allocation_fractions[1], bond_length[1], bond_rate[1]);
    Reservoir low_wjlwtp_owasa("Low WJLWTP (OWASA)", 22, 0, catchment_haw,
                               JL_supply_capacity * JL_allocation_fractions[2], 33.0 * JL_allocation_fractions[2],
                               &evaporation_jordan_lake, 13940,
                               city_infrastructure_rof_triggers[2], construction_time_interval,
                               243.3 * JL_allocation_fractions[2], bond_length[2], bond_rate[2]);
    Reservoir high_wjlwtp_owasa("High WJLWTP (OWASA)", 23, 0, catchment_haw,
                                JL_supply_capacity * JL_allocation_fractions[2], 54.0 * JL_allocation_fractions[2],
                                &evaporation_jordan_lake, 13940,
                                city_infrastructure_rof_triggers[2], construction_time_interval,
                                73.5 * JL_allocation_fractions[2], bond_length[2], bond_rate[2]);
    Reservoir low_wjlwtp_raleigh("Low WJLWTP (Raleigh)", 24, 0, catchment_haw,
                                 JL_supply_capacity * JL_allocation_fractions[3], 33.0 * JL_allocation_fractions[3],
                                 &evaporation_jordan_lake, 13940,
                                 city_infrastructure_rof_triggers[3], construction_time_interval,
                                 243.3 * JL_allocation_fractions[3], bond_length[3], bond_rate[3]);
    Reservoir high_wjlwtp_raleigh("High WJLWTP (Raleigh)", 25, 0, catchment_haw,
                                  JL_supply_capacity * JL_allocation_fractions[4], 54.0 * JL_allocation_fractions[3],
                                  &evaporation_jordan_lake, 13940,
                                  city_infrastructure_rof_triggers[3], construction_time_interval,
                                  73.5 * JL_allocation_fractions[3], bond_length[3], bond_rate[3]);

    Reservoir dummy_endpoint("Dummy Node", 11, 0, vector<Catchment *>(), 0, 0, &evaporation_durham, 1);


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
    water_sources.push_back(&ul_expansion);
    water_sources.push_back(&neuse_river_intake);
    water_sources.push_back(&low_sq_expansion);
    water_sources.push_back(&low_lm_expansion);
    water_sources.push_back(&low_reclaimed);
    water_sources.push_back(&low_wjlwtp_durham);
    water_sources.push_back(&low_wjlwtp_owasa);
    water_sources.push_back(&low_wjlwtp_raleigh);
    water_sources.push_back(&high_sq_expansion);
    water_sources.push_back(&high_lm_expansion);
    water_sources.push_back(&high_reclaimed);
    water_sources.push_back(&high_wjlwtp_durham);
    water_sources.push_back(&high_wjlwtp_owasa);
    water_sources.push_back(&high_wjlwtp_raleigh);

    water_sources.push_back(&dummy_endpoint);

    /*
     * System connection diagram (water
     * flows from top to bottom)
     * Potential projects and expansions
     * of existing sources in parentheses
     *
     *      3(12,13)   4(26)   5(14)            0(15,16)        (18,19)
     *         \         /      /                  |              /
     *          \       /      /                   |             /
     *           \     /      /                    |            /
     *           |    /      /                    (9)          /
     *           |   /      /                      \       ----
     *           |   |     /                        \     /
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

    //FIXME: CHANGE IDS OF SOURCES SO THAT GRAPH NODES ARE THE FIRST WHATEVER MANY IDS.
    Graph g(12);
    g.addEdge(9, 1);
    g.addEdge(0, 1);
    g.addEdge(1, 8);
    g.addEdge(8, 10);
    g.addEdge(10, 11);
    g.addEdge(2, 11);
    g.addEdge(7, 11);

    g.addEdge(3, 6);
    g.addEdge(4, 6);
    g.addEdge(5, 6);
    g.addEdge(6, 11);

    /// Create catchments and corresponding vector
    vector<int> infra_order_durham = {9, 15, 16, 18, 19, 20, 21};
    vector<int> infra_order_owasa = {26, 12, 13, 14, 20, 21};
    vector<int> infra_order_raleigh = {7, 8, 17, 10, 20, 21};
    int demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);


    Utility cary((char *) "Cary", 0, &demand_cary, demand_n_weeks, 0.03, 1,
                 wwtp_discharge_cary);
    Utility durham((char *) "Durham", 1, &demand_durham, demand_n_weeks, 0.07, 1, infra_order_durham, 0.05,
                   wwtp_discharge_durham);
    Utility owasa((char *) "OWASA", 2, &demand_owasa, demand_n_weeks, 0.05, 1, infra_order_owasa, 0.05,
                  wwtp_discharge_owasa);
    Utility raleigh((char *) "Raleigh", 3, &demand_raleigh, demand_n_weeks, 0.05, 1, infra_order_raleigh, 0.05,
                    wwtp_discharge_raleigh);

    vector<Utility *> utilities;
    utilities.push_back(&cary);
    utilities.push_back(&durham);
    utilities.push_back(&owasa);
    utilities.push_back(&raleigh);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {6},
            {0, 9, 15, 16, 18, 19, 20, 21},
            {3, 4, 5,  26, 12, 13, 14, 22, 23, 20, 21},
            {1, 2, 7,  8,  17, 10, 24, 25, 20, 21}
    };

    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    /// Restriction policies
    vector<double> initial_restriction_triggers = {0.04, 0.04, 0.04, 0.04};

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

    Restrictions restrictions_c(0, restriction_stage_multipliers_cary, restriction_stage_triggers_cary);
    Restrictions restrictions_d(1, restriction_stage_multipliers_durham, restriction_stage_triggers_durham);
    Restrictions restrictions_o(2, restriction_stage_multipliers_owasa, restriction_stage_triggers_owasa);
    Restrictions restrictions_r(3, restriction_stage_multipliers_raleigh, restriction_stage_triggers_raleigh);

    drought_mitigation_policies = {&restrictions_c, &restrictions_d, &restrictions_o, &restrictions_r};

    /// Transfer policy
    /*
     *      0
     *     / \
     *  0 v   v 1
     *   /     \
     *  3---><--1--><--2
     *      2       3
     */

    vector<int> buyers_ids = {1, 2, 3};
    vector<double> buyers_transfers_capacities = {10.8 * 7, 10.0 * 7, 11.5 * 7, 7.0 * 7};
//    vector<double> buyers_transfers_capacities = {10.8, 10.0, 11.5, 7.0};
    vector<double> buyers_transfers_trigger = {0.02, 0.02, 0.02};

    Graph ug(4);
    ug.addEdge(0, 1);
    ug.addEdge(0, 3);
    ug.addEdge(1, 3);
    ug.addEdge(1, 2);

    Transfers t(0, 0, 245, buyers_ids, buyers_transfers_capacities, buyers_transfers_trigger, ug,
                vector<double>(), vector<int>());
    drought_mitigation_policies.push_back(&t);

    /// Data collector pointer
    DataCollector *data_collector = nullptr;

    /// Creates simulation object
    Simulation s(water_sources, g, reservoir_utility_connectivity_matrix, utilities, drought_mitigation_policies,
                 2385, 10, data_collector); //2385
    cout << "Beginning Triangle simulation" << endl;
    s.runFullSimulation(4);
    cout << "Ending Triangle simulation" << endl;
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
    triangleTest();
//    U3R5 problem(1);
//    problem.run();

    return 0;
}