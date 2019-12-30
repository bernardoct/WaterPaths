//
// Created by D.Gold on 8/28/18.
//

#include <algorithm>
#include <iostream>
#include <omp.h>
#include "PaperTestProblem.h"
#include "../Controls/SeasonalMinEnvFlowControl.h"
#include "../Controls/InflowMinEnvFlowControl.h"
#include "../Controls/FixedMinEnvFlowControl.h"
#include "../Controls/Custom/JordanLakeMinEnvFlowControl.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "../DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "../SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../Simulation/Simulation.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"

#ifdef PARALLEL
#include <mpi.h>

void PaperTestProblem::setProblemDefinition(BORG_Problem &problem)
{
    // The parameter bounds are the same for all formulations

    BORG_Problem_set_bounds(problem, 0, 0.001, 1.0);    // watertown restrictions
    BORG_Problem_set_bounds(problem, 1, 0.001, 1.0);    // dryville restrictions
    BORG_Problem_set_bounds(problem, 2, 0.001, 1.0);    // fallsland restrictions
    BORG_Problem_set_bounds(problem, 3, 0.001, 1.0);    // dryville transfer
    BORG_Problem_set_bounds(problem, 4, 0.001, 1.0);    // fallsland transfer
    BORG_Problem_set_bounds(problem, 5, 0.334, 0.9);    // watertown LMA
    BORG_Problem_set_bounds(problem, 6, 0.05, 0.333);   // dryville LMA
    BORG_Problem_set_bounds(problem, 7, 0.05, 0.333);   // fallsland LMA
    BORG_Problem_set_bounds(problem, 8, 0.0, 0.1);      // watertown annual payment
    BORG_Problem_set_bounds(problem, 9, 0.0, 0.1);      // dryville annual payment
    BORG_Problem_set_bounds(problem, 10, 0.0, 0.1);     // fallsland annual payment
    BORG_Problem_set_bounds(problem, 11, 0.001, 1.0);   // watertown insurance use
    BORG_Problem_set_bounds(problem, 12, 0.001, 1.0);   // dryville insurance use
    BORG_Problem_set_bounds(problem, 13, 0.001, 1.0);   // fallsland insurance use
    BORG_Problem_set_bounds(problem, 14, 0.0, 0.02);    // watertown insurance payment
    BORG_Problem_set_bounds(problem, 15, 0.0, 0.02);    // dryville insurance payment
    BORG_Problem_set_bounds(problem, 16, 0.0, 0.02);    // fallsland insurance payment
    BORG_Problem_set_bounds(problem, 17, 0.001, 1.0);   // watertown inf trigger
    BORG_Problem_set_bounds(problem, 18, 0.001, 1.0);   // dryville inf trigger
    BORG_Problem_set_bounds(problem, 19, 0.001, 1.0);   // fallsland inf trigger
    BORG_Problem_set_bounds(problem, 20, 0.0, 1.0);     // new river rank watertown
    BORG_Problem_set_bounds(problem, 21, 0.0, 1.0);     // college rock expansion rank low
    BORG_Problem_set_bounds(problem, 22, 0.0, 1.0);     // college rock expansion rank high
    BORG_Problem_set_bounds(problem, 23, 0.0, 1.0);     // watertown reuse I rank
    BORG_Problem_set_bounds(problem, 24, 0.0, 1.0);     // sugar creek rank
    BORG_Problem_set_bounds(problem, 25, 0.0, 1.0);     // granite quarry rank
    BORG_Problem_set_bounds(problem, 26, 0.0, 1.0);     // new river rank fallsland
    BORG_Problem_set_bounds(problem, 27, 0.0, 1.0);     // watertown reuse II rank
    BORG_Problem_set_bounds(problem, 28, 0.0, 1.0);     // watertown reuse II rank
    BORG_Problem_set_bounds(problem, 29, 0.0, 1.0);     // watertown reuse II rank

    // Set epsilons for objectives

    BORG_Problem_set_epsilon(problem, 0, 0.001);
    BORG_Problem_set_epsilon(problem, 1, 0.02);
    BORG_Problem_set_epsilon(problem, 2, 10.0);
    BORG_Problem_set_epsilon(problem, 3, 0.025);
    BORG_Problem_set_epsilon(problem, 4, 0.01);
}
#endif

PaperTestProblem::PaperTestProblem(unsigned long n_weeks, int n_realizations,
                                   int import_export_rof_table,
                                   string system_io,
                                   string &rof_tables_directory,
                                   int seed, unsigned long n_threads,
                                   string bootstrap_file,
                                   string &utilities_rdm_file,
                                   string &policies_rdm_file,
                                   string &water_sources_rdm_file,
                                   int n_sets, int n_bs_samples,
                                   string &solutions_file,
                                   vector<int> &solutions_to_run_range,
                                   bool plotting, bool print_obj_row)
        : HardCodedProblem(n_weeks, n_realizations, import_export_rof_table,
                           system_io, rof_tables_directory, seed, n_threads,
                           bootstrap_file, utilities_rdm_file,
                           policies_rdm_file,
                           water_sources_rdm_file, n_sets, n_bs_samples,
                           solutions_file, solutions_to_run_range, plotting,
                           print_obj_row) {
    readInputData();
}

PaperTestProblem::~PaperTestProblem() = default;

int PaperTestProblem::functionEvaluation(double *vars, double *objs,
                                         double *consts) {

    // ===================== SET UP DECISION VARIABLES  =====================
    printf("Setting up problem\n");
    Simulation *s = nullptr;
    double calibrate_volume_multiplier = 1.;

    double watertown_restriction_trigger = vars[0];
    double dryville_restriction_trigger = vars[1];
    double fallsland_restriction_trigger = vars[2];
    double dryville_transfer_trigger = vars[3];
    double fallsland_transfer_trigger = vars[4];
    double watertown_LMA = vars[5];
    double dryville_LMA = vars[6];
    double fallsland_LMA = vars[7];
    double watertown_annual_payment = vars[8];
    double dryville_annual_payment = vars[9];
    double fallsland_annual_payment = vars[10];
    double watertown_insurance_use = vars[11];
    double dryville_insurance_use = vars[12];
    double fallsland_insurance_use = vars[13];
    double watertown_insurance_payment = vars[14];
    double dryville_insurance_payment = vars[15];
    double fallsland_insurance_payment = vars[16];
    double watertown_inftrigger = vars[17];
    double dryville_inftrigger = vars[18];
    double fallsland_inftrigger = vars[19];

    if (import_export_rof_tables == EXPORT_ROF_TABLES) {
        dryville_inftrigger = 1.1;
        fallsland_inftrigger = 1.1;
        watertown_inftrigger = 1.1;
    };

    double new_river_rank_watertown = vars[20];
    double new_river_rank_fallsland = vars[26];
    double sugar_creek_rank = vars[24];
    double granite_quarry_rank = vars[25];
    double college_rock_expansion_low_rank = vars[21];
    double college_rock_expansion_high_rank = vars[22];
    double watertown_reuse_i_rank = vars[23];
    double watertown_reuse_ii_rank = vars[27];
    double dryville_reuse_rank = vars[28];
    double fallsland_reuse_rank = vars[29];

    vector<infraRank> dryville_infra_order_raw = {
            infraRank(5, sugar_creek_rank),
            infraRank(11, dryville_reuse_rank)
    };

    vector<infraRank> fallsland_infra_order_raw = {
            infraRank(4, new_river_rank_fallsland),
            infraRank(12, fallsland_reuse_rank)
    };

    vector<infraRank> watertown_infra_order_raw = {
            infraRank(4, new_river_rank_watertown),
            infraRank(7, college_rock_expansion_low_rank),
            infraRank(8, college_rock_expansion_high_rank),
            infraRank(9, watertown_reuse_i_rank),
            infraRank(10, watertown_reuse_ii_rank)
    };

    sort(dryville_infra_order_raw.begin(),
         dryville_infra_order_raw.end(),
         by_xreal());
    sort(fallsland_infra_order_raw.begin(),
         fallsland_infra_order_raw.end(),
         by_xreal());
    sort(watertown_infra_order_raw.begin(),
         watertown_infra_order_raw.end(),
         by_xreal());

    vector<int> rof_triggered_infra_order_dryville =
            vecInfraRankToVecInt(dryville_infra_order_raw);
    vector<double> rofs_infra_dryville = vector<double>
            (rof_triggered_infra_order_dryville.size(), dryville_inftrigger);

    vector<int> rof_triggered_infra_order_fallsland =
            vecInfraRankToVecInt(fallsland_infra_order_raw);
    vector<double> rofs_infra_fallsland = vector<double>
            (rof_triggered_infra_order_fallsland.size(), fallsland_inftrigger);

    vector<int> rof_triggered_infra_order_watertown =
            vecInfraRankToVecInt(watertown_infra_order_raw);
    vector<double> rofs_infra_watertown = vector<double>
            (rof_triggered_infra_order_watertown.size(), watertown_inftrigger);

    // Remove small expansions being built after big expansions that would
    // encompass the small expansions.
    double added_storage_college_rock_expansion_low =
            1000 * calibrate_volume_multiplier;
    double added_storage_college_rock_expansion_high =
            2500 * calibrate_volume_multiplier;

//    checkAndFixInfraExpansionHighLowOrder(
//            rof_triggered_infra_order_watertown, rofs_infra_watertown,
//            7,
//            8,
//            added_storage_college_rock_expansion_low,
//            added_storage_college_rock_expansion_high);

    double watertown_demand_buffer = 1.0;
    double dryville_demand_buffer = 1.0;
    double fallsland_demand_buffer = 1.0;

    // Normalize lake michael allocations in case they exceed 1
    double sum_lma_allocations = dryville_LMA + fallsland_LMA + watertown_LMA;
    if (sum_lma_allocations == 0.)
        __throw_invalid_argument("LMA allocations cannot be all zero.");
    if (sum_lma_allocations > 1) {
        dryville_LMA /= sum_lma_allocations;
        fallsland_LMA /= sum_lma_allocations;
        watertown_LMA /= sum_lma_allocations;
    }

    // ==================== SET UP RDM FACTORS ============================
    if (utilities_rdm.empty()) {
        // All matrices below have dimensions n_realizations x nr_rdm_factors
        utilities_rdm = std::vector<vector<double>>(
                n_realizations, vector<double>(4, 1.));
        water_sources_rdm = std::vector<vector<double>>(
                n_realizations, vector<double>(51, 1.));
        policies_rdm = std::vector<vector<double>>(
                n_realizations, vector<double>(4, 1.));
    }

    // ===================== SET UP PROBLEM COMPONENTS =====================
//Beginning with Reservoir continuity

    int streamflow_n_weeks = (int) streamflows_durham[0].size();

    EvaporationSeries evaporation_durham(evap_durham,
                                         streamflow_n_weeks); //Evaporation
    EvaporationSeries evaporation_falls_lake(evap_falls_lake,
                                             streamflow_n_weeks); //Evaporation
    EvaporationSeries evaporation_jordan_lake(evap_jordan_lake,
                                              streamflow_n_weeks); // Lake Michael

    // Create catchments and corresponding vectors
    // Autumn Lake (abstracted Neuse River Basin)
    Catchment lower_flat_river(streamflows_flat, streamflow_n_weeks);
    Catchment little_river_raleigh_autumn(streamflows_llr, streamflow_n_weeks);

    // Add catchments to vector
    vector<Catchment *> catchment_autumn;
    catchment_autumn.push_back(&lower_flat_river);
    catchment_autumn.push_back(&little_river_raleigh_autumn);

    // College Rock Reservoir Catchment (abstracted from upper cape fear)
    Catchment phils_creek(streamflows_phils, streamflow_n_weeks);
    Catchment morgan_creek(streamflows_morgan, streamflow_n_weeks);
    Catchment crabtree_creek_cr(streamflows_crabtree, streamflow_n_weeks);
    Catchment cane_creek_cr(streamflows_cane, streamflow_n_weeks);

    // Add catchments to vector
    // College Rock (University Lake)
    vector<Catchment *> catchment_college_rock;
    catchment_college_rock.push_back(&morgan_creek);
    catchment_college_rock.push_back(&crabtree_creek_cr);
    catchment_college_rock.push_back(&phils_creek);

    // Granite Quarry (Stone Quarry)
    Catchment durham_inflows(streamflows_durham, streamflow_n_weeks);
    vector<Catchment *> catchment_granite_quarry;
    catchment_granite_quarry.push_back(&phils_creek);
    catchment_granite_quarry.push_back(&durham_inflows);

    // Sugar Creek (Cane Creek)
    Catchment swift_creek(streamflows_swift, streamflow_n_weeks);
    Catchment sugar_creek(streamflows_cane, streamflow_n_weeks);

    // Add catchment to vector
    vector<Catchment *> catchment_sugar_creek;
    catchment_sugar_creek.push_back(&sugar_creek);
    catchment_sugar_creek.push_back(&swift_creek);

    // Lake Michael Catchment (abstracted lower cape fear)
    Catchment lower_haw_river(streamflows_haw, streamflow_n_weeks);

    // Add catchment to vector
    vector<Catchment *> catchment_lower_haw_river;
    catchment_lower_haw_river.push_back(&lower_flat_river);

    // New River Reservoir catchment
    Catchment little_river_raleigh_nrr(streamflows_llr, streamflow_n_weeks);
    Catchment clayton_gage(streamflows_clayton, streamflow_n_weeks);

    vector<Catchment *> catchment_new_river;
    catchment_new_river.push_back(&little_river_raleigh_nrr);
    catchment_new_river.push_back(&clayton_gage);

    // Downstream Gage
    Catchment cape_fear_river_at_lillington(streamflows_lillington,
                                            streamflow_n_weeks);

    // Add catchment to vector
    vector<Catchment *> gage_lillington;
    gage_lillington.push_back(&cape_fear_river_at_lillington);

    double autumn_lake_supply_capacity =
            (14000.0 + 6349 + 2790 + 13500) * table_gen_storage_multiplier *
            calibrate_volume_multiplier; //6000 added to balance test problem.
    double autumn_lake_wq_capacity = 14000.0 * table_gen_storage_multiplier *
                                     calibrate_volume_multiplier;
    double autumn_lake_storage_capacity =
            autumn_lake_wq_capacity + autumn_lake_supply_capacity;
    vector<double> autumn_lake_storage = {0, autumn_lake_supply_capacity,
                                          autumn_lake_storage_capacity};
    vector<double> autumn_lake_area = {0.32 * 5734, 0.32 * 29000, 0.28 * 40434};
    DataSeries autumn_lake_storage_area(autumn_lake_storage, autumn_lake_area);

    vector<double> new_river_res_storage = {0,
                                            7300 * calibrate_volume_multiplier};
    vector<double> new_river_res_area = {0, 0.3675 * new_river_res_storage[1]};
    DataSeries new_river_storage_area(new_river_res_storage,
                                      new_river_res_area);

    vector<double> sugar_creek_res_storage = {0, 4500};//2909};
    vector<double> sugar_creek_res_area = {0,
                                           0.3675 * sugar_creek_res_storage[1]};
    DataSeries sugar_creek_storage_area(sugar_creek_res_storage,
                                        sugar_creek_res_area);

    vector<double> granite_quarry_storage = {0, 1500};
    vector<double> granite_quarry_area = {0,
                                          0.3675 * granite_quarry_storage[1]};
    DataSeries granite_quarry_storage_area(granite_quarry_storage,
                                           granite_quarry_area);

    // Create minimum environmental flow rules (controls)
    double lake_michael_supply_capacity =
            9100.0 * table_gen_storage_multiplier *
            calibrate_volume_multiplier; // reduced to .69 of JL cap
    double lake_michael_wq_capacity = 10825.0 * table_gen_storage_multiplier *
                                      calibrate_volume_multiplier;
    vector<int> autumn_controls_weeks = {0, 13, 43, 53};
    vector<double> autumn_releases = {(39 + 10 + 2) * 7, (65 + 4 + 1) * 7,
                                      (39 + 10 + 2) * 7};
    vector<double> sugar_creek_inflows = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                          1.797 * 7};
    vector<double> sugar_creek_releases = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                           1.797 * 7};
    vector<int> new_river_controls_weeks = {0, 13, 43, 53};
    vector<double> new_river_releases = {3 * 7, 8 * 7, 3 * 7};

    // College Rock and Granite Quarry have no min flow
    FixedMinEnvFlowControl college_rock_min_env_control(0, 0);
    JordanLakeMinEnvFlowControl lake_michael_min_env_control(1,
                                                             cape_fear_river_at_lillington,
                                                             64.63, 129.26,
                                                             25.85, 93.89,
                                                             190.84, 287.79,
                                                             lake_michael_supply_capacity,
                                                             lake_michael_wq_capacity);
    SeasonalMinEnvFlowControl autumn_min_env_control(2, autumn_controls_weeks,
                                                     autumn_releases);
    InflowMinEnvFlowControl sugar_creek_min_env_control(3,
                                                        sugar_creek_inflows,
                                                        sugar_creek_releases);
    FixedMinEnvFlowControl new_river_min_env_control(4, 0);
    FixedMinEnvFlowControl granite_quarry_min_env_control(6, 0);

    vector<MinEnvFlowControl *> min_env_flow_controls;
    min_env_flow_controls.push_back(&autumn_min_env_control);
    min_env_flow_controls.push_back(&lake_michael_min_env_control);
    min_env_flow_controls.push_back(&sugar_creek_min_env_control);
    min_env_flow_controls.push_back(&college_rock_min_env_control);
    min_env_flow_controls.push_back(&granite_quarry_min_env_control);

    // Lake Michael parameters
    double lake_michael_storage_capacity =
            lake_michael_wq_capacity + lake_michael_supply_capacity;
    vector<int> lake_michael_allocations_ids = {0, 1, 2,
                                                WATER_QUALITY_ALLOCATION};
    vector<double> lake_michael_allocation_fractions = {
            watertown_LMA * lake_michael_supply_capacity /
            lake_michael_storage_capacity,
            dryville_LMA * lake_michael_supply_capacity /
            lake_michael_storage_capacity,
            fallsland_LMA * lake_michael_supply_capacity /
            lake_michael_storage_capacity,
            lake_michael_wq_capacity / lake_michael_storage_capacity};
    vector<double> lake_michael_treatment_allocation_fractions = {1., 0.0, 0.0};

    // Autumn Lake parameters
    vector<int> autumn_lake_allocations_ids = {1, 2, WATER_QUALITY_ALLOCATION};
    vector<double> autumn_lake_allocation_fractions = {
            0.22 * autumn_lake_supply_capacity / autumn_lake_storage_capacity,
            0.78 * autumn_lake_supply_capacity / autumn_lake_storage_capacity,
            autumn_lake_wq_capacity / autumn_lake_storage_capacity};
    vector<double> autumn_lake_treatment_allocation_fractions = {0.38, 0.62};

// Create existing_infrastructure reservoirs
    Reservoir college_rock_reservoir("College Rock Reservoir",
                                     0,
                                     catchment_college_rock,
                                     1049 * table_gen_storage_multiplier *
                                     calibrate_volume_multiplier,
                                     560,
                                     evaporation_jordan_lake,
                                     222);

    AllocatedReservoir lake_michael("Lake Michael",
                                    1,
                                    catchment_lower_haw_river,
                                    lake_michael_storage_capacity,
                                    548,
                                    evaporation_jordan_lake,
                                    13940,
                                    lake_michael_allocations_ids,
                                    lake_michael_allocation_fractions,
                                    lake_michael_treatment_allocation_fractions);

    AllocatedReservoir autumn_lake("Autumn Lake",
                                   2,
                                   catchment_autumn,
                                   autumn_lake_storage_capacity,
                                   ILLIMITED_TREATMENT_CAPACITY,
                                   evaporation_falls_lake,
                                   autumn_lake_storage_area,
                                   autumn_lake_allocations_ids,
                                   autumn_lake_allocation_fractions,
                                   autumn_lake_treatment_allocation_fractions);

    // Create potential sources
    vector<double> city_infrastructure_rof_triggers = {watertown_inftrigger,
                                                       dryville_inftrigger,
                                                       fallsland_inftrigger};

    //FIXME ORIGINAL CODE SETS WEEKS_IN_YEAR TO 0 HERE
    vector<double> construction_time_interval = {3.0, 5.0};

    LevelDebtServiceBond dummy_bond(3, 1., 1, 1., vector<int>(1, 0));
    Reservoir dummy_endpoint("Dummy Node", 3, vector<Catchment *>(), 1., 0,
                             evaporation_durham, 1, {},
                             construction_time_interval, 0, dummy_bond);

    //FIXME check bond, this one is from little river raliegh
    vector<int> nrr_allocations_ids = {1, 2, WATER_QUALITY_ALLOCATION};
    vector<double> nrr_allocation_fractions = {0.3, 0.4, 0.3};
    vector<double> nrr_treatment_allocation_fractions = {0.5, 0.5};

    LevelDebtServiceBond new_river_bond(4, 263.0, 25, 0.05, vector<int>(1, 0));
    AllocatedReservoir new_river_reservoir("New River Reservoir",
                                           4,
                                           catchment_new_river,
                                           new_river_res_storage[1],
                                           ILLIMITED_TREATMENT_CAPACITY,
                                           evaporation_falls_lake,
                                           new_river_storage_area, {},
                                           construction_time_interval,
                                           17 * WEEKS_IN_YEAR,
                                           new_river_bond,
                                           nrr_allocations_ids,
                                           nrr_allocation_fractions,
                                           nrr_treatment_allocation_fractions);

    LevelDebtServiceBond sugar_bond(5, 150.0, 25, 0.05, vector<int>(1, 0));
    Reservoir sugar_creek_reservoir("Sugar Creek Reservoir",
                                    5,
                                    catchment_sugar_creek,
                                    sugar_creek_res_storage[1],
                                    ILLIMITED_TREATMENT_CAPACITY,
                                    evaporation_jordan_lake,
                                    sugar_creek_storage_area, {},
                                    construction_time_interval,
                                    17 * WEEKS_IN_YEAR,
                                    sugar_bond);

//    LevelDebtServiceBond granite_bond(5, 22.6, 25, 0.05, vector<int>(1, 0));
    Reservoir granite_quarry("Granite Quarry",
                             6,
                             catchment_granite_quarry,
                             granite_quarry_storage[1],
                             ILLIMITED_TREATMENT_CAPACITY,
                             evaporation_jordan_lake,
                             granite_quarry_storage_area);

    vector<double> college_rock_expansion_low_construction_time = {3, 5};
    LevelDebtServiceBond college_rock_expansion_low_bond(7, 50, 30, .05,
                                                         vector<int>(1, 0));
    ReservoirExpansion college_rock_expansion_low(
            (char *) "College Rock Expansion Low", 7, 0,
            added_storage_college_rock_expansion_low, {},
            college_rock_expansion_low_construction_time, 5 * WEEKS_IN_YEAR,
            college_rock_expansion_low_bond);

    vector<double> college_rock_expansion_high_construction_time = {3, 5};
    LevelDebtServiceBond college_rock_expansion_high_bond(8, 100, 30, .05,
                                                          vector<int>(1, 0));
    ReservoirExpansion college_rock_expansion_high(
            (char *) "College Rock Expansion High", 8, 0,
            added_storage_college_rock_expansion_high, {},
            college_rock_expansion_high_construction_time, 5 * WEEKS_IN_YEAR,
            college_rock_expansion_high_bond);

    vector<double> watertown_reuse_construction_time = {3, 5};
    LevelDebtServiceBond watertown_reuse_bond_i(9, 50, 30, .05,
                                                vector<int>(1, 0));
    WaterReuse watertown_reuse_i((char *) "Watertown Reuse I", 9, 35, {},
                                 watertown_reuse_construction_time,
                                 5 * WEEKS_IN_YEAR,
                                 watertown_reuse_bond_i);

    LevelDebtServiceBond watertown_reuse_bond_ii(10, 30, 30, .05,
                                                 vector<int>(1, 0));
    WaterReuse watertown_reuse_ii((char *) "Watertown Reuse II", 10, 35, {},
                                  watertown_reuse_construction_time,
                                  5 * WEEKS_IN_YEAR,
                                  watertown_reuse_bond_ii);

    LevelDebtServiceBond dryville_reuse_bond(11, 30, 30, .05,
                                             vector<int>(1, 0));
    WaterReuse dryville_reuse((char *) "Dryfille Reuse", 11, 35, {},
                              watertown_reuse_construction_time,
                              5 * WEEKS_IN_YEAR,
                              dryville_reuse_bond);

    LevelDebtServiceBond fallsland_reuse_bond(12, 50, 30, .05,
                                              vector<int>(1, 0));
    WaterReuse fallsland_reuse((char *) "Fallsland Reuse", 12, 35, {},
                               watertown_reuse_construction_time,
                               5 * WEEKS_IN_YEAR,
                               fallsland_reuse_bond);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&college_rock_reservoir);
    water_sources.push_back(&autumn_lake);
    water_sources.push_back(&lake_michael);
    water_sources.push_back(&sugar_creek_reservoir);
    water_sources.push_back(&new_river_reservoir);
    water_sources.push_back(&granite_quarry);
    water_sources.push_back(&college_rock_expansion_low);
    water_sources.push_back(&college_rock_expansion_high);
    water_sources.push_back(&watertown_reuse_i);
    water_sources.push_back(&watertown_reuse_ii);
    water_sources.push_back(&dryville_reuse);
    water_sources.push_back(&fallsland_reuse);
    water_sources.push_back(&dummy_endpoint);


/*
 *
 *  0 College Rock Reservoir (7) small expansion (8) large expansion
 *   \
 *    \                          (6) Granite Quarry
 *     \                         /
 *      \                      (5) Sugar Creek Reservoir
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
 *                 \ /    (9) watertown reuse
 *                  |
 *                  |
 *                  3 Dummy Endpoint
 */

    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(1, 3);
    g.addEdge(6, 5);
    g.addEdge(5, 2);
    g.addEdge(2, 4);
    g.addEdge(4, 3);

    auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

    vector<double> bond_term = {25, 25, 25, 25};
    vector<double> bond_rate = {0.05, 0.05, 0.05, 0.05};
    double discount_rate = 0.07;

    //FIXME make return flows after utilities are created?
    vector<int> watertown_ws_return_id;
    vector<vector<double>> watertown_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_watertown(
            watertown_discharge_fraction_series,
            watertown_ws_return_id);

    vector<int> dryville_ws_return_id = {4};
    vector<vector<double>> dryville_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_dryville(
            demand_to_wastewater_fraction_dryville,
            dryville_ws_return_id);

    vector<int> fallsland_ws_return_id = {4};
    vector<vector<double>> fallsland_discharge_fraction_series;
    WwtpDischargeRule wwtp_discharge_fallsland(
            demand_to_wastewater_fraction_fallsland,
            fallsland_ws_return_id);

    //FIXME: bond etc need to be updated, should chat about demand buffer
    vector<int> watertown_res_expansion_order = {7, 8};
    Utility watertown("Watertown", 0, demand_watertown, demand_n_weeks,
                      watertown_annual_payment,
                      watertownDemandClassesFractions,
                      watertownUserClassesWaterPrices, wwtp_discharge_watertown,
                      watertown_demand_buffer,
                      rof_triggered_infra_order_watertown, vector<int>(),
                      rofs_infra_watertown, discount_rate,
                      vector<vector<int>>());

    //FIXME: bond etc need to be updated, should chat about demand buffer
    Utility dryville("Dryville", 1, demand_dryville, demand_n_weeks,
                     dryville_annual_payment,
                     dryvilleDemandClassesFractions,
                     dryvilleUserClassesWaterPrices,
                     wwtp_discharge_dryville, dryville_demand_buffer,
                     rof_triggered_infra_order_dryville, vector<int>(),
                     rofs_infra_dryville, discount_rate, vector<vector<int>>());

    Utility fallsland("Fallsland", 2, demand_fallsland, demand_n_weeks,
                      fallsland_annual_payment,
                      fallslandDemandClassesFractions,
                      fallslandUserClassesWaterPrices,
                      wwtp_discharge_fallsland, fallsland_demand_buffer,
                      rof_triggered_infra_order_fallsland,
                      vector<int>(), rofs_infra_fallsland, discount_rate,
                      vector<vector<int>>());

    vector<Utility *> utilities;
    utilities.push_back(&watertown);
    utilities.push_back(&dryville);
    utilities.push_back(&fallsland);

    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 1, 4, 7, 8, 9, 10}, //Watertown
            {2, 5, 6, 11}, //Dryville
            {2, 4, 12} //Fallsland
    };

    auto table_storage_shift = vector<vector<double>>(3, vector<double>(
            water_sources.size() + 1, 0.));
    table_storage_shift[2][4] = 1500;
    table_storage_shift[1][5] = 100;
    table_storage_shift[0][9] = 1000;
    table_storage_shift[0][10] = 600;
    table_storage_shift[1][11] = 600;
    table_storage_shift[2][12] = 1000;

    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    vector<double> initial_restriction_triggers = {
            watertown_restriction_trigger,
            dryville_restriction_trigger,
            fallsland_restriction_trigger};

    vector<double> restriction_stage_multipliers_watertown = {0.9, 0.75, 0.65,
                                                              0.5};
    vector<double> restriction_stage_triggers_watertown = {
            initial_restriction_triggers[0],
            initial_restriction_triggers[0] + 0.15f,
            initial_restriction_triggers[0] + 0.35f,
            initial_restriction_triggers[0] + 0.6f};

    vector<double> restriction_stage_multipliers_dryville = {0.9, 0.75, 0.65,
                                                             0.5};
    vector<double> restriction_stage_triggers_dryville = {
            initial_restriction_triggers[1],
            initial_restriction_triggers[1] + 0.15f,
            initial_restriction_triggers[1] + 0.35f,
            initial_restriction_triggers[1] + 0.6f};

    vector<double> restriction_stage_multipliers_fallsland = {0.9, 0.75, 0.65,
                                                              0.5};
    vector<double> restriction_stage_triggers_fallsland = {
            initial_restriction_triggers[2],
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

    drought_mitigation_policies = {&restrictions_w, &restrictions_d,
                                   &restrictions_f};


    // Transfer policy

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
    vector<double> buyer_transfer_capacities = {20. * 7, 20. * 7, 15. * 7};

    vector<double> buyer_transfer_triggers = {dryville_transfer_trigger,
                                              fallsland_transfer_trigger};

    Graph ug(3);
    ug.addEdge(0, 1);
    ug.addEdge(0, 2);
    ug.addEdge(1, 2);

    Transfers t(3, 0, 1, 35,
                buyers_ids,
                buyer_transfer_capacities,
                buyer_transfer_triggers,
                ug,
                vector<double>(),
                vector<int>());
    drought_mitigation_policies.push_back(&t);


    // Set up insurance
    vector<double> insurance_triggers = {watertown_insurance_use,
                                         dryville_insurance_use,
                                         fallsland_insurance_use};
    vector<double> fixed_payouts = {watertown_insurance_payment,
                                    dryville_insurance_payment,
                                    fallsland_insurance_payment};

    vector<int> insured_utilities = {0, 1, 2};

//    InsuranceStorageToROF in(4, water_sources, g,
//                             reservoir_utility_connectivity_matrix, utilities,
//                             drought_mitigation_policies, min_env_flow_controls,
//                             utilities_rdm, water_sources_rdm, policies_rdm,
//                             insurance_triggers, 1.2, fixed_payouts, n_weeks);
//
//    drought_mitigation_policies.push_back(&in);


    // Creates simulation object depending on use (or lack thereof) ROF tables
    printf("Starting Simulation\n");
    double start_time = omp_get_wtime();
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
        this->master_data_collector = s->runFullSimulation(n_threads, vars);
    } else if (import_export_rof_tables == IMPORT_ROF_TABLES) {
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
                           rof_tables,
                           table_storage_shift,
                           rof_tables_directory);
        this->master_data_collector = s->runFullSimulation(n_threads, vars);
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
        this->master_data_collector = s->runFullSimulation(n_threads, vars);
    }
    double end_time = omp_get_wtime();
    printf(" Function evaluation time: %f\n", end_time - start_time);

    // Calculate objectives and store them in Borg decision variables array.
#ifdef  PARALLEL
    if (import_export_rof_tables != EXPORT_ROF_TABLES) {
        objectives = calculateAndPrintObjectives(false);

        int i = 0;
        objs[i] = 1. - min(min(objectives[i], objectives[5 + i]),
                   objectives[10 + i]);
        for (i = 1; i < 5; ++i) {
            objs[i] = max(max(objectives[i], objectives[5 + i]),
                            objectives[10 + i]);
        }

    for (int i = 0; i < NUM_OBJECTIVES; ++i) {
         if (isnan(objs[i])) {
        for (int j = 0; j < NUM_OBJECTIVES; ++j) {
            objs[i] = 1e5;
        }
        break;
        }
    }
    }

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

void PaperTestProblem::readInputData() {
    cout << "Reading input data." << endl;
    string data_dir = DEFAULT_DATA_DIR + BAR;

    string rdm_tseries_dir;
    if (rdm_no != NON_INITIALIZED) {
        rdm_tseries_dir = "rdm_tseries/rdm_" + to_string(rdm_no) + BAR;
    } else {
        rdm_tseries_dir = DEFAULT_RDM_TSERIES_DIR;
    }

#pragma omp parallel num_threads(omp_get_thread_num()) default(none) shared(rdm_tseries_dir)
    {
#pragma omp single
        streamflows_durham = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "durham_inflows.csv", n_realizations);
#pragma omp single
        streamflows_flat = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "falls_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_swift = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "lake_wb_inflows.csv", n_realizations);
#pragma omp single
        streamflows_llr = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "little_river_raleigh_inflows.csv", n_realizations);
#pragma omp single
        streamflows_phils = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "stone_quarry_inflows.csv", n_realizations);
#pragma omp single
        streamflows_cane = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "cane_creek_inflows.csv", n_realizations);
#pragma omp single
        streamflows_morgan = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "university_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_crabtree = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "crabtree_inflows.csv", n_realizations);
//                BAR + "university_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_haw = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "jordan_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_lillington = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "lillington_inflows.csv", n_realizations);
#pragma omp single
        streamflows_clayton = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "inflows" +
                evap_inflows_suffix +
                BAR + "clayton_inflows.csv", n_realizations);

#pragma omp single
        evap_durham = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir +
                "evaporation" + evap_inflows_suffix +
                BAR + "durham_evap.csv", n_realizations);
#pragma omp single
        evap_falls_lake = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir +
                "evaporation" + evap_inflows_suffix +
                BAR + "falls_lake_evap.csv", n_realizations);
#pragma omp single
        evap_jordan_lake = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir +
                "evaporation" + evap_inflows_suffix +
                BAR + "jordan_lake_evap.csv", n_realizations);
#pragma omp single
        evap_little_river = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir +
                "evaporation" + evap_inflows_suffix +
                BAR + "little_river_raleigh_evap.csv", n_realizations);
#pragma omp single
        {
            evap_wheeler_benson = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir +
                    "evaporation" + evap_inflows_suffix +
                    BAR + "wb_evap.csv", n_realizations);
        }

#pragma omp single
        demand_watertown = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "demands" +
                evap_inflows_suffix +
                BAR + "cary_demand.csv", n_realizations);
#pragma omp single
        demand_dryville = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "demands" +
                evap_inflows_suffix +
                BAR + "durham_demand.csv", n_realizations);
#pragma omp single
        demand_fallsland = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + rdm_tseries_dir + "demands" +
                evap_inflows_suffix +
                BAR + "raleigh_demand.csv", n_realizations);

#pragma omp single
        {
            demand_to_wastewater_fraction_fallsland = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "demand_to_wastewater_fraction_owasa_raleigh.csv");
            demand_to_wastewater_fraction_dryville = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "demand_to_wastewater_fraction_owasa_raleigh.csv");

            watertownDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "caryDemandClassesFractions.csv");
            dryvilleDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "durhamDemandClassesFractions.csv");
            fallslandDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "raleighDemandClassesFractions.csv");

            watertownUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "caryUserClassesWaterPrices.csv");
            dryvilleUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "durhamUserClassesWaterPrices.csv");
            fallslandUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR +
                    "raleighUserClassesWaterPrices.csv");
        }
    }
}


