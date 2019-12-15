//
// Created by Bernardo on 5/5/2018.
//


#define CATCH_CONFIG_MAIN

#include "Catch2/single_include/catch2/catch.hpp"
#include "../src/SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../src/SystemComponents/WaterSources/WaterReuse.h"
#include "../src/SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../src/SystemComponents/Utility/Utility.h"
#include "../src/Utils/Utils.h"
#include "../src/SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../src/ContinuityModels/ContinuityModelRealization.h"
#include "../src/InputFileParser/MasterSystemInputFileParser.h"
#include "../src/InputFileParser/CatchmentParser.h"
#include "../src/InputFileParser/Exceptions/MissingParameter.h"
#include "../src/InputFileParser/Exceptions/InconsistentMutuallyImplicativeParameters.h"
#include "../src/DroughtMitigationInstruments/Transfers.h"
#include "../src/Controls/FixedMinEnvFlowControl.h"
#include "../src/Controls/SeasonalMinEnvFlowControl.h"
#include "../src/Controls/InflowMinEnvFlowControl.h"
#include "../src/SystemComponents/WaterSources/JointTreatmentCapacityExpansion.h"

using namespace Catch::literals;

TEST_CASE("Mass balance Allocated reservoir",
          "[AllocatedReservoir][Mass Balance][Exceptions]") {
    int streamflow_n_weeks = 52 * (1 + 50);
    vector<vector<double>> streamflows_haw =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 300.));
    vector<vector<double>> evap_jordan_lake =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 250.));
    Catchment lower_haw_river(streamflows_haw, streamflow_n_weeks);
    vector<Catchment *> catchment_haw;
    catchment_haw.push_back(&lower_haw_river);

    EvaporationSeries evaporation_jordan_lake(
            evap_jordan_lake,
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
    vector<double> jl_treatment_allocation_fractions = {0.333, 0.333, 0.333,
                                                        0.0};

    AllocatedReservoir allocated_lake("Jordan Lake",
                                      0,
                                      catchment_haw,
                                      jl_storage_capacity,
                                      1000.,
                                      evaporation_jordan_lake,
                                      1.,
                                      jl_allocations_ids,
                                      jl_allocation_fractions,
                                      jl_treatment_allocation_fractions);

    vector<double> rdm_factors = {1., 1., 1.};
    allocated_lake.setRealization(0, rdm_factors);

    SECTION("Mass balance partially full, no upstream flow, no wastewater, no allocation full") {
        vector<double> demands = {20., 20., 40., 20.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 0., 0., demands);

        CHECK(allocated_lake.getAvailableVolume() == 99900.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 4982.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 9985.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 14967.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 19990.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 49975.);
    }

    SECTION("Mass balance partially full, upstream flow, wastewater, no allocation full") {
        vector<double> demands = {20., 20., 40., 20.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 10., 30., demands);

        CHECK(allocated_lake.getAvailableVolume() == 99940.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 4984.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 9989.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 14973.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 19998.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 49995.);
    }

    SECTION("Mass balance partially full, upstream flow, wastewater, no allocation full, two time steps") {
        vector<double> demands = {20., 20., 40., 20.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 10., 30., demands);
        allocated_lake.applyContinuity(1, 10., 30., demands);

        CHECK(allocated_lake.getAvailableVolume() == 99880.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 4969);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 9978.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 14947);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 19996.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 49990.);
    }

    SECTION("Mass balance partially full, no upstream flow, no wastewater, one allocation full with redistribution") {
        vector<double> demands = {20., 20., 40., 0.};
        allocated_lake.setMin_environmental_outflow(50.);
        allocated_lake.applyContinuity(0, 0., 0., demands);

        CHECK(allocated_lake.getAvailableVolume() == 99920.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 4983.125);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 9986.25);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 14969.375);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 20000.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 49981.25);
    }

    SECTION("Mass balance partially full, upstream flow, wastewater, no allocation full, multiple time steps, "
            "insufficient wq for 1 week", "[Negative storage]") {
        vector<double> demands = {200., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(3000.);
        for (int w = 0; w < 17; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        CHECK(allocated_lake.getAvailableVolume() == 33810.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 1676.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 6753.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 8429.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 16906.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 45.);
    }

    SECTION("Mass balance partially full, multiple time steps, insufficient wq for 3 weeks",
            "[Negative storage]") {
        vector<double> demands = {200., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(3000.);
        for (int w = 0; w < 19; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        CHECK(allocated_lake.getAvailableVolume() == 31900.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 1285.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 6371.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 7656.5);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 16542.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 45.);
    }

    SECTION("Mass balance partially full, multiple time steps, insufficient wq for 3 weeks",
            "[Negative storage]") {
        vector<double> demands = {600., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(300.);
        for (int w = 0; w < 10; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        CHECK(allocated_lake.getAvailableVolume() == 83900.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 0.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 8090.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 11135.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 18180.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 46495.);
    }

    SECTION("Mass balance partially full, multiple time steps, insufficient wq for 3 weeks",
            "[Negative storage]") {
        vector<double> demands = {600., 200., 400., 200.};
        allocated_lake.setMin_environmental_outflow(5500.);
        for (int w = 0; w < 10; ++w) {
            allocated_lake.applyContinuity(w, 10., 30., demands);
        }

        CHECK(allocated_lake.getAvailableVolume() == 37405.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(0) == 0.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(1) == 8090.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(2) == 11135.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(3) == 18180.);
        CHECK(allocated_lake.getAvailableAllocatedVolume(4) == 0.);
        CHECK(allocated_lake.getTotal_outflow() >= 0.);
    }

    SECTION("Exception for treatment capacity allocated to water quality pool.",
            "[Exception]") {

        vector<int> jl_allocations_ids_wq_pool = {0, 1, 2, 3,
                                                  WATER_QUALITY_ALLOCATION};
        vector<double> jl_treatment_allocation_fractions_extra_for_wq_pool = {
                0.333, 0.333, 0.233, 0.0, 0.1};

        CHECK_THROWS([&]() {
            AllocatedReservoir allocated_lake("Jordan Lake",
                                              0,
                                              catchment_haw,
                                              jl_storage_capacity,
                                              1000.,
                                              evaporation_jordan_lake,
                                              1.,
                                              jl_allocations_ids_wq_pool,
                                              jl_allocation_fractions,
                                              jl_treatment_allocation_fractions_extra_for_wq_pool);
        }());
    }

}

TEST_CASE("Utility and infrastructure basic functionalities",
          "[Infrastructure][Utility][Bond]") {
    // Reservoir data
    int streamflow_n_weeks = 52 * (70 + 50);
    vector<vector<double>> streamflows =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 300.));
    vector<vector<double>> evaporation =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 250.));

    Catchment river_catchment(streamflows, streamflow_n_weeks);
    vector<Catchment *> river_catchments(1, &river_catchment);
    EvaporationSeries evaporation_series(evaporation, streamflow_n_weeks);

    LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0));
    LevelDebtServiceBond bond1(1, 200.0, 25, 0.05, vector<int>(1, 0));
    LevelDebtServiceBond bond2(2, 400.0, 25, 0.05, vector<int>(1, 0));

    // Reservoir
    Reservoir existing_reservoir("Existing Reservoir", 0,
                                 vector<Catchment *>(), 1000.0,
                                 ILLIMITED_TREATMENT_CAPACITY,
                                 evaporation_series, 1.);
    Reservoir reservoir("Little River Reservoir (Raleigh)", 7,
                        vector<Catchment *>(), 3700.0,
                        ILLIMITED_TREATMENT_CAPACITY, evaporation_series, 1.,
                        {4.5, 4.50001}, 17 * WEEKS_IN_YEAR, bond);
    ReservoirExpansion expansion1("Existing Res Expansion First Phase", 8, 0, 500.,
                                  {3, 3.0001}, 0, bond1);
    ReservoirExpansion expansion2("Existing Res Expansion First Phase", 9, 0, 1000.,
                                  {4, 4.0001}, 0, bond2);

    // UtilityParser data
    vector<vector<double>> demands =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 100.));
    vector<int> durham_ws_return_id = {};
    WwtpDischargeRule wwtp_discharge_durham(
            vector<vector<double>>(),
            durham_ws_return_id);
    vector<vector<int>> wjlwtp_remove_from_to_build_list;

    vector<vector<double>> demand_class_fractions = {{0.173046633, 0.170108967, 0.195049165, 0.039185639, 0.065861249, 0, 0, 0.345705321, 0, 0.011043025, 0.89},
                                                     {0.174218798, 0.176103061, 0.23872941,  0.044298325, 0.072064026, 0, 0, 0.287271971, 0, 0.00731441,  0.91},
                                                     {0.165397772, 0.153365266, 0.192593725, 0.033804252, 0.067691513, 0, 0, 0.377294656, 0, 0.009852817, 0.84},
                                                     {0.174090772, 0.168076618, 0.19902952,  0.039942618, 0.064681076, 0, 0, 0.346182747, 0, 0.007996648, 0.88},
                                                     {0.146306233, 0.142541416, 0.182540819, 0.03397242,  0.064700708, 0, 0, 0.404017943, 0, 0.025920462, 0.82},
                                                     {0.142660453, 0.141824612, 0.177751136, 0.038466639, 0.059367893, 0, 0, 0.395287674, 0, 0.044641593, 0.75},
                                                     {0.125978495, 0.12909426,  0.164147643, 0.043465465, 0.05819444,  0, 0, 0.408360658, 0, 0.070759039, 0.83},
                                                     {0.132975086, 0.137751684, 0.168810089, 0.048402154, 0.063577059, 0, 0, 0.372134783, 0, 0.076349145, 0.84},
                                                     {0.124916911, 0.124843094, 0.158802204, 0.03935136,  0.059549521, 0, 0, 0.412025491, 0, 0.080511419, 0.87},
                                                     {0.140403715, 0.141682851, 0.174901252, 0.043405443, 0.062146593, 0, 0, 0.370492303, 0, 0.066967843, 0.89},
                                                     {0.140226464, 0.137090428, 0.178922408, 0.038725701, 0.066038848, 0, 0, 0.38148803,  0, 0.057508121, 0.87},
                                                     {0.156423153, 0.152041568, 0.186688374, 0.038647627, 0.066779948, 0, 0, 0.355242017, 0, 0.044177313, 0.89}};
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
    double total_storage_capacity;
    double total_treatment_capacity;
    double total_available_volume;
    double total_stored_volume;

    vector<WaterSource *> water_sources(10);
    vector<int> priority_draw_water_source;
    vector<int> non_priority_draw_water_source;
    vector<vector<double>> debt_payment_streams;

    SECTION("Single infrastructure option.",
            "[WaterSource][Infrastructure]") {

        // Setup Infrastructure construction manager holding reservoir
        vector<double> rof_triggers = vector<double>(1, 0.);
        auto infra_built_remove = vector<vector<int>>();
        vector<int> construction_order = {7};
        vector<int> construction_order_empty;

        Utility utility("U1", 0, demands, streamflow_n_weeks, 0.03,
                        demand_class_fractions, user_classes_prices,
                        wwtp_discharge_durham, 0., construction_order,
                        construction_order_empty, rof_triggers, 0.07,
                        vector<vector<int>>(), vector<vector<int>>());

        utility.addWaterSource(&reservoir);

        SECTION("Construction time within bounds.",
                "[WaterSource][Infrastructure]") {
            for (int i = 0; i < 10; ++i) {
                vector<double> construction_time_interval = {3., 5.};
                WaterReuse reuse("Test", 18, 5., construction_time_interval,
                                 7 * WEEKS_IN_YEAR, bond);
                CHECK(reuse.getConstruction_time() > WEEKS_IN_YEAR * 3 - 1);
                CHECK(reuse.getConstruction_time() < WEEKS_IN_YEAR * 5 + 1);
            }
        }

        SECTION("Infrastructure manager", "[InfrastructureManager]") {
            InfrastructureManager infrastructure_construction_manager(0,
                                                                      rof_triggers,
                                                                      infra_built_remove,
                                                                      vector<vector<int>>(),
                                                                      0.06,
                                                                      construction_order,
                                                                      vector<int>());
            infrastructure_construction_manager.connectWaterSourcesVectorsToUtilitys(
                    water_sources, priority_draw_water_source,
                    non_priority_draw_water_source);
            water_sources[7] = &reservoir;
            infrastructure_construction_manager.addWaterSource(&reservoir);

            SECTION("Begin building infrastructure", "[Infrastructure]") {
                infrastructure_construction_manager.beginConstruction(55, 7);
                CHECK(infrastructure_construction_manager.getUnder_construction()[7]);
            }

            SECTION("Set watersource online", "[Infrastructure]") {
                infrastructure_construction_manager.setWaterSourceOnline(
                        (unsigned int) 7, 0, total_storage_capacity,
                        total_treatment_capacity, total_available_volume,
                        total_stored_volume);

                CHECK(water_sources[7]->isOnline());
            }

            SECTION("Infrastruture handler high LT-ROF but week < permitting period",
                    "[Infrastructure]") {
                infrastructure_construction_manager.infrastructureConstructionHandler(
                        1., 53, 0, total_storage_capacity,
                        total_treatment_capacity,
                        total_available_volume,
                        total_stored_volume);
                CHECK(!infrastructure_construction_manager.getUnder_construction()[7]);
            }

            SECTION("Infrastruture handler high LT-ROF but week > permitting period",
                    "[Infrastructure]") {
                infrastructure_construction_manager.infrastructureConstructionHandler(
                        1., 1000, 0, total_storage_capacity,
                        total_treatment_capacity,
                        total_available_volume,
                        total_stored_volume);
                CHECK(infrastructure_construction_manager.getUnder_construction()[7]);
            }
        }


        SECTION("Utility infrastructure construction",
                "[Utility][Infrastructure]") {

            vector<double> utility_rdm(4, 1.);
            utility.setRealization(0, utility_rdm);

            SECTION("Infrastruture handler high LT-ROF but week < permitting period") {
                utility.infrastructureConstructionHandler(1., 0);
                CHECK(!utility.getInfrastructure_construction_manager().getUnder_construction()[7]);
            }

            SECTION("Infrastruture handler high LT-ROF but week > permitting period") {
                utility.infrastructureConstructionHandler(1., 1461);
                CHECK(utility.getInfrastructure_construction_manager().getUnder_construction()[7]);
            }

            SECTION("Infrastructure beginning and finishing") {
                utility.infrastructureConstructionHandler(1., 1461);
                utility.infrastructureConstructionHandler(1., 1461 +
                                                              (int) reservoir.construction_time +
                                                              1);
                CHECK(!utility.getInfrastructure_construction_manager().getUnder_construction()[7]);
            }
        }


        SECTION("Joint infrastructure",
                "[Infrastructure][Joint infrastructure]") {
            vector<double> rof_triggers_non_zero = vector<double>(10, 0.1);
            vector<int> construction_order_seq = {0};
            Utility utility1("U1", 0, demands, streamflow_n_weeks, 0.03,
                             demand_class_fractions, user_classes_prices,
                             wwtp_discharge_durham, 0., construction_order_seq,
                             construction_order_empty, rof_triggers,
                             0.07, vector<vector<int>>(),
                             vector<vector<int>>());
            Utility utility2("U2", 1, demands, streamflow_n_weeks, 0.03,
                             demand_class_fractions, user_classes_prices,
                             wwtp_discharge_durham, 0., construction_order_seq,
                             construction_order_empty, rof_triggers,
                             0.07, vector<vector<int>>(),
                             vector<vector<int>>());
            LevelDebtServiceBond bond1(0, 200.0, 25, 0.05, vector<int>(1, 0));
            LevelDebtServiceBond bond2(1, 300.0, 25, 0.05, vector<int>(1, 0));

            // Allocated Reservoir
            vector<int> utilities_with_allocations = {0, 1};
            vector<double> capacity_to_be_added = {10, 20};
            map<int, double> added_treatment_capacities = {{0, 10}, {1, 20}};
            vector<double> allocated_fractions = {0.2, 0.4};
            vector<double> allocated_treatment_fractions = {0.3, 0.5};

            AllocatedReservoir allocated_reservoir("My allocated reservoir", 1,
                                                   vector<Catchment *>(),
                                                   1000.0,
                                                   100, evaporation_series, 1.,
                                                   utilities_with_allocations,
                                                   allocated_fractions,
                                                   allocated_treatment_fractions);

            vector<Bond *> bonds = Utils::copyBonds({&bond1, &bond2});
            // Sequential joint wtp expansion to be added to allocated reservoir ID 1.
            JointTreatmentCapacityExpansion wtp("WTP expansion", 0, 1, added_treatment_capacities, bonds,
                                                  {3.000, 3.001}, 0.);

            vector<WaterSource *> water_sources =
                    Utils::copyWaterSourceVector({&allocated_reservoir, &wtp});
            vector<Utility *> utilities =
                    Utils::copyUtilityVector({&utility1, &utility2});

            vector<vector<int>> water_sources_to_utilities = {{0, 1},
                                                              {0, 1}};
            vector<DroughtMitigationPolicy *> dmps;
            vector<MinEnvFlowControl *> mev;
            auto rdm = vector<vector<double>>(2, vector<double>(50, 1.));

            ContinuityModelRealization model(water_sources, Graph(1),
                                             water_sources_to_utilities,
                                             utilities, dmps, mev, rdm[0],
                                             rdm[0], rdm[0], 0);

            vector<double> lt_rofs = {1., 0.};

            SECTION("Test triggering and forced construction for joint infrastructure.") {
                // Begin construction
                model.setLongTermROFs(lt_rofs, 100);

                // Check triggering
                CHECK(model.getContinuity_utilities()[0]->
                        getInfrastructure_construction_manager().getUnder_construction()[0]);
                CHECK(model.getContinuity_utilities()[1]->
                        getInfrastructure_construction_manager().getUnder_construction()[0]);
                CHECK(!model.getContinuity_water_sources()[0]->isOnline());

                // Check financial
                CHECK(model.getContinuity_utilities()[0]->
                        getInfrastructure_net_present_cost() == Approx(134.99));
                CHECK(model.getContinuity_utilities()[1]->
                        getInfrastructure_net_present_cost() ==
                      Approx(202.49).epsilon(0.005));
            }

            SECTION("Test setting joint infrastructure online after construction "
                    "period.") {
                // Begin construction
                model.setLongTermROFs(lt_rofs, 100);
                // Finish construction and set source online
                model.setLongTermROFs(lt_rofs, 362);
                CHECK(!model.getContinuity_utilities()[0]->
                                getInfrastructure_construction_manager()
                        .getUnder_construction()[0]);
                CHECK(!model.getContinuity_utilities()[1]->
                                getInfrastructure_construction_manager()
                        .getUnder_construction()[0]);
                CHECK(model.getContinuity_water_sources()[1]->
                        getAllocatedTreatmentCapacity(0) == 40);
                CHECK(model.getContinuity_water_sources()[1]->
                        getAllocatedTreatmentCapacity(1) == 70);
            }
        }


        SECTION("UtilityParser other functions", "[Utility]") {

            vector<double> utility_rdm(4, 1.);
            utility.setRealization(0, utility_rdm);

            SECTION("UtilityParser split demands", "[splitDemands]") {
                Reservoir reservoir2("R2", 0, vector<Catchment *>(), 3700. * 3,
                                     ILLIMITED_TREATMENT_CAPACITY,
                                     evaporation_series, 1.);

                vector<double> rdm_reservoir(20, 1.);
                reservoir.setRealization(0, rdm_reservoir);
                reservoir2.setRealization(0, rdm_reservoir);

                utility.addWaterSource(&reservoir2);
                vector<vector<double>> split_demands(8, vector<double>(1));

                SECTION("One source online and other offline") {
                    utility.splitDemands(0, split_demands);

                    CHECK(split_demands[0][0] == 100.);
                    CHECK(split_demands[7][0] == 0.);
                }

                SECTION("Two sources online") {
                    utility.setWaterSourceOnline(7, 0);
                    reservoir.applyContinuity(0, 10000, 0,
                                              split_demands[0]); // Fill reservoir
                    utility.updateTotalAvailableVolume();
                    utility.splitDemands(0, split_demands);

                    CHECK(split_demands[0][0] == 75.);
                    CHECK(split_demands[7][0] == 25.);
                }

                SECTION("Two sources online, unfulfilled demand") {
                    utility.setWaterSourceOnline(7, 0);
                    reservoir.applyContinuity(0, 260, 0,
                                              split_demands[0]); // Fill reservoir
                    vector<double> demand_reservoir2(1, 3700. * 3 - 270.);
                    reservoir2.applyContinuity(0, 0, 0,
                                               demand_reservoir2); // Fill reservoir
                    utility.updateTotalAvailableVolume();
                    utility.splitDemands(0, split_demands);

                    CHECK(split_demands[0][0] == 20.);
                    CHECK(split_demands[7][0] == 10.);
                    CHECK(utility.getUnfulfilled_demand() == 70.);
                }
            }
        }
    }

    SECTION("Multiple connected infrastructure options") {
        // Setup Infrastructure construction manager holding reservoir
        vector<double> rof_triggers = vector<double>(3, 0.1);
        auto infra_built_remove = vector<vector<int>>();
        vector<int> construction_order = {7, 9, 8};
        vector<vector<int>> construction_pre_requisites = {{8, 9}};
        vector<int> construction_order_empty;
        int new_infra;
        total_storage_capacity = 0;
        priority_draw_water_source = {0};

        water_sources[0] = &existing_reservoir;
        water_sources[7] = &reservoir;
        water_sources[8] = &expansion1;
        water_sources[9] = &expansion2;

        InfrastructureManager manager(0, rof_triggers, {{}},
                                      construction_pre_requisites, 0.05,
                                      construction_order,
                                      construction_order_empty);

        manager.connectWaterSourcesVectorsToUtilitys(
                water_sources, priority_draw_water_source,
                non_priority_draw_water_source);
        manager.addWaterSource(&existing_reservoir);
        manager.addWaterSource(&reservoir);
        manager.addWaterSource(&expansion1);
        manager.addWaterSource(&expansion2);

        new_infra = manager.infrastructureConstructionHandler(
                1., 1, 0., total_storage_capacity, total_treatment_capacity,
                total_available_volume, total_stored_volume);
        CHECK(new_infra == 8);
        CHECK(total_storage_capacity == 0);

        new_infra = manager.infrastructureConstructionHandler(
                0., 499, 0., total_storage_capacity, total_treatment_capacity,
                total_available_volume, total_stored_volume);
        new_infra = manager.infrastructureConstructionHandler(
                0., 540, 0., total_storage_capacity, total_treatment_capacity,
                total_available_volume, total_stored_volume);
        CHECK(new_infra == 9);
        CHECK(total_storage_capacity == 1500);

        new_infra = manager.infrastructureConstructionHandler(
                0., 1000, 0., total_storage_capacity, total_treatment_capacity,
                total_available_volume, total_stored_volume);
        new_infra = manager.infrastructureConstructionHandler(
                0., 999, 0., total_storage_capacity, total_treatment_capacity,
                total_available_volume, total_stored_volume);
        CHECK(new_infra == NON_INITIALIZED);
        CHECK(total_storage_capacity == 2500);

        new_infra = manager.infrastructureConstructionHandler(
                1., 1500, 0., total_storage_capacity, total_treatment_capacity,
                total_available_volume, total_stored_volume);
        CHECK(new_infra == 7);
        CHECK(total_storage_capacity == 2500);
    }
}


TEST_CASE("BondParsers") {
    double e = 0.005;

    SECTION("Level debt service begin after inauguration", "[BondParsers]") {
        LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0));
        SECTION("No discount nor premium") {
            bond.issueBond(0, 156, 1., 1.);
            CHECK(bond.getNetPresentValueAtIssuance(0.05, 0) ==
                  Approx(86.38).epsilon(e));
            CHECK(bond.getNetPresentValueAtIssuance(0.07, 0) ==
                  Approx(67.50).epsilon(e));
            CHECK(bond.getNetPresentValueAtIssuance(0.03, 0) ==
                  Approx(113.07).epsilon(e));
            CHECK(bond.getDebtService(104) == 0.);
            for (int w = 156; w < 1410; ++w) {
                if (Utils::weekOfTheYear(w) == 0) {
                    CHECK(bond.getDebtService((int) w) ==
                          Approx(7.10).epsilon(e));
                }
            }
            CHECK(bond.getDebtService(1461) == 0.);
        }
    }

    SECTION("Level debt service begin the year after issuance",
            "[BondParsers]") {
        LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0),
                                  BEGIN_REPAYMENT_AT_ISSUANCE);
        SECTION("No discount nor premium") {
            bond.issueBond(0, 156, 1., 1.);
            CHECK(bond.getNetPresentValueAtIssuance(0.05, 0) ==
                  Approx(100).epsilon(e));
            CHECK(bond.getNetPresentValueAtIssuance(0.07, 0) ==
                  Approx(82.69).epsilon(e));
            CHECK(bond.getNetPresentValueAtIssuance(0.03, 0) ==
                  Approx(123.55).epsilon(e));
            for (int w = 52; w < 1305; ++w) {
                if (Utils::weekOfTheYear(w) == 0) {
                    CHECK(bond.getDebtService((int) w) ==
                          Approx(7.10).epsilon(e));
                }
            }
            CHECK(bond.getDebtService(1304) == 0.);
        }
    }
}


TEST_CASE("Test Joint Triggering of Allocated Reservoir",
          "[Allocated Reservoir][Infrastructure]") {
    // Reservoir data
    int streamflow_n_weeks = 52 * (70 + 100);
    vector<vector<double>> streamflows =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 300.));
    vector<vector<double>> evaporation =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 250.));

    Catchment river_catchment(streamflows, streamflow_n_weeks);
    vector<Catchment *> river_catchments(1, &river_catchment);
    EvaporationSeries evaporation_series(evaporation, streamflow_n_weeks);


    // UtilityParser data
    vector<vector<double>> demands =
            vector<vector<double>>(1, vector<double>(
                    (unsigned long) streamflow_n_weeks, 100.));
    vector<int> ws_return_id = {3};

    vector<vector<double>> demand_to_wastewater_fraction = Utils::parse2DCsvFile(
            "../TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");

    WwtpDischargeRule wwtp_discharge(
            demand_to_wastewater_fraction,
            ws_return_id);

    vector<vector<double>> demand_class_fractions = {{0.173046633, 0.170108967, 0.195049165, 0.039185639, 0.065861249, 0, 0, 0.345705321, 0, 0.011043025, 0.89},
                                                     {0.174218798, 0.176103061, 0.23872941,  0.044298325, 0.072064026, 0, 0, 0.287271971, 0, 0.00731441,  0.91},
                                                     {0.165397772, 0.153365266, 0.192593725, 0.033804252, 0.067691513, 0, 0, 0.377294656, 0, 0.009852817, 0.84},
                                                     {0.174090772, 0.168076618, 0.19902952,  0.039942618, 0.064681076, 0, 0, 0.346182747, 0, 0.007996648, 0.88},
                                                     {0.146306233, 0.142541416, 0.182540819, 0.03397242,  0.064700708, 0, 0, 0.404017943, 0, 0.025920462, 0.82},
                                                     {0.142660453, 0.141824612, 0.177751136, 0.038466639, 0.059367893, 0, 0, 0.395287674, 0, 0.044641593, 0.75},
                                                     {0.125978495, 0.12909426,  0.164147643, 0.043465465, 0.05819444,  0, 0, 0.408360658, 0, 0.070759039, 0.83},
                                                     {0.132975086, 0.137751684, 0.168810089, 0.048402154, 0.063577059, 0, 0, 0.372134783, 0, 0.076349145, 0.84},
                                                     {0.124916911, 0.124843094, 0.158802204, 0.03935136,  0.059549521, 0, 0, 0.412025491, 0, 0.080511419, 0.87},
                                                     {0.140403715, 0.141682851, 0.174901252, 0.043405443, 0.062146593, 0, 0, 0.370492303, 0, 0.066967843, 0.89},
                                                     {0.140226464, 0.137090428, 0.178922408, 0.038725701, 0.066038848, 0, 0, 0.38148803,  0, 0.057508121, 0.87},
                                                     {0.156423153, 0.152041568, 0.186688374, 0.038647627, 0.066779948, 0, 0, 0.355242017, 0, 0.044177313, 0.89}};
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

    // Reservoirs
    vector<double> res_storage = {0, 1000};
    vector<double> res_area = {0, 0.3675 * 1000};
    DataSeries reservoir_storage_area(res_storage, res_area);


    Reservoir reservoir_zero("existing_infrastructure reservoir 0", 0,
                             vector<Catchment *>(), 1000.0,
                             ILLIMITED_TREATMENT_CAPACITY, evaporation_series,
                             reservoir_storage_area);

    // Reservoir
    Reservoir reservoir_one("existing_infrastructure reservoir 1", 1,
                            river_catchments, 1000.0,
                            ILLIMITED_TREATMENT_CAPACITY, evaporation_series,
                            reservoir_storage_area);

    Reservoir dummy_endpoint("existing_infrastructure reservoir 1", 3,
                             river_catchments, 1000.0,
                             ILLIMITED_TREATMENT_CAPACITY, evaporation_series,
                             reservoir_storage_area);


    // Setup Infrastructure construction manager holding reservoir
    vector<double> rof_triggers = vector<double>(1, 0.);
    auto infra_built_remove = vector<vector<int>>();
    vector<int> construction_order = {1};
    vector<int> construction_order_empty;

    vector<WaterSource *> water_sources;
    water_sources.push_back(&reservoir_zero);
    water_sources.push_back(&reservoir_one);


    //vector<double> rof_triggers_non_zero = vector<double>(10, 0.1);
    vector<int> construction_order_seq = {2};


    Utility utility1("U1", 0, demands, streamflow_n_weeks, 0.03,
                     demand_class_fractions, user_classes_prices,
                     wwtp_discharge, 0.,
                     construction_order_seq, construction_order_empty,
                     rof_triggers, 0.07, vector<vector<int>>(),
                     vector<vector<int>>());

    Utility utility2("U2", 1, demands, streamflow_n_weeks, 0.03,
                     demand_class_fractions, user_classes_prices,
                     wwtp_discharge, 0.,
                     construction_order_seq, construction_order_empty,
                     rof_triggers, 0.07, vector<vector<int>>(),
                     vector<vector<int>>());

    vector<Utility *> utilities;
    utilities.push_back(&utility1);
    utilities.push_back(&utility2);

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 2}, // utility1
            {1, 2}  // utility2
    };


    // Allocated Reservoir
    vector<int> utilities_with_allocations = {0, 1};
    vector<double> allocated_fractions = {0.5, 0.5};
    vector<double> allocated_treatment_fractions = {0.5, 0.5};

    vector<double> allocated_res_storage = {0, 1000};
    vector<double> allocated_res_area = {0, 0.3675 * 1000};
    DataSeries allocated_reservoir_storage_area(allocated_res_storage,
                                                allocated_res_area);

    LevelDebtServiceBond bond(0, 100.0, 25, 0.05, vector<int>(1, 0));
    vector<double> construction_time_interval = {3.0, 5.0};

    AllocatedReservoir allocated_reservoir("My allocated reservoir", 2,
                                           river_catchments, 1000.0,
                                           ILLIMITED_TREATMENT_CAPACITY,
                                           evaporation_series,
                                           allocated_reservoir_storage_area,
                                           construction_time_interval,
                                           17 * WEEKS_IN_YEAR,
                                           bond,
                                           utilities_with_allocations,
                                           allocated_fractions,
                                           allocated_treatment_fractions);

    water_sources.push_back(&allocated_reservoir);
    vector<DroughtMitigationPolicy *> dmps;
    vector<MinEnvFlowControl *> mev;
    auto rdm = vector<vector<double>>(2, vector<double>(50, 1.));

    allocated_reservoir.setOnline();

    SECTION("Testing initial reservoir allocation volumes") {
        //Available volumes should all be initialized to zero
        CHECK(allocated_reservoir.getAvailableVolume() == 0);
        CHECK(allocated_reservoir.getAvailable_allocated_volumes()[0] == 0);
        CHECK(allocated_reservoir.getAvailable_allocated_volumes()[1] == 0);
    }

}

TEST_CASE("Create a utility", "[UtilityParser]") {
    unsigned long n_realizations = 5;
    auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);
    vector<vector<double>> demand_watertown = Utils::parse2DCsvFile(
            "../TestFiles/demands/cary_demand.csv", n_realizations);

    vector<int> watertown_ws_return_id;
    vector<vector<double>> watertown_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_watertown(
            watertown_discharge_fraction_series,
            watertown_ws_return_id);

    vector<int> construction_order_seq = {2};
    vector<double> rof_triggers = vector<double>(1, 0.);

    vector<vector<double>> watertownDemandClassesFractions = Utils::parse2DCsvFile(
            "../TestFiles/caryDemandClassesFractions.csv");

    vector<vector<double>> watertownUserClassesWaterPrices = Utils::parse2DCsvFile(
            "../TestFiles/caryUserClassesWaterPrices.csv");

    Utility watertown("Watertown", 0, demand_watertown, demand_n_weeks,
                      .1,
                      watertownDemandClassesFractions,
                      watertownUserClassesWaterPrices, wwtp_discharge_watertown,
                      0, construction_order_seq, vector<int>(), rof_triggers,
                      .05, vector<vector<int>>(), vector<vector<int>>());
};

TEST_CASE("Read time series.", "[Time series parsing]") {
    SECTION("Read time series.", "[Time series parsing]") {
        auto series1 = Utils::parse2DCsvFile(
                "../TestFiles/inflows/durham_inflows.csv");
        auto series2 = Utils::parse2DCsvFile(
                "../TestFiles/inflows/falls_lake_inflows.csv");
        vector<vector<vector<double>> *> series = {&series1, &series2};
        CatchmentParser catchment_parser;
        catchment_parser.parseSeries(series, 3000, 1000);

        auto catchments = catchment_parser.getParsedCatchments();
        vector<double> rdms(50, 1.);
        catchments[0]->setRealization(1, rdms);
        catchments[1]->setRealization(10, rdms);
        CHECK(catchments.size() == 2);
        CHECK(catchments[0]->getStreamflow(0) == Approx(2525.78));
        CHECK(catchments[1]->getStreamflow(0) == Approx(2409.25));
    }
}

TEST_CASE("Read input file.",
          "[Input File Parser][Aux input functions][Read input file block][Exceptions]") {

    auto rdm_vec = vector<double>(50, 1.);

    SECTION("Check missing parameter exception handling for input parser",
            "[Input File Parser][Exceptions]") {
        string input_test_file = "../Tests/test_input_file_missing_name.wp";
        MasterSystemInputFileParser parser;
        CHECK_THROWS_AS(parser.preloadAndCheckInputFile(input_test_file),
                        MissingParameter);
    }

    SECTION("Check mutually implicative exception handling for input parser",
            "[Input File Parser][Exceptions]") {
        MasterSystemInputFileParser parser;
        string input_test_file = "../Tests/test_input_file_incomplete_non_existing_ws.wp";
        parser.preloadAndCheckInputFile(input_test_file);
        CHECK_THROWS_AS(parser.createSystemObjects(nullptr),
                        InconsistentMutuallyImplicativeParameters);
    }

    string input_test_file = "../Tests/test_input_file.wp";

    SECTION("Check if block is read appropriately.",
            "[Input File Parser][Read input file block]") {
        MasterSystemInputFileParser parser;
        int l;
        ifstream inputFile(input_test_file);
        string line;
        while (line != "[RESERVOIR]")
            getline(inputFile, line);
        auto block = MasterSystemInputFileParser::readBlock(inputFile, l, line);

        CHECK(block.size() == 6);
        CHECK(block[0][0] == "name");
        CHECK(block[1][1] == "560");
        CHECK(block[3].size() == 4);
        CHECK(block[5][1] == "222");
    }

    SECTION("Check if decision variables are properly inserted in a line.",
            "[Input File Parser][Read input file block]") {
        MasterSystemInputFileParser parser;
        string data = "0.1,%%%,%%%,%%%";
        vector<double> vars = {0.13, 0.24, 24, 0.2, 0.1};
        int count_var = 0;
        parser.replaceNumericVarsIntoPlaceHolders(vars.data(), count_var, data);
        CHECK(data == "0.1,0.130000,0.240000,24.000000");

        data = "@ccccc,aaa,@bb";
        parser.reorderCSVDataInBlockLine(vars.data(), count_var, data);
        CHECK(data == "bb,aaa,ccccc");

        vars = {0.9, 0.8, 0.13, 0.24, 24, 0.2, 0.1};
        vector<string> v1 = {"param1", "%%%,0.1", "0.75,%%%"};
        vector<string> v2 = {"param2", "0.1,%%%,%%%,%%%", "@ccccc,aaa,@bb"};
        vector<vector<vector<string>>> block = {{v1, v2}};
        parser.replacePlaceHoldersByDVs(vars.data(), block);
        v1 = {"param1", "0.900000,0.1", "0.75,0.800000"};
        v2 = {"param2", "0.1,0.130000,0.240000,24.000000", "bb,aaa,ccccc"};
        CHECK(block[0][0] == v1);
        CHECK(block[0][1] == v2);
    }
}

int zero_week = -(int) (WEEKS_IN_YEAR * WEEKS_ROF_SHORT_TERM) + 104;
auto rdm_vec = vector<double>(50, 1.);
string input_test_file = "../Tests/test_input_file.wp";

TEST_CASE(
        "Checking if water sources numeric and text data was parsed correctly.",
        "[Input File Parser][Water Sources][BondParsers]") {
    MasterSystemInputFileParser parser;
    parser.preloadAndCheckInputFile(input_test_file);
    parser.createSystemObjects(nullptr);
    auto water_sources = parser.getWaterSources();

    SECTION("Checking if built reservoir numeric and text data was parsed correctly.",
            "[Input File Parser][Reservoir][BondParsers]") {

        CHECK(water_sources[0]->id == 0);
        CHECK(water_sources[0]->name == "College_Rock_Reservoir");
        CHECK(dynamic_cast<Reservoir *>(water_sources[0])->getArea() ==
              222);
        CHECK(dynamic_cast<Reservoir *>(water_sources[0])->getSupplyCapacity() ==
              1049);
    }

    SECTION("Checking if built allocated reservoir numeric and text data was parsed correctly.",
            "[Input File Parser][Allocated Reservoir]") {

        CHECK(water_sources[3]->id == 3);
        CHECK(water_sources[3]->name == "Autumn_Lake");
        CHECK(water_sources[3]->getAllocatedFraction(1) == 0.19);
        CHECK(water_sources[3]->getAllocatedTreatmentCapacity(2) == 52000.);
        CHECK(dynamic_cast<AllocatedReservoir *>(water_sources[3])->getStorageAreaCurve().getSeries_x()[1] ==
              14000);
    }

    SECTION("Checking if non-built allocated reservoir numeric and text data was parsed correctly.",
            "[Input File Parser][Allocated Reservoir][BondParsers]") {

        CHECK(water_sources[5]->id == 5);
        CHECK(water_sources[5]->getConstruction_time() <
              5.1 * WEEKS_IN_YEAR);
        CHECK(water_sources[5]->getConstruction_time() >
              2.9 * WEEKS_IN_YEAR);
        CHECK(water_sources[5]->getBond(0).type == LEVEL_DEBT_SERVICE);
        CHECK(water_sources[5]->getBond(0).pay_on_weeks[0] == 0);
        CHECK(water_sources[5]->getBond(0).pay_on_weeks.size() == 1);
        CHECK(!water_sources[5]->getBond(0).isIssued());
        CHECK(water_sources[5]->name == "New_River_Reservoir");
        water_sources[5]->setRealization(0, rdm_vec);
        CHECK(dynamic_cast<Reservoir *>(water_sources[5])->getStorageAreaCurve().getSeries_x()[1] ==
              7300);
        CHECK(dynamic_cast<Reservoir *>(water_sources[5])->getEvaporationSeries().getEvaporation(
                zero_week) == Approx(0.0068410998f));
    }

    SECTION("Checking if reuse station numeric and text data was parsed correctly.",
            "[Input File Parser][Reuse Station][BondParsers]") {

        CHECK(water_sources[9]->id == 9);
        CHECK(water_sources[9]->getConstruction_time() <
              5.1 * WEEKS_IN_YEAR);
        CHECK(water_sources[9]->getConstruction_time() >
              2.9 * WEEKS_IN_YEAR);
        CHECK(water_sources[9]->getBond(0).type == LEVEL_DEBT_SERVICE);
        CHECK(water_sources[9]->getBond(0).pay_on_weeks[0] == 0);
        CHECK(water_sources[9]->getBond(0).pay_on_weeks.size() == 1);
        CHECK(water_sources[9]->getBond(0).getCostOfCapital() == 50);
        CHECK(water_sources[9]->getBond(0).getCouponRate() == 0.05);
        CHECK(water_sources[9]->getBond(0).getNPayments() == 30);
        CHECK(!water_sources[9]->getBond(0).isIssued());
        CHECK(water_sources[9]->name == "Watertown_Reuse_I");
        CHECK(water_sources[9]->getTotalTreatmentCapacity() == 35);
    }

    SECTION("Checking if reservoir expansion numeric and text data was parsed correctly.",
            "[Input File Parser][Reservoir Expansion][BondParsers]") {

        CHECK(water_sources[8]->id == 8);
        CHECK(water_sources[8]->getConstruction_time() <
              6.1 * WEEKS_IN_YEAR);
        CHECK(water_sources[8]->getConstruction_time() >
              3.9 * WEEKS_IN_YEAR);
        CHECK(water_sources[8]->name == "College_Rock_Expansion_High");
        CHECK(water_sources[8]->getSupplyCapacity() == 2500);
        CHECK(water_sources[8]->getBond(0).type == LEVEL_DEBT_SERVICE);
        CHECK(water_sources[8]->getBond(0).pay_on_weeks[0] == 0);
        CHECK(water_sources[8]->getBond(0).pay_on_weeks.size() == 1);
        CHECK(water_sources[8]->getBond(0).getCostOfCapital() == 100);
        CHECK(water_sources[8]->getBond(0).getCouponRate() == 0.05);
        CHECK(water_sources[8]->getBond(0).getNPayments() == 30);
        CHECK(!water_sources[8]->getBond(0).isIssued());
        CHECK(dynamic_cast<ReservoirExpansion *>(water_sources[8])->parent_reservoir_ID ==
              0);
    }
}

TEST_CASE("Checking if utility numeric and text data was parsed correctly.",
          "[Input File Parser][UtilityParser][BondParsers]") {
    MasterSystemInputFileParser parser;
    parser.preloadAndCheckInputFile(input_test_file);
    parser.createSystemObjects(new double);
    auto utilities = parser.getUtilities();
    CHECK(utilities[0]->id == 0);
    CHECK(utilities[0]->name == "Watertown");
    vector<int> expected_infra_const_order = {5, 7, 8, 9, 10};
    CHECK(utilities[0]->getRof_infrastructure_construction_order() ==
          expected_infra_const_order);
    CHECK(utilities[0]->getInfrastructure_construction_manager().getId() ==
          0);
    CHECK(utilities[0]->getInfrastructure_construction_manager().getInfraDiscountRate() ==
          0.07);
    vector<double> expected_rof_triggers = {1e10, 1e10, 1e10, 1e10, 1e10,
                                            .1, 1e10, 0.1, 0.1, 0.1, 0.1};
    CHECK(utilities[0]->getInfrastructure_construction_manager().getInfraConstructionTriggers() ==
          expected_rof_triggers);
}

TEST_CASE("Check parsers of auxiliary data.",
          "[Input File Parser][Water Sources Graph][source to utility matrix]") {
    MasterSystemInputFileParser parser;
    parser.preloadAndCheckInputFile(input_test_file);
    parser.createSystemObjects(nullptr);
    SECTION("Checking if graph was imported properly.") {
        const auto &water_sources_graph = parser.getWaterSourcesGraph();
        vector<int> top_order = {0, 1, 2, 6, 3, 5, 4};
        CHECK(water_sources_graph.getTopological_order() == top_order);
    }SECTION("Checking if connectivity matrix was imported properly.") {
        auto matrix = parser.getReservoirUtilityConnectivityMatrix();
        CHECK(matrix[0][0] == 0);
        CHECK(matrix[0][6] == 10);
        CHECK(matrix[2][2] == 12);
    }SECTION("Checking if storage shift table was imported properly.") {
        auto matrix = parser.getTableStorageShift();
        CHECK(matrix[0][0] == 0);
        CHECK(matrix[2][5] == 1500);
        CHECK(matrix[2][12] == 1000);
        CHECK(matrix[2][10] == 0);
    }
}

TEST_CASE("Checking if drought mitigation instruments are imported properly.",
          "[Input File Parser][Restrictions][Transfers][Drought Mitigation Policies]") {
    MasterSystemInputFileParser parser;
    parser.preloadAndCheckInputFile(input_test_file);
    parser.createSystemObjects(new double);
    const auto &drought_mitigation_policies = parser.getDroughtMitigationPolicy();
    SECTION("Checking if restrictions are imported properly",
            "[Input File Parser][Restrictions][Drought Mitigation Policies]") {
        CHECK(dynamic_cast<Restrictions *>(drought_mitigation_policies[0])->id ==
              0);
        vector<double> stage_multipliers = {0.9, 0.75, 0.65, 0.5};
        vector<double> stage_triggers = {0.05, 0.2, 0.4, 0.65};
        CHECK(dynamic_cast<Restrictions *>(drought_mitigation_policies[0])->getStageMultipliers() ==
              stage_multipliers);
        CHECK(dynamic_cast<Restrictions *>(drought_mitigation_policies[0])->getStageTriggers() ==
              stage_triggers);
    }

    SECTION("Checking if transfers are imported properly",
            "[Input File Parser][Restrictions][Drought Mitigation Policies]") {
        CHECK(drought_mitigation_policies[3]->id == 3);
        vector<int> expected_utilities_ids = {1, 2, 0};
        CHECK(dynamic_cast<Transfers *>(drought_mitigation_policies[3])->getUtilities_ids() ==
              expected_utilities_ids);
        int n_utils = 3;
        vector<vector<int>> expected_Aeq = {{-1, -1, 0,  1, 0,  0,},
                                            {0,  1,  1,  0, -1, 0,},
                                            {1,  0,  -1, 0, 0,  -1}};
        auto Aeq = dynamic_cast<Transfers *>(drought_mitigation_policies[3])->getAeq();
        for (int i = 0; i < n_utils; ++i) {
            for (int j = 0; j < 2 * n_utils; ++j) {
                CHECK(Aeq[i][j] == expected_Aeq[i][j]);
            }
        }

        auto ub = dynamic_cast<Transfers *>(drought_mitigation_policies[3])->getUb();
        CHECK(ub[0] == 140);
        CHECK(ub[1] == 140);
        CHECK(ub[2] == 105);
    }
}

TEST_CASE("Checking if reservoir control rules are imported properly.",
          "[Input File Parser][Reservoir Control Rules]") {
    MasterSystemInputFileParser parser;
    parser.preloadAndCheckInputFile(input_test_file);
    parser.createSystemObjects(new double);
    const auto &reservoir_control_rules = parser.getReservoirControlRules();
    auto nrr_control = dynamic_cast<FixedMinEnvFlowControl *>(reservoir_control_rules[0]);
    CHECK(nrr_control->water_source_id == 5);
    CHECK(nrr_control->getRelease(0) == 0);
    auto gq_control = dynamic_cast<FixedMinEnvFlowControl *>(reservoir_control_rules[1]);
    CHECK(gq_control->water_source_id == 1);
    CHECK(gq_control->getRelease(0) == 0);
    auto crr_control = dynamic_cast<FixedMinEnvFlowControl *>(reservoir_control_rules[2]);
    CHECK(crr_control->water_source_id == 0);
    CHECK(crr_control->getRelease(0) == 0);

    auto scr_control = dynamic_cast<InflowMinEnvFlowControl *>(reservoir_control_rules[3]);
    vector<double> sugar_creek_inflows = {1.0, 3.5, 7.0, 10.5, 12.579};
    vector<double> sugar_creek_releases = {1.0, 3.5, 7.0, 10.5, 12.579};
    CHECK(scr_control->water_source_id == 6);
    CHECK(scr_control->getInflows() == sugar_creek_inflows);
    CHECK(scr_control->getReleases() == sugar_creek_releases);
    auto al_control = dynamic_cast<SeasonalMinEnvFlowControl *>(reservoir_control_rules[4]);
    vector<int> autumn_controls_weeks = {0, 13, 43, 53};
    vector<double> autumn_releases = {357, 490, 357};
    CHECK(al_control->water_source_id == 3);
    CHECK(al_control->getWeekThresholds() == autumn_controls_weeks);
    CHECK(al_control->getMinEnvFlows() == autumn_releases);
}
