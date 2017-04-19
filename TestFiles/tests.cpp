//
// Created by bct52 on 4/19/17.
//

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../src/SystemComponents/WaterSources/Reservoir.h"

TEST_CASE("Net present cost calculations", "[NPC]") {
    Reservoir r("Res", 0, 1, vector<Catchment *>(), 1, 1, 1, vector<double>(2, 1), 5000, 20, 0.05);

    REQUIRE(r.calculateNetPresentCost(0, 0.05) == 5000);
    REQUIRE(r.calculateNetPresentCost(0, 0.04) == Approx(5452.6147334301));
    REQUIRE(r.calculateNetPresentCost(521, 0.04) == Approx(3685.751));
}