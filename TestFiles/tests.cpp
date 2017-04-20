//
// Created by bct52 on 4/19/17.
//


#include "../src/SystemComponents/WaterSources/Reservoir.h"
#include "../src/Utils/Utils.h"
#include "catch-main.h"
#include "../src/Utils/DataCollector.h"

TEST_CASE("Net present cost calculations", "[NPC]") {
    Reservoir r("Res", 0, 1, vector<Catchment *>(), 1, 1, 1, vector<double>(2, 1), 5000, 20, 0.05);

    REQUIRE(r.calculateNetPresentCost(0, 0.05) == Approx(5000));
    REQUIRE(r.calculateNetPresentCost(0, 0.04) == Approx(5452.6147334301));
    REQUIRE(r.calculateNetPresentCost(521, 0.04) == Approx(3685.751));
}

TEST_CASE("Split of demand among water sources of a utility proportionally to their stored volume", "[Split Demand]") {

    /// Read streamflows
    int streamflow_n_weeks = (int) std::round(Constants::WEEKS_IN_YEAR * Constants::NUMBER_REALIZATIONS_ROF);
    double *streamflows_test = new double[streamflow_n_weeks + 1];
    streamflows_test[streamflow_n_weeks] = 5;

    /// Create catchments and corresponding vectors
    Catchment c1(streamflows_test, streamflow_n_weeks);

    vector<Catchment *> catchments1(1, &c1);
    double min_env_flow_intake = 2.0;
    Reservoir r1("R1", 0, NON_INITIALIZED, catchments1, 200.0, NON_INITIALIZED);
    Reservoir r2("R2", 1, NON_INITIALIZED, catchments1, 400.0, NON_INITIALIZED);

    SECTION("Split demand among reservoirs only") {
        Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, NON_INITIALIZED, NON_INITIALIZED);
        u1.addWaterSource(&r1);
        u1.addWaterSource(&r2);
        u1.splitDemands(0);

        REQUIRE(u1.getReservoirDraw(0) == Approx(9.0617079441 * 1 / 3));
    }

    SECTION("Split demand among intakes and reservoirs") {
        Utility u1("U1", 0, "../TestFiles/demandsLong.csv", streamflow_n_weeks, NON_INITIALIZED, NON_INITIALIZED);
        Intake i1("I1", 2, min_env_flow_intake, catchments1, 5);
        u1.addWaterSource(&r1);
        u1.addWaterSource(&r2);
        u1.addWaterSource(&i1);
        u1.splitDemands(0);

        REQUIRE(u1.getReservoirDraw(0) == Approx((9.0617079441 - (5.0 - min_env_flow_intake)) * 1 / 3));
    }
}

TEST_CASE("Test reliability objective", "[Reliability Objective") {
    Utility_t u(0, 0, "U_test");

//    0.834874330726829	0.803223272934052	0.446117291551915
//    0.195507623927823	0.703115092725931	0.194098335461135
//    0.915611391391226	0.375596192228751	0.754884749387044
//    0.799908475529645	0.102013441894622	0.982775089481746
//    0.620921073157045	0.322484128876482	0.045317683986853

//    answer = 0.333 // from second row.

}