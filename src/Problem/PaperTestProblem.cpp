//
// Created by D.Gold on 8/28/18.
//

#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>
#include <fstream>
#include <omp.h>
#ifdef  PARALLEL
#include <mpi.h>
#endif


#include "PaperTestProblem.h"
#include "../Controls/SeasonalMinEnvFlowControl.h"
#include "../Controls/Custom/FallsLakeMinEnvFlowControl.h"
#include "../Controls/StorageMinEnvFlowControl.h"
#include "../Controls/InflowMinEnvFlowControl.h"
#include "../Controls/FixedMinEnvFlowControl.h"
#include "../Controls/Custom/JordanLakeMinEnvFlowControl.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../SystemComponents/WaterSources/Quarry.h"
#include "../SystemComponents/WaterSources/Relocation.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "../DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "../SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../Simulation/Simulation.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/ReservoirExpansion.h"


/**
 * Runs three utility carolina problem for demonstration paper
 * @param vars
 * @param n_realizations
 * @param n_weeks
 * @param sol_number
 * @param output_directory
 * @todo test and edit
 */

int PaperTestProblem::functionEvaluation(double *vars, double *objs, double *consts) {

    // ===================== SET UP DECISION VARIABLES  =====================

    //FIXME why do we make a null pointer here?
    Simulation *s = nullptr;

    //FIXME THESE ARE TEST VALUES
    double watertown_LMA = 0.7;
    double dryville_LMA = 0.20;
    double fallsland_LMA = 0.1;
    double watertown_annual_payment = 0.05;
    double watertown_demand_buffer = 1.0;
    double dryville_annual_payment = 0.05;
    double dryville_demand_buffer = 1.0;
    double fallsland_annual_payment = 0.05;
    double fallsland_demand_buffer = 1.0;
    double watertown_restriction_trigger = 10.05;
    double dryville_restriction_trigger = 10.05;
    double fallsland_restriction_trigger = 10.05;
    double dryville_transfer_trigger = 10.03;
    double fallsland_transfer_trigger = 10.03;
    double watertown_insurance_use = .15; //FIXME: CHECK THAT THIS IS RESTRICTION STAGE
    double dryville_insurance_use = .15;
    double fallsland_insurance_use = .15;
    double waterland_insurance_payment = 0.15;
    double dryville_insurance_payment = 0.15;
    double fallsland_insurance_payment = 0.15;




    if (utilities_rdm.empty()) {
        /// All matrices below have dimensions n_realizations x nr_rdm_factors
        utilities_rdm = std::vector<vector<double>>(
                n_realizations, vector<double>(4, 1.));
        water_sources_rdm = std::vector<vector<double>>(
                n_realizations, vector<double>(51, 1.));
        policies_rdm = std::vector<vector<double>>(
                n_realizations, vector<double>(4, 1.));
    }


    // ===================== SET UP PROBLEM COMPONENTS =====================
//Beginning with Reservoir continuity

    int streamflow_n_weeks = 52 * (70 + 50);

    //FIXME: Come back and sure these get loaded and are properly matched to res
    EvaporationSeries evaporation_durham(&evap_durham, streamflow_n_weeks); //Evaporation
    EvaporationSeries evaporation_falls_lake(&evap_falls_lake, streamflow_n_weeks); //Evaporation
    EvaporationSeries evaporation_jordan_lake(&evap_jordan_lake, streamflow_n_weeks); // Lake Michael
    EvaporationSeries evaporation_owasa(&evap_owasa, streamflow_n_weeks);

    // Create catchments and corresponding vectors

    // Autumn Lake (abstracted Neuse River Basin)
    Catchment durham_inflows(&streamflows_durham, streamflow_n_weeks); // use Durham inflows for half the inflows
    Catchment lower_flat_river(&streamflows_flat, streamflow_n_weeks);
    Catchment swift_creek(&streamflows_swift, streamflow_n_weeks);


    // Add catchments to vector
    vector<Catchment *> catchment_autumn;
    catchment_autumn.push_back(&durham_inflows);
    catchment_autumn.push_back(&lower_flat_river);
    //catchment_autumn.push_back(&little_river_raleigh);
    catchment_autumn.push_back(&swift_creek);

    // College Rock Reservoir Catchment (abstracted from upper cape fear)
    Catchment phils_creek(&streamflows_phils, streamflow_n_weeks);
    Catchment morgan_creek(&streamflows_morgan, streamflow_n_weeks);

    // Add catchments to vector
    vector<Catchment *> catchment_college_rock;
    catchment_college_rock.push_back(&phils_creek);
    catchment_college_rock.push_back(&morgan_creek);

    // Sugar Creek (abstracted Cane Creek)
    Catchment sugar_creek(&streamflows_cane, streamflow_n_weeks);

    // Add catchment to vector
    vector<Catchment *> catchment_sugar_creek;
    catchment_sugar_creek.push_back(&sugar_creek);

    // Lake Michael Catchment (abstracted lower cape fear)
    Catchment lower_haw_river(&streamflows_haw, streamflow_n_weeks);

    // Add catchment to vector
    vector<Catchment *> catchment_lower_haw_river;
    catchment_lower_haw_river.push_back(&lower_flat_river);

    // New River Reservoir catchment
    Catchment little_river_raleigh(&streamflows_llr, streamflow_n_weeks);

    vector<Catchment *> catchment_new_river;
    catchment_new_river.push_back(&little_river_raleigh);

    // Downstream Gage
    Catchment cape_fear_river_at_lillington(&streamflows_lillington, streamflow_n_weeks);

    // Add catchment to vector
    vector<Catchment *> gage_lillington;
    gage_lillington.push_back(&cape_fear_river_at_lillington);

    // Create storage vs. area reservoir curves
    //FIXME DISCUSS WITH BERNARDO HOW THIS WORKS TO MAKE SURE CORRECT, WHY DO ONLY SOME RES HAVE THESE CURVES?

    // CURRENTLY THIS IS THE STORAGE OF FALLS + DURHAM (MICHIE AND LR) + WB
    // ASSUMING DURHAM AND WB SCALE PROPORTIONALLY TO FALLS
    vector<double> autumn_lake_storage = {0, (23266 + 4257 + 1869) * table_gen_storage_multiplier,
                                          (34700 + 6349 + 2790) * table_gen_storage_multiplier};
    vector<double> autumn_lake_area = {0.32 * 5734, 0.32 * 29000, 0.28 * 40434};
    DataSeries autumn_lake_storage_area(&autumn_lake_storage, &autumn_lake_area);

    vector<double> new_river_res_storage = {0, 3700};
    vector<double> new_river_res_area = {0, 0.3675 * 3700};
    DataSeries new_river_storage_area(&new_river_res_storage,
                                      &new_river_res_area);

    vector<double> sugar_creek_res_storage = {0, 2909};
    vector<double> sugar_creek_res_area = {0, 0.3675 * 2909};
    DataSeries sugar_creek_storage_area(&sugar_creek_res_storage,
                                        &sugar_creek_res_area);

    // Create minimum environmental flow rules (controls)
    // Autumn is combining Falls+Durham+WB
    //FIXME FIX THESE FLOW REQUIREMENTS
    vector<int> autumn_controls_weeks = {0, 13, 43, 53};
    vector<double> autumn_releases = {(39 + 10 + 2) * 7, (65 + 4 + 1) * 7, (39 + 10 + 2) * 7};

    SeasonalMinEnvFlowControl autumn_min_env_control(2, autumn_controls_weeks, autumn_releases);

    // Lake Michael is based off the Jordan Lake and uses its class
    JordanLakeMinEnvFlowControl lake_michael_min_env_control( 1,
            cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
            290.84, 387.79, 30825.0 * table_gen_storage_multiplier, 10300.0 * table_gen_storage_multiplier);

    //FIXME SUGAR CREEK BASED ON CCR, SO LEAVING AS IS, IS THIS A GOOD IDEA?
    vector<double> sugar_creek_inflows = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                          1.797 * 7};
    vector<double> sugar_creek_releases = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                           1.797 * 7};

    InflowMinEnvFlowControl sugar_creek_min_env_control(3,
                                                        sugar_creek_inflows,
                                                        sugar_creek_releases);

    // College Rock has no min flow
    FixedMinEnvFlowControl college_rock_min_env_control(0, 0);

    //FIXME made these numbers up
    vector<int> new_river_controls_weeks = {0, 13, 43, 53};
    vector<double> new_river_releases = {3 * 7, 8 * 7, 3 * 7};

    SeasonalMinEnvFlowControl new_river_min_env_control(4, new_river_controls_weeks, new_river_releases);

    vector<MinEnvFlowControl *> min_env_flow_controls;
    min_env_flow_controls.push_back(&autumn_min_env_control);
    min_env_flow_controls.push_back(&lake_michael_min_env_control);
    min_env_flow_controls.push_back(&sugar_creek_min_env_control);
    min_env_flow_controls.push_back(&college_rock_min_env_control);

    // Lake Michael parameters

    double lake_michael_supply_capacity = 10300 * table_gen_storage_multiplier; // reduced to .69 of JL cap
    double lake_michael_wq_capacity = 30825.0 * table_gen_storage_multiplier;
    double lake_michael_storage_capacity = lake_michael_wq_capacity + lake_michael_supply_capacity;
    vector<int> lake_michael_allocations_ids = {0, 1, 2, WATER_QUALITY_ALLOCATION};
    vector<double> lake_michael_allocation_fractions = {
            watertown_LMA * lake_michael_supply_capacity / lake_michael_storage_capacity,
            dryville_LMA * lake_michael_supply_capacity / lake_michael_storage_capacity,
            fallsland_LMA * lake_michael_supply_capacity / lake_michael_storage_capacity,
            lake_michael_wq_capacity / lake_michael_storage_capacity};
    vector<double> lake_michael_treatment_allocation_fractions = {1., 0.0, 0.0};

    // Autumn Lake parameters
    // WB and Durham storage added to water supply capacity
    double autumn_lake_supply_capacity = (14700.0 + 6349 + 2790) * table_gen_storage_multiplier;
    double autumn_lake_wq_capacity = 20000.0 * table_gen_storage_multiplier;
    double autumn_lake_storage_capacity = autumn_lake_wq_capacity + autumn_lake_supply_capacity;
    //FIXME: can reallocate to make more interesting
    vector<int> autumn_lake_allocations_ids = {1, 2, WATER_QUALITY_ALLOCATION};
    vector<double> autumn_lake_allocation_fractions = {
            0.29 * autumn_lake_supply_capacity / autumn_lake_storage_capacity,
            0.71 * autumn_lake_supply_capacity / autumn_lake_storage_capacity,
            autumn_lake_wq_capacity / autumn_lake_storage_capacity};
    vector<double> autumn_lake_treatment_allocation_fractions = {0.38, 0.62};

// Create existing reservoirs
    /// combined university lake and stone quarry
    Reservoir college_rock_reservoir("College Rock Reservoir",
                                     0,
                                     catchment_college_rock,
                                     (449 + 200) * table_gen_storage_multiplier,
                                     ILLIMITED_TREATMENT_CAPACITY,
                                     evaporation_owasa,
                                     222);

    AllocatedReservoir lake_michael("Lake Michael",
                                    1,
                                    catchment_lower_haw_river,
                                    lake_michael_storage_capacity,
                                    448,
                                    evaporation_jordan_lake,
                                    13940,
                                    &lake_michael_allocations_ids,
                                    &lake_michael_allocation_fractions,
                                    &lake_michael_treatment_allocation_fractions);

    AllocatedReservoir autumn_lake("Autumn Lake",
                                   2,
                                   catchment_autumn,
                                   autumn_lake_storage_capacity,
                                   ILLIMITED_TREATMENT_CAPACITY,
                                   evaporation_falls_lake,
                                   &autumn_lake_storage_area,
                                   &autumn_lake_allocations_ids,
                                   &autumn_lake_allocation_fractions,
                                   &autumn_lake_treatment_allocation_fractions);

    // Create potential sources

    //FIXME ORIGINAL CODE SETS WEEKS_IN_YEAR TO 0 HERE
    vector<double> construction_time_interval = {3.0, 5.0};

    LevelDebtServiceBond sugar_bond(3, 263.0, 25, 0.05, vector<int>(1, 0));
    Reservoir sugar_creek_reservoir("Sugar Creek Reservoir",
                                    3,
                                    catchment_sugar_creek,
                                    2909,
                                    ILLIMITED_TREATMENT_CAPACITY,
                                    evaporation_owasa,
                                    &sugar_creek_storage_area,
                                    construction_time_interval,
                                    17 * WEEKS_IN_YEAR,
                                    sugar_bond);

    //FIXME check bond, this one is from little river raliegh
    LevelDebtServiceBond new_river_bond(4, 263.0, 25, 0.05, vector<int>(1, 0));
    Reservoir new_river_reservoir("New River Reservoir",
                                  4,
                                  catchment_new_river,
                                  3700.0,
                                  ILLIMITED_TREATMENT_CAPACITY,
                                  evaporation_falls_lake,
                                  &new_river_storage_area,
                                  construction_time_interval,
                                  17 * WEEKS_IN_YEAR,
                                  new_river_bond);

    LevelDebtServiceBond dummy_bond(5, 1., 1, 1., vector<int>(1, 0));
    Reservoir dummy_endpoint("Dummy Node", 5, vector<Catchment *>(), 1., 0, evaporation_durham, 1,
                             construction_time_interval, 0, dummy_bond);

    //FIXME: Edit the expansion volumes for CRR, just made these up
    vector<double> college_rock_expansion_low_construction_time = {3, 5};
    LevelDebtServiceBond college_rock_expansion_low_bond(6, 50, 30, .05, vector<int>(1, 0));
    ReservoirExpansion college_rock_expansion_low((char *) "College Rock Expansion Low", 6, 0, 500,
                                                  college_rock_expansion_low_construction_time, 5, college_rock_expansion_low_bond);

    vector<double> college_rock_expansion_high_construction_time = {3, 5};
    LevelDebtServiceBond college_rock_expansion_high_bond(7, 100, 30, .05, vector<int>(1, 0));
    ReservoirExpansion college_rock_expansion_high((char *) "College Rock Expansion High", 7, 0, 1000,
                                                   college_rock_expansion_high_construction_time, 5,
                                                   college_rock_expansion_high_bond);

    vector<double> watertown_reuse_construction_time = {3, 5};
    LevelDebtServiceBond watertown_reuse_bond(8, 50, 30, .05, vector<int>(1, 0));
    WaterReuse watertown_reuse((char *) "Watertwon Reuse", 8, 20, watertown_reuse_construction_time, 5,
                               watertown_reuse_bond);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&college_rock_reservoir);
    water_sources.push_back(&autumn_lake);
    water_sources.push_back(&lake_michael);
    water_sources.push_back(&new_river_reservoir);
    water_sources.push_back(&sugar_creek_reservoir);
    water_sources.push_back(&college_rock_expansion_low);
    water_sources.push_back(&college_rock_expansion_high);
    water_sources.push_back(&watertown_reuse);
    water_sources.push_back(&dummy_endpoint);

/*
 *
 *  0 College Rock Reservoir (6) small expansion (7) large expansion
 *   \
 *    \
 *     \
 *      \                      (3) Sugar Creek Reservoir
 *       1 Lake Michael        /
 *        \                   /
 *         \                 2 Autumn Lake
 *          \               /
 *    Lillington           /
 *            \           /
 *             \         /
 *              \       /
 *               \    (4) New River Reservoir
 *                \   /
 *                 \ /    (8) watertown reuse
 *                  |
 *                  |
 *                  5 Dummy Endpoint
 */

    //FIXME ADD CRR EXPANSION
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(3, 2);
    g.addEdge(2, 4);
    g.addEdge(1, 5);
    g.addEdge(4, 5);

    auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

    vector<double> bond_term = {25, 25, 25, 25};
    vector<double> bond_rate = {0.05, 0.05, 0.05, 0.05};
    double discount_rate = 0.05;

    //FIXME make return flows after utilities are created?
    vector<int> watertown_ws_return_id;
    vector<vector<double>> watertown_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_watertown(
            watertown_discharge_fraction_series,
            watertown_ws_return_id);

    vector<int> dryville_ws_return_id;
    vector<vector<double>> dryville_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_dryville(
            dryville_discharge_fraction_series,
            dryville_ws_return_id);

    vector<int> fallsland_ws_return_id;
    vector<vector<double>> fallsland_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_fallsland(
            fallsland_discharge_fraction_series,
            fallsland_ws_return_id);

    //FIXME: rof triggered order needs to be changed to DV, bond etc need to be updated
    vector<int> rof_triggered_infra_order_watertown = {4, 6, 7, 8};
    Utility watertown((char *) "Watertown", 0, demand_watertown, demand_n_weeks, watertown_annual_payment,
                      &watertownDemandClassesFractions,
                      &watertownUserClassesWaterPrices, wwtp_discharge_watertown, watertown_demand_buffer,
                      rof_triggered_infra_order_watertown, vector<int>(), vector<double>(1, 0.01), discount_rate, 30, .05);

    //FIXME: rof triggered order needs to be changed to DV, bond etc need to be updated
    vector<int> rof_triggered_infra_order_dryville = {3};
    Utility dryville((char *) "Dryville", 1, demand_dryville, demand_n_weeks, dryville_annual_payment,
                     &dryvilleDemandClassesFractions, &dryvilleUserClassesWaterPrices,
                     wwtp_discharge_dryville, dryville_demand_buffer, rof_triggered_infra_order_dryville, vector<int>(),
                     vector<double>(1, 0.01), discount_rate, 30, 0.05);

    //FIXME: rof triggered order needs to be changed to DV, bond etc need to be updated
    vector<int> rof_triggered_infra_order_fallsland = {4};
    Utility fallsland((char *) "Fallsland", 2, demand_fallsland, demand_n_weeks, fallsland_annual_payment,
                      &fallslandDemandClassesFractions, &fallslandUserClassesWaterPrices,
                      wwtp_discharge_fallsland, fallsland_demand_buffer, rof_triggered_infra_order_fallsland,
                      vector<int>(), vector<double>(1, 0.01), discount_rate, 30, 0.05);

    vector<Utility*> utilities;
    utilities.push_back(&watertown);
    utilities.push_back(&dryville);
    utilities.push_back(&fallsland);

    //FIXME CHECK TO MAKE SURE PROPER RESERVOIRS CONNECTED
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 1, 2, 4, 6, 7, 8}, //Watertown
            {2, 3}, //Dryville
            {2, 4} //Fallsland
    };

    //FIXME: table_storage_shift ??
    auto table_storage_shift = vector<vector<double>>(3, vector<double>(water_sources.size() + 1, 0.));

    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    vector<double> initial_restriction_triggers = {watertown_restriction_trigger,
                                                   dryville_restriction_trigger,
                                                   fallsland_restriction_trigger};

    vector<double> restriction_stage_multipliers_watertown = {0.9, 0.8, 0.7, 0.6};
    vector<double> restriction_stage_triggers_watertown = {initial_restriction_triggers[0],
                                                           initial_restriction_triggers[0] + 0.15f,
                                                           initial_restriction_triggers[0] + 0.35f,
                                                           initial_restriction_triggers[0] + 0.6f};

    vector<double> restriction_stage_multipliers_dryville = {0.9, 0.8, 0.7, 0.6};
    vector<double> restriction_stage_triggers_dryville = {initial_restriction_triggers[1],
                                                          initial_restriction_triggers[1] + 0.15f,
                                                          initial_restriction_triggers[1] + 0.35f,
                                                          initial_restriction_triggers[1] + 0.6f};

    vector<double> restriction_stage_multipliers_fallsland = {0.9, 0.8, 0.7, 0.6};
    vector<double> restriction_stage_triggers_fallsland = {initial_restriction_triggers[2],
                                                           initial_restriction_triggers[2] + 0.15f,
                                                           initial_restriction_triggers[2] + 0.35f,
                                                           initial_restriction_triggers[2] + 0.6f};

    Restrictions restrictions_w(0,
                                restriction_stage_multipliers_watertown,
                                restriction_stage_triggers_watertown);

    Restrictions restrictions_d(1,
                                restriction_stage_multipliers_dryville,
                                restriction_stage_triggers_dryville);

    Restrictions restrictions_f(2,
                                restriction_stage_multipliers_fallsland,
                                restriction_stage_triggers_fallsland);

    drought_mitigation_policies = {&restrictions_w, &restrictions_d, &restrictions_f};


    /// Transfer policy

    /*
     *      0
     *     / \
     *  0 v   v 1
     *   /     \
     *  1--> <--2
     *      2
     */

    vector<int> buyers_ids = {1, 2};

    //FIXME: TEST VALUES, MAY WANT TO EDIT
    vector<double> buyer_transfer_capacities = {10.0*7, 10.0*7, 10.0*7};

    vector<double> buyer_transfer_triggers = {dryville_transfer_trigger,
                                              fallsland_transfer_trigger};

    Graph ug(3);
    ug.addEdge(0,1);
    ug.addEdge(0,2);
    ug.addEdge(1,2);

    Transfers t(3, 0, 1, 35,
                buyers_ids,
                buyer_transfer_capacities,
                buyer_transfer_triggers,
                ug,
                vector<double>(),
                vector<int>());
    drought_mitigation_policies.push_back(&t);


    /// Set up insurance
    vector<double> insurance_triggers = {watertown_insurance_use,
                                         dryville_insurance_use,
                                         fallsland_insurance_use};
    vector<double> fixed_payouts = {waterland_insurance_payment,
                                    dryville_insurance_payment,
                                    fallsland_insurance_payment};

    vector<int> insured_utilities = {0, 1, 2};

    InsuranceStorageToROF in(4, water_sources, g, reservoir_utility_connectivity_matrix, utilities,
                             min_env_flow_controls, utilities_rdm, water_sources_rdm, insurance_triggers, 1.2,
                             fixed_payouts, n_weeks);

    drought_mitigation_policies.push_back(&in);


    /// Creates simulation object depending on use (or lack thereof) ROF tables
    double realization_start;
    if (import_export_rof_tables == EXPORT_ROF_TABLES) {
        s = new Simulation(water_sources,
                           g,
                           reservoir_utility_connectivity_matrix,
                           utilities,
                           drought_mitigation_policies,
                           min_env_flow_controls,
                           utilities_rdm,
                           water_sources_rdm,
                           policies_rdm,
                           n_weeks,
                           realizations_to_run,
                           rof_tables_directory);
//        realization_start = omp_get_wtime();
        this->master_data_collector = s->runFullSimulation(n_threads);
    } else if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        s = new Simulation (water_sources,
                            g,
                            reservoir_utility_connectivity_matrix,
                            utilities,
                            drought_mitigation_policies,
                            min_env_flow_controls,
                            utilities_rdm,
                            water_sources_rdm,
                            policies_rdm,
                            n_weeks,
                            realizations_to_run,
                            rof_tables,
                            table_storage_shift,
                            rof_tables_directory);
//        realization_start = omp_get_wtime();
        this->master_data_collector = s->runFullSimulation(n_threads);
    } else {
        s = new Simulation(water_sources,
                           g,
                           reservoir_utility_connectivity_matrix,
                           utilities,
                           drought_mitigation_policies,
                           min_env_flow_controls,
                           utilities_rdm,
                           water_sources_rdm,
                           policies_rdm,
                           n_weeks,
                           realizations_to_run);
//        realization_start = omp_get_wtime();
        this->master_data_collector = s->runFullSimulation(n_threads);
    }

//    double realization_end = omp_get_wtime();
//    std::cout << "Simulation took  " << realization_end - realization_start
//              << "s" << std::endl;

    /// Calculate objectives and store them in Borg decision variables array.
#ifdef  PARALLEL
    objectives = calculateAndPrintObjectives(false);

        int i = 0;
        objs[i] = 1. - min(min(objectives[i], objectives[5 + i]),
        		   min(objectives[10 + i], objectives[15 + i]));
        for (i = 1; i < 5; ++i) {
            objs[i] = max(max(objectives[i], objectives[5 + i]),
      	                  max(objectives[10 + i], objectives[15 + i]));
        }

        objs[5] = OWASA_JLA + Durham_JLA + Cary_JLA + Raleigh_JLA;

        objectives.push_back(objs[5]);

        if (s != nullptr) {
            delete s;
	}
	s = nullptr;
#endif
//    } catch (const std::exception& e) {
//        simulationExceptionHander(e, s, objs, vars);
//	return 1;
//    }

    delete s;

    return 0;

};



PaperTestProblem::~PaperTestProblem() = default;

void PaperTestProblem::readInputData() {
    cout << "Reading input data." << endl;

#pragma omp parallel num_threads(20)
    {
#pragma omp single
        streamflows_durham = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/durham_inflows.csv", n_realizations);
#pragma omp single
        streamflows_flat = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/falls_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_swift = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/lake_wb_inflows.csv", n_realizations);
#pragma omp single
        streamflows_llr = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/little_river_raleigh_inflows.csv", n_realizations);
        // }
#pragma omp single
        streamflows_phils = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/stone_quarry_inflows.csv", n_realizations);
#pragma omp single
        streamflows_cane = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/cane_creek_inflows.csv", n_realizations);
#pragma omp single
        streamflows_morgan = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/university_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_haw = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/jordan_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_lillington = Utils::parse2DCsvFile("../TestFiles/inflows" + evap_inflows_suffix + "/lillington_inflows.csv", n_realizations);
// };
        //cout << "Reading evaporations." << endl;
#pragma omp single
        evap_durham = Utils::parse2DCsvFile("../TestFiles/evaporation" + evap_inflows_suffix + "/durham_evap.csv", n_realizations);
#pragma omp single
        evap_falls_lake = Utils::parse2DCsvFile("../TestFiles/evaporation" + evap_inflows_suffix + "/falls_lake_evap.csv", n_realizations);
#pragma omp single
        evap_owasa = Utils::parse2DCsvFile("../TestFiles/evaporation" + evap_inflows_suffix + "/owasa_evap.csv", n_realizations);
#pragma omp single
        evap_little_river = Utils::parse2DCsvFile("../TestFiles/evaporation" + evap_inflows_suffix + "/little_river_raleigh_evap.csv", n_realizations);
#pragma omp single
        {
            evap_wheeler_benson = Utils::parse2DCsvFile("../TestFiles/evaporation" + evap_inflows_suffix + "/wb_evap.csv", n_realizations);
            evap_jordan_lake = evap_owasa;
        }

        //cout << "Reading demands." << endl;
#pragma omp single
        demand_watertown = Utils::parse2DCsvFile("../TestFiles/demands" + evap_inflows_suffix + "/cary_demand.csv", n_realizations);
#pragma omp single
        demand_dryville = Utils::parse2DCsvFile("../TestFiles/demands" + evap_inflows_suffix + "/durham_demand.csv", n_realizations);
#pragma omp single
        demand_fallsland = Utils::parse2DCsvFile("../TestFiles/demands" + evap_inflows_suffix + "/raleigh_demand.csv", n_realizations);

        //cout << "Reading others." << endl;
#pragma omp single
        {
            demand_to_wastewater_fraction_fallsland = Utils::parse2DCsvFile("../TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");
            demand_to_wastewater_fraction_dryville = Utils::parse2DCsvFile("../TestFiles/demand_to_wastewater_fraction_durham.csv");

            watertownDemandClassesFractions = Utils::parse2DCsvFile("../TestFiles/caryDemandClassesFractions.csv");
            dryvilleDemandClassesFractions = Utils::parse2DCsvFile("../TestFiles/durhamDemandClassesFractions.csv");
            fallslandDemandClassesFractions = Utils::parse2DCsvFile("../TestFiles/raleighDemandClassesFractions.csv");

            watertownUserClassesWaterPrices = Utils::parse2DCsvFile("../TestFiles/caryUserClassesWaterPrices.csv");
            dryvilleUserClassesWaterPrices = Utils::parse2DCsvFile("../TestFiles/durhamUserClassesWaterPrices.csv");
            fallslandUserClassesWaterPrices = Utils::parse2DCsvFile("../TestFiles/raleighUserClassesWaterPrices.csv");
        }
//    cout << "Done reading input data." << endl;
    }

}


int PaperTestProblem::simulationExceptionHander(const std::exception &e, Simulation *s,
                                                double *objs, const double *vars) {
    int num_dec_var = 57;
//        printf("Exception called during calculations. Decision variables are below:\n");
    ofstream sol;
    int world_rank;
#ifdef  PARALLEL
    int mpi_initialized;
	MPI_Initialized(&mpi_initialized);
	if (mpi_initialized)
            MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	else
	    world_rank = 0;
#else
    world_rank = 0;
#endif
    string error_file = "sol_error_rank_" + to_string(world_rank) + ".csv";
    sol.open(error_file.c_str());
    for (int i = 0; i < num_dec_var; ++i) {
        sol << vars[i] << ",";
    }
    sol << flush;
    sol.close();
    printf("Error. Decision variables printed in %s\n", error_file.c_str());

#ifdef PARALLEL
    objs[0] = 0.;
	objs[1] = 1.1;
	objs[2] = 1000;
	objs[3] = 5.;
	objs[4] = 5.;
	objs[5] = 1.1;
	if (s != nullptr) {
	    delete s;
	    s = nullptr;
	}
#else
    Utils::print_exception(e);
#endif
    return 1;
}



PaperTestProblem::PaperTestProblem(unsigned long n_weeks, int import_export_rof_table)
        : Problem(n_weeks), import_export_rof_tables(import_export_rof_table) {
    if (import_export_rof_table == EXPORT_ROF_TABLES) {
        table_gen_storage_multiplier = BASE_STORAGE_CAPACITY_MULTIPLIER;
    } else {
        table_gen_storage_multiplier = 1.;
    }
}

/**
 * Read pre-computed ROF tables.
 * @param folder Folder containing the ROF tables.
 * @param n_realizations number of realizations.
 */
void
PaperTestProblem::setRofTables(unsigned long n_realizations, int n_utilities, string rof_tables_directory) {

    //double start_time = omp_get_wtime();
    cout << "Loading ROF tables" << endl;
    int n_tiers = NO_OF_INSURANCE_STORAGE_TIERS + 1;

    /// Get number of weeks in tables
    string file_name = rof_tables_directory + "/tables_r" + to_string(0) + "_u" + to_string(0);
    ifstream in(file_name, ios_base::binary);
    if (!in.good()) {
        string error_table_file = "Tables file not found: " + file_name;
        __throw_invalid_argument(error_table_file.c_str());
    }

    unsigned n_weeks_in_table;
    in.read(reinterpret_cast<char *>(&n_weeks_in_table), sizeof(unsigned));

    /// Create empty tables
    rof_tables = vector<vector<Matrix2D<double>>>(
            n_realizations,
            vector<Matrix2D<double>>((unsigned long) n_utilities,
                                     Matrix2D<double>(n_weeks_in_table / n_tiers, n_tiers)));

    this->rof_tables_directory = rof_tables_directory;

    /// Load ROF tables
    for (unsigned long r = 0; r < n_realizations; ++r) {
        for (int u = 0; u < n_utilities; ++u) {
            string file_name = rof_tables_directory + "/tables_r" + to_string(r) + "_u" + to_string(u);
            ifstream in(file_name, ios_base::binary);
            if (!in.good()) {
                string error_table_file = "Tables file not found: " + file_name;
                __throw_invalid_argument(error_table_file.c_str());
            }

            /// Get table file size from table files.
            unsigned stringsize;
            in.read(reinterpret_cast<char *>(&stringsize), sizeof(unsigned));

            /// Get table information from table files.
            double data[stringsize];
            in.read(reinterpret_cast<char *>(&data),
                    stringsize * sizeof(double));

            /// Create tables based on table files.
            rof_tables[r][u].setData(data, (int) stringsize);

            for (unsigned long i = 0; i < stringsize; ++i) {
                double d = data[i];
                if (std::isnan(d) || d > 1.01 || d < 0) {
                    string error_m = "nan or out of [0,1] rof imported "
                                     "tables. Realization " +
                                     to_string(r) + "\n";
                    printf("%s", error_m.c_str());
                    __throw_logic_error(error_m.c_str());
                }
            }

            in.close();
        }
    }

    //printf("Loading tables took %f time.\n", omp_get_wtime() - start_time);
}



void PaperTestProblem::setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, string rof_tables_directory) {
    if (std::abs(import_export_rof_tables) > 1)
        __throw_invalid_argument("Import/export ROF tables can be assigned as:\n"
                                 "-1 - import tables\n"
                                 "0 - ignore tables\n"
                                 "1 - export tables.\n"
                                 "The value entered is invalid.");
    PaperTestProblem::import_export_rof_tables = import_export_rof_tables;
//    this->rof_tables_directory = output_directory + "/TestFiles/" + rof_tables_directory;
    this->rof_tables_directory = rof_tables_directory;

    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        PaperTestProblem::setRofTables(n_realizations, n_utilities, this->rof_tables_directory);
    } else {
        const string mkdir_command = "mkdir";
        system((mkdir_command + " " + this->rof_tables_directory).c_str());
    }
}