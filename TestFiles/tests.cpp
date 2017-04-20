//
// Created by bct52 on 4/19/17.
//


#include "../src/SystemComponents/WaterSources/Reservoir.h"
#include "../src/Utils/Utils.h"
#include "catch-main.h"

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