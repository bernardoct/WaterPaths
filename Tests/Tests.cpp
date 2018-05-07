//
// Created by Bernardo on 5/5/2018.
//


#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../src/SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../src/SystemComponents/Utility/Utility.h"
#include "../src/Utils/Utils.h"


TEST_CASE("Mass balance Allocated reservoir", "[AllocatedReservoir][Mass Balance]") {
    int streamflow_n_weeks = 52 * (1 + 50);
    vector<vector<double>> streamflows_haw =
            vector<vector<double>>(1, vector<double>((unsigned long) streamflow_n_weeks, 300.));
    vector<vector<double>> evap_jordan_lake =
            vector<vector<double>>(1, vector<double>((unsigned long) streamflow_n_weeks, 250.));
    Catchment lower_haw_river(&streamflows_haw, streamflow_n_weeks);
    vector<Catchment *> catchment_haw;
    catchment_haw.push_back(&lower_haw_river);

    EvaporationSeries evaporation_jordan_lake(
            &evap_jordan_lake,
            streamflow_n_weeks);

    /// Jordan Lake parameters
    double jl_supply_capacity = 50000.;
    double jl_wq_capacity = 50000.;
    double jl_storage_capacity = jl_wq_capacity + jl_supply_capacity;
    vector<int> jl_allocations_ids = {0, 1, 2, 3, WATER_QUALITY_ALLOCATION};
    vector<double> jl_allocation_fractions = {
            0.1 * jl_supply_capacity / jl_storage_capacity, //  5000 MG
            0.2 * jl_supply_capacity / jl_storage_capacity, // 10000 MG
            0.3 * jl_supply_capacity / jl_storage_capacity, // 15000 MG
            0.4 * jl_supply_capacity / jl_storage_capacity, // 20000 MG
            jl_wq_capacity / jl_storage_capacity};          // 50000 MG
    vector<double> jl_treatment_allocation_fractions = {0.333, 0.333, 0.333, 0.0};

    AllocatedReservoir allocated_lake("Jordan Lake",
                                   0,
                                   catchment_haw,
                                   jl_storage_capacity,
                                   1000.,
                                   evaporation_jordan_lake,
                                   1.,
                                   &jl_allocations_ids,
                                   &jl_allocation_fractions,
                                   &jl_treatment_allocation_fractions);

    vector<double> rdm_factors = {1., 1., 1.};
    allocated_lake.setRealization(0, rdm_factors);

    SECTION("Mass balance partially full, no upstream flow, no wastewater, no allocation full") {
        vector<double> demands = {20., 20., 40., 20.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 0., 0., demands);

        REQUIRE(allocated_lake.getAvailableVolume() == 99900.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 4982.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 9985.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 14967.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 19990.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 49975.);
    }

    SECTION("Mass balance partially full, upstream flow, wastewater, no allocation full") {
        vector<double> demands = {20., 20., 40., 20.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 10., 30., demands);

        REQUIRE(allocated_lake.getAvailableVolume() == 99940.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 4984.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 9989.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 14973.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 19998.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 49995.);
    }

    SECTION("Mass balance partially full, upstream flow, wastewater, no allocation full, two time steps") {
        vector<double> demands = {20., 20., 40., 20.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 10., 30., demands);
        allocated_lake.applyContinuity(1, 10., 30., demands);

        REQUIRE(allocated_lake.getAvailableVolume() == 99880.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 4969);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 9978.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 14947);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 19996.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 49990.);
    }

    SECTION("Mass balance partially full, no upstream flow, no wastewater, one allocation full with redistribution") {
        vector<double> demands = {20., 20., 40., 0.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 0., 0., demands);

        REQUIRE(allocated_lake.getAvailableVolume() == 99920.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 4983.125);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 9986.25);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 14969.375);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 20000.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 49981.25);
    }

    SECTION("Mass balance partially full, upstream flow, wastewater, no allocation full, multiple time steps, "
            "insufficient wq for 1 week", "[Negative storage]") {
        vector<double> demands = {200., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(3000.);
        for (int w = 0; w < 17; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        REQUIRE(allocated_lake.getAvailableVolume() == 33810.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 1676.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 6753.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 8429.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 16906.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 45.);
    }

    SECTION("Mass balance partially full, multiple time steps, insufficient wq for 3 weeks", "[Negative storage]") {
        vector<double> demands = {200., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(3000.);
        for (int w = 0; w < 19; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        REQUIRE(allocated_lake.getAvailableVolume() == 31900.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 1285.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 6371.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 7656.5);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 16542.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 45.);
    }

    SECTION("Mass balance partially full, multiple time steps, insufficient wq for 3 weeks", "[Negative storage]") {
        vector<double> demands = {600., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(300.);
        for (int w = 0; w < 10; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        REQUIRE(allocated_lake.getAvailableVolume() == 83900.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 0.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 8090.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 11135.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 18180.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 46495.);
    }

    SECTION("Mass balance partially full, multiple time steps, insufficient wq for 3 weeks", "[Negative storage]") {
        vector<double> demands = {600., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(5500.);
        for (int w = 0; w < 10; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        REQUIRE(allocated_lake.getAvailableVolume() == 37405.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(0) == 0.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(1) == 8090.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(2) == 11135.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(3) == 18180.);
        REQUIRE(allocated_lake.getAvailableAllocatedVolume(4) == 0.);
    }
}

TEST_CASE("Infrastructure construction", "[Infrastructure][Utility][Bonds]") {
    // Reservoir data
    int streamflow_n_weeks = 52 * (70 + 50);
    vector<vector<double>> streamflows =
            vector<vector<double>>(1, vector<double>((unsigned long) streamflow_n_weeks, 300.));
    vector<vector<double>> evaporation =
            vector<vector<double>>(1, vector<double>((unsigned long) streamflow_n_weeks, 250.));

    Catchment river_catchment(&streamflows, streamflow_n_weeks);
    vector<Catchment *> river_catchments(1, &river_catchment);
    EvaporationSeries evaporation_series(&evaporation, streamflow_n_weeks);

    LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0));

    // Reservoir
    Reservoir reservoir("Little River Reservoir (Raleigh)", 7, vector<Catchment *>(), 3700.0,
                                     ILLIMITED_TREATMENT_CAPACITY, evaporation_series, 1.,
                                     {4.5, 4.50001}, 17 * WEEKS_IN_YEAR, bond);

    // Utility data
    vector<vector<double>> demands =
            vector<vector<double>>(1, vector<double>((unsigned long) streamflow_n_weeks, 100.));
    vector<int> durham_ws_return_id = {};
    WwtpDischargeRule wwtp_discharge_durham(
            vector<vector<double>>(),
            durham_ws_return_id);
    vector<vector<int>> wjlwtp_remove_from_to_build_list;

    vector<vector<double>> demand_class_fractions = {{0.173046633,0.170108967,0.195049165,0.039185639,0.065861249,0,0,0.345705321,0,0.011043025,0.89},
                                                     {0.174218798,0.176103061,0.23872941,0.044298325,0.072064026,0,0,0.287271971,0,0.00731441,0.91},
                                                     {0.165397772,0.153365266,0.192593725,0.033804252,0.067691513,0,0,0.377294656,0,0.009852817,0.84},
                                                     {0.174090772,0.168076618,0.19902952,0.039942618,0.064681076,0,0,0.346182747,0,0.007996648,0.88},
                                                     {0.146306233,0.142541416,0.182540819,0.03397242,0.064700708,0,0,0.404017943,0,0.025920462,0.82},
                                                     {0.142660453,0.141824612,0.177751136,0.038466639,0.059367893,0,0,0.395287674,0,0.044641593,0.75},
                                                     {0.125978495,0.12909426,0.164147643,0.043465465,0.05819444,0,0,0.408360658,0,0.070759039,0.83},
                                                     {0.132975086,0.137751684,0.168810089,0.048402154,0.063577059,0,0,0.372134783,0,0.076349145,0.84},
                                                     {0.124916911,0.124843094,0.158802204,0.03935136,0.059549521,0,0,0.412025491,0,0.080511419,0.87},
                                                     {0.140403715,0.141682851,0.174901252,0.043405443,0.062146593,0,0,0.370492303,0,0.066967843,0.89},
                                                     {0.140226464,0.137090428,0.178922408,0.038725701,0.066038848,0,0,0.38148803,0,0.057508121,0.87},
                                                     {0.156423153,0.152041568,0.186688374,0.038647627,0.066779948,0,0,0.355242017,0,0.044177313,0.89}};
    vector<vector<double>> user_classes_prices = {{2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7},
                                                  {2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 2299.3, 3462.3, 3796.5, 4959.5, 7432.6, 4384.7}};

    // Setup Infrastructure construction manager holding reservoir
    vector<double> rof_triggers = {0.};
    auto infra_built_remove = vector<vector<int>>();
    vector<int> construction_order = {7};
    vector<int> construction_order_empty;

    SECTION("Infrastructure manager") {
        InfrastructureManager infrastructure_construction_manager(0, rof_triggers, infra_built_remove, 0.06, 25, 0.05,
                                                                  construction_order, vector<int>());

        vector<WaterSource *> water_sources(8);
        vector<int> priority_draw_water_source;
        vector<int> non_priority_draw_water_source;
        vector<vector<double>> debt_payment_streams;
        double total_storage_capacity;
        double total_treatment_capacity;
        double total_available_volume;
        double total_stored_volume;
        infrastructure_construction_manager.connectWaterSourcesVectorsToUtilitys(water_sources, priority_draw_water_source,
                                                                                 non_priority_draw_water_source);
        water_sources[7] = &reservoir;
        infrastructure_construction_manager.addWaterSource(&reservoir);

        SECTION("Begin building infrastructure", "[Infrastructure]") {
            infrastructure_construction_manager.beginConstruction(55, 7);
            REQUIRE(infrastructure_construction_manager.getUnder_construction()[7]);
        }

        SECTION("Set watersource online", "[Infrastructure]") {
            infrastructure_construction_manager.setWaterSourceOnline((unsigned int) 7, 0, total_storage_capacity,
                                                                     total_treatment_capacity, total_available_volume,
                                                                     total_stored_volume);

            REQUIRE(water_sources[7]->isOnline());
        }

        SECTION("Infrastruture handler high LT-ROF but week < permitting period", "[Infrastructure]") {
            infrastructure_construction_manager.infrastructureConstructionHandler(1., 53, 0, total_storage_capacity,
                                                                                  total_treatment_capacity,
                                                                                  total_available_volume,
                                                                                  total_stored_volume);
            REQUIRE(!infrastructure_construction_manager.getUnder_construction()[7]);
        }

        SECTION("Infrastruture handler high LT-ROF but week > permitting period", "[Infrastructure]") {
            infrastructure_construction_manager.infrastructureConstructionHandler(1., 1000, 0, total_storage_capacity,
                                                                                  total_treatment_capacity,
                                                                                  total_available_volume,
                                                                                  total_stored_volume);
            REQUIRE(infrastructure_construction_manager.getUnder_construction()[7]);
        }
    }

    SECTION("Utility") {
        Utility utility("U1", 0, demands, streamflow_n_weeks, 0.03, &demand_class_fractions, &user_classes_prices,
                        wwtp_discharge_durham, 0., construction_order, construction_order_empty, rof_triggers, 0.07, 25,
                        0.05);

        utility.addWaterSource(&reservoir);

        vector<double> utility_rdm(4, 1.);
        utility.setRealization(0, utility_rdm);

        SECTION("Infrastruture handler high LT-ROF but week < permitting period", "[Infrastructure]") {
            utility.infrastructureConstructionHandler(1., 0);
            REQUIRE(!utility.getInfrastructure_construction_manager().getUnder_construction()[7]);
        }

        SECTION("Infrastruture handler high LT-ROF but week > permitting period", "[Infrastructure]") {
            utility.infrastructureConstructionHandler(1., 1461);
            REQUIRE(utility.getInfrastructure_construction_manager().getUnder_construction()[7]);
        }

        SECTION("Infrastructure beginning and finishing", "[Infrastructure]") {
            utility.infrastructureConstructionHandler(1., 1461);
            utility.infrastructureConstructionHandler(1., 1461 + (int) reservoir.construction_time + 1);
            REQUIRE(!utility.getInfrastructure_construction_manager().getUnder_construction()[7]);
        }
    }
}

TEST_CASE("Bonds") {
    double e = 0.005;

    SECTION("Level debt service begin after inauguration", "[Bonds]") {
        LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0));
        SECTION("No discount nor premium") {
            bond.issueBond(0, 156, 1., 1.);
            REQUIRE(bond.getNetPresentValueAtIssuance(0.05, 0) == Approx(86.38).epsilon(e));
            REQUIRE(bond.getNetPresentValueAtIssuance(0.07, 0) == Approx(67.50).epsilon(e));
            REQUIRE(bond.getNetPresentValueAtIssuance(0.03, 0) == Approx(113.07).epsilon(e));
            REQUIRE(bond.getDebtService(104) == 0.);
            for (int w = 156; w < 1410; ++w) {
                if (Utils::weekOfTheYear(w) == 0) {
                    REQUIRE(bond.getDebtService((int) w) == Approx(7.10).epsilon(e));
                }
            }
            REQUIRE(bond.getDebtService(1461) == 0.);
        }
    }

    SECTION("Level debt service begin the year after issuance", "[Bonds]") {
        LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0), BEGIN_REPAYMENT_AT_ISSUANCE);
        SECTION("No discount nor premium") {
            bond.issueBond(0, 156, 1., 1.);
            REQUIRE(bond.getNetPresentValueAtIssuance(0.05, 0) == Approx(100).epsilon(e));
            REQUIRE(bond.getNetPresentValueAtIssuance(0.07, 0) == Approx(82.69).epsilon(e));
            REQUIRE(bond.getNetPresentValueAtIssuance(0.03, 0) == Approx(123.55).epsilon(e));
            for (int w = 52; w < 1305; ++w) {
                if (Utils::weekOfTheYear(w) == 0) {
                    REQUIRE(bond.getDebtService((int) w) == Approx(7.10).epsilon(e));
                }
            }
            REQUIRE(bond.getDebtService(1304) == 0.);
        }
    }
}