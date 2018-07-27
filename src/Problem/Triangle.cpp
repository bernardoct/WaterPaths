//
// Created by Bernardo on 11/23/2017.
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
#include "Triangle.h"
#include "../Controls/SeasonalMinEnvFlowControl.h"
#include "../Controls/Custom/FallsLakeMinEnvFlowControl.h"
#include "../Controls/StorageMinEnvFlowControl.h"
#include "../Controls/InflowMinEnvFlowControl.h"
#include "../Controls/FixedMinEnvFlowControl.h"
#include "../Controls/Custom/JordanLakeMinEnvFlowControl.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../SystemComponents/WaterSources/Quarry.h"
#include "../SystemComponents/WaterSources/Relocation.h"
#include "../SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/SequentialJointTreatmentExpansion.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "../DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "../Simulation/Simulation.h"
#include "../SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../SystemComponents/Bonds/BalloonPaymentBond.h"

/**
 * Runs carolina problem.
 * @param vars
 * @param n_realizations
 * @param n_weeks
 * @param sol_number
 * @param output_directory
 * @todo check for solutions in which a utility does not have an allocation
 * on Jordan Lake (or any generic lake) but still pay for joint treatment
 * infrastructure).
 */
int Triangle::functionEvaluation(double *vars, double *objs, double *consts) {
//    cout << "Building Triangle Problem." << endl;
    //srand(0);//(unsigned int) time(nullptr));
    // ===================== SET UP DECISION VARIABLES  =====================

    Simulation *s = nullptr;
//    try {
	//__throw_invalid_argument("Test error");
        double Durham_restriction_trigger = vars[0];
        double OWASA_restriction_trigger = vars[1];
        double cary_restriction_trigger = vars[3];
        double durham_transfer_trigger = vars[4];
        double owasa_transfer_trigger = vars[5];
        double OWASA_JLA = vars[7];
        double Raleigh_JLA = vars[8];
        double Durham_JLA = vars[9];
        double Cary_JLA = vars[10];
        double durham_annual_payment = vars[11]; // contingency fund
        double owasa_annual_payment = vars[12];
        double raleigh_annual_payment = vars[13];
        double cary_annual_payment = vars[14];
        double durham_inftrigger = vars[23];
        double owasa_inftrigger = vars[24];
        double cary_inftrigger = vars[26];
        if (import_export_rof_tables == EXPORT_ROF_TABLES) {
            durham_inftrigger = 1.1;
            owasa_inftrigger = 1.1;
            cary_inftrigger = 1.1;
        }
        double university_lake_expansion_ranking = vars[27]; // 14
        double Cane_creek_expansion_ranking = vars[28]; // 24
        double Stone_quarry_reservoir_expansion_shallow_ranking = vars[29]; // 12
        double Stone_quarry_reservoir_expansion_deep_ranking = vars[30]; // 13
        double Teer_quarry_expansion_ranking = vars[31]; // 9
        double reclaimed_water_ranking_low = vars[32]; // 18
        double reclaimed_water_high = vars[33]; // 19
        double lake_michie_expansion_ranking_low = vars[34]; // 15
        double lake_michie_expansion_ranking_high = vars[35]; // 16

        double western_wake_treatment_plant_rank_OWASA_low = vars[40]; // 20
        double western_wake_treatment_plant_rank_OWASA_high = vars[41]; // 21
        double western_wake_treatment_plant_rank_durham_low = vars[42]; // 20
        double western_wake_treatment_plant_rank_durham_high = vars[43]; // 21

        double caryupgrades_2 = vars[47];
        double caryupgrades_3 = vars[48];
        double western_wake_treatment_plant_owasa_frac = vars[49];
        double western_wake_treatment_frac_durham = vars[50];
        double western_wake_treatment_plant_raleigh_frac = vars[51];

        double durham_inf_buffer = vars[53];
        double owasa_inf_buffer = vars[54];
        double cary_inf_buffer = vars[56];

        /// Fix IDs for all water sources
        int lake_michie_id = 0;
        int stone_quarry_id = 1;
        int univ_lake_id = 2;
        int ccr_id = 3;
        int jordan_lake_id = 4;
        int teer_quarry_id = 5;
        int dummy_id = 6;
        int michie_low_exp_id = 7;
        int michie_high_exp_id = 8;
        int quarry_low_exp_id = 9;
        int quarry_high_exp_id = 10;
        int univ_lake_exp_id = 11;
        int ccr_exp_id = 12;
        int reclaimed_low_id = 13;
        int reclaimed_high_id = 14;
        int cary_low_exp_id = 15;
        int cary_high_exp_id = 16;
        int wjlwtp_low_id = 17;
        int wjlwtp_high_id = 18;

        if (utilities_rdm.empty()) {
            /// All matrices below have dimensions n_realizations x nr_rdm_factors
            utilities_rdm = std::vector<vector<double>>(
                    n_realizations, vector<double>(4, 1.));
            water_sources_rdm = std::vector<vector<double>>(
                    n_realizations, vector<double>(51, 1.));
            policies_rdm = std::vector<vector<double>>(
                    n_realizations, vector<double>(4, 1.));
        }

        vector<infraRank> durham_infra_order_raw = {
                infraRank(teer_quarry_id, Teer_quarry_expansion_ranking),
                infraRank(michie_low_exp_id, lake_michie_expansion_ranking_low),
                infraRank(michie_high_exp_id, lake_michie_expansion_ranking_high),
                infraRank(reclaimed_low_id, reclaimed_water_ranking_low),
                infraRank(reclaimed_high_id, reclaimed_water_high),
                infraRank(wjlwtp_low_id, western_wake_treatment_plant_rank_durham_low),
                infraRank(wjlwtp_high_id, western_wake_treatment_plant_rank_durham_high)
        };

        vector<infraRank> owasa_infra_order_raw = {
                infraRank(quarry_low_exp_id, Stone_quarry_reservoir_expansion_shallow_ranking),
                infraRank(quarry_high_exp_id, Stone_quarry_reservoir_expansion_deep_ranking),
                infraRank(univ_lake_exp_id, university_lake_expansion_ranking),
                infraRank(ccr_exp_id, Cane_creek_expansion_ranking),
                infraRank(wjlwtp_low_id, western_wake_treatment_plant_rank_OWASA_low),
                infraRank(wjlwtp_high_id, western_wake_treatment_plant_rank_OWASA_high)
        };

        double added_storage_michie_expansion_low = 2500;
        double added_storage_michie_expansion_high = 5200;
        double reclaimed_capacity_low = 2.2 * 7;
        double reclaimed_capacity_high = 9.1 * 7;

        /// get infrastructure construction order based on decision variables
        sort(durham_infra_order_raw.begin(),
             durham_infra_order_raw.end(),
             by_xreal());
        sort(owasa_infra_order_raw.begin(),
             owasa_infra_order_raw.end(),
             by_xreal());

        vector<int> rof_triggered_infra_order_durham =
                vecInfraRankToVecInt(durham_infra_order_raw);
        vector<double> rofs_infra_durham = vector<double>
                (rof_triggered_infra_order_durham.size(), durham_inftrigger);
        vector<int> rof_triggered_infra_order_owasa =
                vecInfraRankToVecInt(owasa_infra_order_raw);
        vector<double> rofs_infra_owasa = vector<double>
                (rof_triggered_infra_order_owasa.size(), owasa_inftrigger);

        /// Remove small expansions being built after big expansions that would
        /// encompass the smal expansions.
        added_storage_michie_expansion_high =
                checkAndFixInfraExpansionHighLowOrder(
                        &rof_triggered_infra_order_durham,
                        michie_low_exp_id,
                        michie_high_exp_id,
                        added_storage_michie_expansion_low,
                        added_storage_michie_expansion_high);

        reclaimed_capacity_high =
                checkAndFixInfraExpansionHighLowOrder(
                        &rof_triggered_infra_order_durham,
                        reclaimed_low_id,
                        reclaimed_high_id,
                        reclaimed_capacity_low,
                        reclaimed_capacity_high);


        /// Normalize Jordan Lake Allocations in case they exceed 1.
        double sum_jla_allocations = OWASA_JLA + Durham_JLA + Cary_JLA + Raleigh_JLA;
        if (sum_jla_allocations == 0.)
            __throw_invalid_argument("JLA allocations cannot be all "
                                             "zero.");
        if (sum_jla_allocations > 0.69) { // At the time this study was done, 31% of JL had been allocated to other utilities.
            OWASA_JLA /= sum_jla_allocations / 0.69;
            Durham_JLA /= sum_jla_allocations / 0.69;
            Cary_JLA /= sum_jla_allocations / 0.69;
            Raleigh_JLA /= sum_jla_allocations / 0.69;
        }

        /// Normalize West Jordan Lake WTP allocations.
        double sum_wjlwtp = western_wake_treatment_frac_durham +
                            western_wake_treatment_plant_owasa_frac +
                            western_wake_treatment_plant_raleigh_frac;
        if (sum_wjlwtp == 0.)
            __throw_invalid_argument("Treatment fractions for WJLWTP cannot be all "
                                             "zero.");
        western_wake_treatment_frac_durham /= sum_wjlwtp;
        western_wake_treatment_plant_owasa_frac /= sum_wjlwtp;
        western_wake_treatment_plant_raleigh_frac /= sum_wjlwtp;


        // ===================== SET UP PROBLEM COMPONENTS =====================

        //cout << "BEGINNING TRIANGLE TEST" << endl << endl;
        // cout << "Using " << omp_get_num_threads() << " cores." << endl;
    //    cout << getexepath() << endl;

        /// Read streamflows
        int streamflow_n_weeks = 52 * (70 + 50);

        /// In case a vector containing realizations numbers to be calculated is passed, set
        /// number of realizations to number of realizations in that vector.

    //    vector<double> sewageFractions = Utils::parse1DCsvFile(
    //            output_directory + "/TestFiles/sewageFractions.csv");

        EvaporationSeries evaporation_durham(&evap_durham, streamflow_n_weeks);
        EvaporationSeries evaporation_jordan_lake(
                &evap_jordan_lake,
                streamflow_n_weeks);
        EvaporationSeries evaporation_owasa(&evap_owasa, streamflow_n_weeks);

        /// Create catchments and corresponding vectors
        //  Durham (Upper Neuse River Basin)
        Catchment durham_inflows(&streamflows_durham, streamflow_n_weeks);

        // OWASA (Upper Cape Fear Basin)
        Catchment phils_reek(&streamflows_phils, streamflow_n_weeks);
        Catchment cane_creek(&streamflows_cane, streamflow_n_weeks);
        Catchment morgan_creek(&streamflows_morgan, streamflow_n_weeks);

        // Cary (Lower Cape Fear Basin)
        Catchment lower_haw_river(&streamflows_haw, streamflow_n_weeks);

        // Downstream Gages
        Catchment cape_fear_river_at_lillington(
                &streamflows_lillington,
                streamflow_n_weeks);

        vector<Catchment *> catchment_durham;

        vector<Catchment *> catchment_phils;
        vector<Catchment *> catchment_cane;
        vector<Catchment *> catchment_morgan;

        vector<Catchment *> catchment_haw;

        vector<Catchment *> gage_lillington;

        catchment_durham.push_back(&durham_inflows);

        catchment_phils.push_back(&phils_reek);
        catchment_cane.push_back(&cane_creek);
        catchment_morgan.push_back(&morgan_creek);

        catchment_haw.push_back(&lower_haw_river);

        gage_lillington.push_back(&cape_fear_river_at_lillington);

        /// Storage vs. area reservoir curves.
        vector<double> falls_lake_storage = {0, 23266 * table_gen_storage_multiplier, 34700 * table_gen_storage_multiplier};
        vector<double> falls_lake_area = {0.32 * 5734, 0.32 * 29000, 0.28 * 40434};
        vector<double> wheeler_benson_storage = {0, 2789.66 * table_gen_storage_multiplier};
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
        vector<int> falls_controls_weeks = {13, 43};
        vector<double> falls_base_releases = {64.64 * 7, 38.78 * 7};
        vector<double> falls_min_gage = {180 * 7, 130 * 7};

        SeasonalMinEnvFlowControl durham_min_env_control(0, dlr_weeks,
                                                         dlr_releases);

        FixedMinEnvFlowControl sq_min_env_control(3, 0);
        InflowMinEnvFlowControl ccr_min_env_control(4, vector<int>(1, 4),
                                                    ccr_inflows,
                                                    ccr_releases);
        FixedMinEnvFlowControl university_min_env_control(5, 0);

        JordanLakeMinEnvFlowControl jordan_min_env_control(
                6, cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
                290.84, 387.79, 30825.0, 14924.0);
        FixedMinEnvFlowControl teer_min_env_control(9, 0);

        vector<MinEnvFlowControl *> min_env_flow_controls;
        min_env_flow_controls.push_back(&durham_min_env_control);
        min_env_flow_controls.push_back(&sq_min_env_control);
        min_env_flow_controls.push_back(&ccr_min_env_control);
        min_env_flow_controls.push_back(&university_min_env_control);
        min_env_flow_controls.push_back(&jordan_min_env_control);
        min_env_flow_controls.push_back(&teer_min_env_control);

        /// Create reservoirs and corresponding vector
        vector<double> construction_time_interval = {3.0, 5.0};
        vector<double> city_infrastructure_rof_triggers = {owasa_inftrigger,
                                                           durham_inftrigger,
                                                           cary_inftrigger};
        vector<double> bond_term = {25, 25, 25};
        vector<double> bond_rate = {0.05, 0.05, 0.05};
        double discount_rate = 0.05;

        /// Jordan Lake parameters
        double jl_supply_capacity = 14924.0 * table_gen_storage_multiplier;
        double jl_wq_capacity = 30825.0 * table_gen_storage_multiplier;
        double jl_storage_capacity = jl_wq_capacity + jl_supply_capacity;
        vector<int> jl_allocations_ids = {0, 1, 2, WATER_QUALITY_ALLOCATION};
        vector<double> jl_allocation_fractions = {
                OWASA_JLA * jl_supply_capacity / jl_storage_capacity,
                Durham_JLA * jl_supply_capacity / jl_storage_capacity,
                Cary_JLA * jl_supply_capacity / jl_storage_capacity,
                jl_wq_capacity / jl_storage_capacity};
        vector<double> jl_treatment_allocation_fractions = {0.0, 0.0, 1.0};

        // Existing Sources
        Reservoir durham_reservoirs("Lake Michie & Little River Res. (Durham)",
                                    lake_michie_id,
                                    catchment_durham,
                                    6349.0 * table_gen_storage_multiplier,
                                    ILLIMITED_TREATMENT_CAPACITY,
                                    evaporation_durham, 1069);

        Reservoir stone_quarry("Stone Quarry",
                               stone_quarry_id,
                               catchment_phils,
                               200.0 * table_gen_storage_multiplier,
                               ILLIMITED_TREATMENT_CAPACITY,
                               evaporation_owasa,
                               10);
        Reservoir ccr("Cane Creek Reservoir",
                      ccr_id,
                      catchment_cane,
                      2909.0 * table_gen_storage_multiplier,
                      ILLIMITED_TREATMENT_CAPACITY,
                      evaporation_owasa,
                      500);
        Reservoir university_lake("University Lake", univ_lake_id, catchment_morgan,
                                  449.0 * table_gen_storage_multiplier,
                                  ILLIMITED_TREATMENT_CAPACITY,
                                  evaporation_owasa,
                                  212);

        AllocatedReservoir jordan_lake("Jordan Lake",
                                       jordan_lake_id,
                                       catchment_haw,
                                       jl_storage_capacity,
                                       448,
                                       evaporation_jordan_lake,
                                       13940,
                                       &jl_allocations_ids,
                                       &jl_allocation_fractions,
                                       &jl_treatment_allocation_fractions);

        // other than Cary WTP for Jordan Lake, assume no WTP constraints - each
        // city can meet its daily demands with available treatment infrastructure

        // Potential Sources
        // The capacities listed here for expansions are what additional capacity is gained relative to existing capacity,
        // NOT the total capacity after the expansion is complete. For infrastructure with a high and low option, this means
        // the capacity for both is relative to current conditions - if Lake Michie is expanded small it will gain 2.5BG,
        // and a high expansion will provide 7.7BG more water than current. if small expansion happens, followed by a large
        // expansion, the maximum capacity through expansion is 7.7BG, NOT 2.5BG + 7.7BG.

        BalloonPaymentBond tq_bond(teer_quarry_id, 22.6, 25, 0.05, vector<int>(1, 0), 3);
        Quarry teer_quarry("Teer Quarry", teer_quarry_id, vector<Catchment *>(), 1315.0, ILLIMITED_TREATMENT_CAPACITY,
                           evaporation_durham, &teer_storage_area, construction_time_interval, 7 * WEEKS_IN_YEAR, tq_bond,
                           15 * 7);

        // diversions to Teer Quarry for Durham based on inflows to downstream Falls Lake from the Flat River
        // FYI: spillage from Eno River also helps determine Teer quarry diversion, but Eno spillage isn't factored into
        // downstream water balance?

        LevelDebtServiceBond ccr_exp_bond(ccr_exp_id, 127.0, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion ccr_expansion("Cane Creek Reservoir Expansion", ccr_exp_id, ccr_id, 3000.0, construction_time_interval,
                                         17 * WEEKS_IN_YEAR, ccr_exp_bond);

        LevelDebtServiceBond low_sq_exp_bond(quarry_low_exp_id, 1.4, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion low_sq_expansion("Low Stone Quarry Expansion", quarry_low_exp_id, stone_quarry_id, 1500.0, construction_time_interval,
                                            23 * WEEKS_IN_YEAR, low_sq_exp_bond);

        LevelDebtServiceBond high_sq_exp_bond(quarry_high_exp_id, 64.6, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion high_sq_expansion("High Stone Quarry Expansion", quarry_high_exp_id, stone_quarry_id, 2200.0, construction_time_interval,
                                             23 * WEEKS_IN_YEAR, high_sq_exp_bond);

        LevelDebtServiceBond ul_exp_bond(univ_lake_exp_id, 107.0, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion univ_lake_expansion("University Lake Expansion", univ_lake_exp_id, univ_lake_id, 2550.0, construction_time_interval,
                                               17 * WEEKS_IN_YEAR, ul_exp_bond);

        LevelDebtServiceBond low_mi_exp_bond(michie_low_exp_id, 158.3, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion low_michie_expansion("Low Lake Michie Expansion", michie_low_exp_id, lake_michie_id, added_storage_michie_expansion_low,
                                                construction_time_interval, 17 * WEEKS_IN_YEAR, low_mi_exp_bond);

        LevelDebtServiceBond high_mi_exp_bond(michie_high_exp_id, 203.3, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion high_michie_expansion("High Lake Michie Expansion", michie_high_exp_id, lake_michie_id, added_storage_michie_expansion_high,
                                                 construction_time_interval, 17 * WEEKS_IN_YEAR, high_mi_exp_bond);

        LevelDebtServiceBond low_rec_bond(reclaimed_low_id, 27.5, 25, 0.05, vector<int>(1, 0));
        WaterReuse low_reclaimed("Low Reclaimed Water System", reclaimed_low_id, reclaimed_capacity_low, construction_time_interval,
                                 7 * WEEKS_IN_YEAR, low_rec_bond);

        LevelDebtServiceBond high_rec_bond(reclaimed_high_id, 104.4, 25, 0.05, vector<int>(1, 0));
        WaterReuse high_reclaimed("High Reclaimed Water System", reclaimed_high_id, reclaimed_capacity_high, construction_time_interval,
                                  7 * WEEKS_IN_YEAR, high_rec_bond);

        vector<double> wjlwtp_treatment_capacity_fractions =
                {western_wake_treatment_plant_owasa_frac,
                 western_wake_treatment_frac_durham,
                 0.,
                 0.};
        vector<double> cary_upgrades_treatment_capacity_fractions = {0., 0., 1., 0.};

        vector<double> capacities_wjlwtp_upgrade_1 = {
                33 * 7 * western_wake_treatment_plant_owasa_frac,
                33 * 7 * western_wake_treatment_frac_durham,
                0.
        };
        vector<double> cost_wjlwtp_upgrade_1 = {
                243.3 * western_wake_treatment_plant_owasa_frac,
                243.3 * western_wake_treatment_frac_durham,
                0.
        };
        vector<double> capacities_wjlwtp_upgrade_2 = {
                (54 * 7 - 33 * 7) * western_wake_treatment_plant_owasa_frac,
                (54 * 7 - 33 * 7) * western_wake_treatment_frac_durham,
                0.
        };
        vector<double> cost_wjlwtp_upgrade_2 = {
                (316.8 - 243.3) * western_wake_treatment_plant_owasa_frac,
                (316.8 - 243.3) * western_wake_treatment_frac_durham,
                0.
        };

        /// Bonds West Jordan Lake treatment plant
        vector<Bond *> wjlwtp_bonds_capacity_1;
        int uid = 0;
        for (double &cost : cost_wjlwtp_upgrade_1) {
            wjlwtp_bonds_capacity_1.emplace_back(new LevelDebtServiceBond(wjlwtp_low_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        vector<Bond *> wjlwtp_bonds_capacity_2;
        uid = 0;
        for (double &cost : cost_wjlwtp_upgrade_2) {
            wjlwtp_bonds_capacity_2.emplace_back(new LevelDebtServiceBond(wjlwtp_high_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        /// West Jordan Lake treatment plant
        SequentialJointTreatmentExpansion low_wjlwtp("Low WJLWTP", wjlwtp_low_id, jordan_lake_id, 0, {wjlwtp_low_id, wjlwtp_high_id}, capacities_wjlwtp_upgrade_1,
                                                     wjlwtp_bonds_capacity_1, construction_time_interval, 12 * WEEKS_IN_YEAR);
        SequentialJointTreatmentExpansion high_wjlwtp("High WJLWTP", wjlwtp_high_id, jordan_lake_id, 1, {wjlwtp_low_id, wjlwtp_high_id}, capacities_wjlwtp_upgrade_2,
                                                      wjlwtp_bonds_capacity_2, construction_time_interval, 12 * WEEKS_IN_YEAR);

        /// Bonds Cary treatment plant expansion
        vector<double> cost_cary_wtp_upgrades = {0, 0, 243. / 2};

        vector<Bond *> bonds_cary_wtp_upgrades_1;
        uid = 0;
        for (double &cost : cost_cary_wtp_upgrades) {
            bonds_cary_wtp_upgrades_1.emplace_back(new LevelDebtServiceBond(cary_low_exp_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        vector<Bond *> bonds_cary_wtp_upgrades_2;
        uid = 0;
        for (double &cost : cost_cary_wtp_upgrades) {
            bonds_cary_wtp_upgrades_2.emplace_back(new LevelDebtServiceBond(cary_high_exp_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        /// Cary treatment plant expansion
        vector<double> capacity_cary_wtp_upgrades = {0, 0, 56, 0};
        SequentialJointTreatmentExpansion caryWtpUpgrade1("Cary WTP upgrade 1", cary_low_exp_id, jordan_lake_id, 0, {cary_low_exp_id, cary_high_exp_id},
                                                          capacity_cary_wtp_upgrades, bonds_cary_wtp_upgrades_1,
                                                          construction_time_interval, NONE);
        SequentialJointTreatmentExpansion caryWtpUpgrade2("Cary WTP upgrade 2", cary_high_exp_id, jordan_lake_id, 1, {cary_low_exp_id, cary_high_exp_id},
                                                          capacity_cary_wtp_upgrades, bonds_cary_wtp_upgrades_2,
                                                          construction_time_interval, NONE);

        LevelDebtServiceBond dummy_bond(dummy_id, 1., 1, 1., vector<int>(1, 0));
        Reservoir dummy_endpoint("Dummy Node", dummy_id, vector<Catchment *>(), 1., 0, evaporation_durham, 1,
                                 construction_time_interval, 0, dummy_bond);

        vector<WaterSource *> water_sources;
        water_sources.push_back(&durham_reservoirs);
        water_sources.push_back(&stone_quarry);
        water_sources.push_back(&ccr);
        water_sources.push_back(&university_lake);
        water_sources.push_back(&jordan_lake);

        water_sources.push_back(&teer_quarry);
        water_sources.push_back(&ccr_expansion);
        water_sources.push_back(&univ_lake_expansion);
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
    * numbers may not be correct - see top of function where ids are set
    *             CCR     UL     SQ
    * SOURCE:      3      2       1
    * EXP/TRTMT: (12)    (11)   (9,10)
    *              |      /       /
    *              |     |_______/
    *              |    /
    *              |   /
    *              |  /
    *              | /
    *              |/
    *          Jordan Lake     Lake Michie & LRR     Teer Quarry
    * SOURCE:      4     (13,14)    0(7,8)               (5)
    * TREATMT:  (15-18) Reclaimed
    *              |        |         |                   |
    *              |       /           \                 /
    *              |      /             \               /
    *              |     /               \             /
    *              |    /                 \           /
    *              |   /                   \         /
    *              |  /                     \       /
    *              |_/                       \     /
    *              |                          \___/
    *              |                            |
    *        Lillington Gage                    |
    *              |                            |
    *               \                          /
    *                \                        /
    *                 \                      /
    *                  \                    /
    *                   \__________________/
    *                            |
    *                    6: Dummy Endpoint
    */

        Graph g(7);
        g.addEdge(0, 6);
        g.addEdge(5, 6);
        g.addEdge(4, 6);
        g.addEdge(1, 4);
        g.addEdge(2, 4);
        g.addEdge(3, 4);

        auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

        vector<int> cary_ws_return_id = {dummy_id};
        vector<vector<double>> cary_discharge_fraction_series;
        WwtpDischargeRule wwtp_discharge_cary(
                demand_to_wastewater_fraction_owasa_raleigh,
                cary_ws_return_id);
        vector<int> owasa_ws_return_id = {jordan_lake_id};
        WwtpDischargeRule wwtp_discharge_owasa(
                demand_to_wastewater_fraction_owasa_raleigh,
                owasa_ws_return_id);
        vector<int> durham_ws_return_id = {dummy_id, jordan_lake_id};
        WwtpDischargeRule wwtp_discharge_durham(
                demand_to_wastewater_fraction_durham,
                durham_ws_return_id);

        vector<vector<int>> wjlwtp_remove_from_to_build_list;// = {{21, 20}};

        vector<int> demand_triggered_infra_order_cary = {cary_low_exp_id, cary_high_exp_id};
        vector<double> demand_infra_cary = {caryupgrades_2 * 7, caryupgrades_3 * 7};
        Utility cary((char *) "Cary", 2, demand_cary, demand_n_weeks, cary_annual_payment, &caryDemandClassesFractions,
                     &caryUserClassesWaterPrices, wwtp_discharge_cary, cary_inf_buffer, vector<int>(),
                     demand_triggered_infra_order_cary, demand_infra_cary, discount_rate, bond_term[2], bond_rate[2]);
        Utility durham((char *) "Durham", 1, demand_durham, demand_n_weeks, durham_annual_payment,
                       &durhamDemandClassesFractions, &durhamUserClassesWaterPrices, wwtp_discharge_durham,
                       durham_inf_buffer, rof_triggered_infra_order_durham,
                       vector<int>(), rofs_infra_durham, discount_rate, wjlwtp_remove_from_to_build_list, bond_term[1], bond_rate[1]);
        Utility owasa((char *) "OWASA", 0, demand_owasa, demand_n_weeks, owasa_annual_payment,
                      &owasaDemandClassesFractions, &owasaUserClassesWaterPrices, wwtp_discharge_owasa, owasa_inf_buffer,
                      rof_triggered_infra_order_owasa,
                      vector<int>(), rofs_infra_owasa, discount_rate, wjlwtp_remove_from_to_build_list, bond_term[0], bond_rate[0]);

        vector<Utility *> utilities;
        utilities.push_back(&cary);
        utilities.push_back(&durham);
        utilities.push_back(&owasa);

        /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
        /// sources IDs.
        vector<vector<int>> reservoir_utility_connectivity_matrix = {
                {ccr_id, univ_lake_id, stone_quarry_id, jordan_lake_id,
                        ccr_exp_id, univ_lake_exp_id, quarry_low_exp_id, quarry_high_exp_id,
                        wjlwtp_low_id, wjlwtp_high_id}, //OWASA
                {lake_michie_id, jordan_lake_id, teer_quarry_id,
                        michie_low_exp_id, michie_high_exp_id, reclaimed_low_id, reclaimed_high_id,
                        wjlwtp_low_id, wjlwtp_high_id}, //Durham
                {jordan_lake_id, cary_low_exp_id, cary_high_exp_id} //Cary
        };

        auto table_storage_shift = std::vector<vector<double>>(3, vector<double>(19, 0.));
        table_storage_shift[1][10] = 100.;
        table_storage_shift[1][16] = 500.;
        table_storage_shift[1][17] = 500.;
        table_storage_shift[1][11] = 700.;
        table_storage_shift[1][5] = 700.;

        vector<DroughtMitigationPolicy *> drought_mitigation_policies;
        /// Restriction policies
        vector<double> initial_restriction_triggers = {OWASA_restriction_trigger,
                                                       Durham_restriction_trigger,
                                                       cary_restriction_trigger};

        vector<double> restriction_stage_multipliers_cary = {0.9, 0.8, 0.7, 0.6};
        vector<double> restriction_stage_triggers_cary = {initial_restriction_triggers[0],
                                                          initial_restriction_triggers[0] + 0.15f,
                                                          initial_restriction_triggers[0] + 0.35f,
                                                          initial_restriction_triggers[0] + 0.6f};
        vector<double> restriction_stage_multipliers_durham = {0.9, 0.8, 0.7, 0.6};
        vector<double> restriction_stage_triggers_durham = {initial_restriction_triggers[1],
                                                            initial_restriction_triggers[1] + 0.15f,
                                                            initial_restriction_triggers[1] + 0.35f,
                                                            initial_restriction_triggers[1] + 0.6f};
        vector<double> restriction_stage_multipliers_owasa = {0.9, 0.8, 0.7};
        vector<double> restriction_stage_triggers_owasa = {initial_restriction_triggers[2],
                                                           initial_restriction_triggers[2] + 0.15f,
                                                           initial_restriction_triggers[2] + 0.35f};


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


        drought_mitigation_policies = {&restrictions_c, &restrictions_d,
                                       &restrictions_o};

        /// Transfer policy
        /*
         *      2
         *     /
         *  0 v
         *   /
         *  1--><--0
         *      1
         */

        vector<int> buyers_ids = {0, 1};
        vector<double> buyers_transfers_capacities = {10.8 * 7, 7.0 * 7};
        vector<double> buyers_transfers_trigger = {owasa_transfer_trigger,
                                                   durham_transfer_trigger};

        Graph ug(3);
        ug.addEdge(2, 1);
        ug.addEdge(1, 0);

        Transfers t(2, 2, jordan_lake_id, 35,
                    buyers_ids,
                    buyers_transfers_capacities,
                    buyers_transfers_trigger,
                    ug,
                    vector<double>(),
                    vector<int>());
        drought_mitigation_policies.push_back(&t);

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
            realization_start = omp_get_wtime();
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
            realization_start = omp_get_wtime();
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
            realization_start = omp_get_wtime();
            this->master_data_collector = s->runFullSimulation(n_threads);
        }

        double realization_end = omp_get_wtime();
        std::cout << "Simulation took  " << realization_end - realization_start
              << "s" << std::endl;

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
}

int Triangle::simulationExceptionHander(const std::exception &e, Simulation *s,
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

Triangle::~Triangle() = default;

Triangle::Triangle(unsigned long n_weeks, int import_export_rof_table)
        : Problem(n_weeks) {
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
Triangle::setRofTables(unsigned long n_realizations, int n_utilities, string rof_tables_directory) {

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
    rof_tables = std::vector<vector<Matrix2D<double>>>(
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

void Triangle::readInputData() {
    //cout << "Reading input data." << endl;

    string test_files_directory = "CDOTestFiles";

#pragma omp parallel num_threads(20)
    {

#pragma omp single
        streamflows_durham = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/inflows/scenario" +
                                                   to_string(scenario) + "/Durham_fullset.csv", n_realizations);
#pragma omp single
        streamflows_phils = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/inflows/scenario" +
                                                  to_string(scenario) + "/PhilsCreek_fullset.csv", n_realizations);
#pragma omp single
        streamflows_cane = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/inflows/scenario" +
                                                 to_string(scenario) + "/CaneCreek_fullset.csv", n_realizations);
#pragma omp single
        streamflows_morgan = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/inflows/scenario" +
                                                   to_string(scenario) + "/MorganCreek_fullset.csv", n_realizations);
#pragma omp single
        streamflows_haw = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/inflows/scenario" +
                                                to_string(scenario) + "/Jordan_fullset.csv", n_realizations);
#pragma omp single
        streamflows_lillington = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory +
                                                       "/inflows/LillingtonInflows_Averages.csv", n_realizations);

#pragma omp single
        evap_durham = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/evaporation/scenario" +
                                            to_string(scenario) + "/michielittle_netEvap_fullset.csv", n_realizations);
#pragma omp single
        evap_owasa = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/evaporation/scenario" +
                                           to_string(scenario) + "/owasaevap_netEvap_fullset.csv", n_realizations);
#pragma omp single
        evap_jordan_lake = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/evaporation/scenario" +
                                                 to_string(scenario) + "/jordanevap_netEvap_fullset.csv", n_realizations);

#pragma omp single
        demand_cary = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/demands/scenario" +
                                            to_string(scenario) + "/cary_adjusteddemand.csv", n_realizations);
#pragma omp single
        demand_durham = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/demands/scenario" +
                                              to_string(scenario) + "/durham_adjusteddemand.csv", n_realizations);
#pragma omp single
        demand_owasa = Utils::parse2DCsvFile(output_directory + "/" + test_files_directory + "/demands/scenario" +
                                             to_string(scenario) + "/owasa_adjusteddemand.csv", n_realizations);

#pragma omp single
        {
            demand_to_wastewater_fraction_owasa_raleigh =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/demand_to_wastewater_fraction_owasa_raleigh.csv");
            demand_to_wastewater_fraction_durham =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/demand_to_wastewater_fraction_durham.csv");

            caryDemandClassesFractions =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/caryDemandClassesFractions.csv");
            durhamDemandClassesFractions =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/durhamDemandClassesFractions.csv");
            owasaDemandClassesFractions =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/owasaDemandClassesFractions.csv");

            caryUserClassesWaterPrices =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/caryUserClassesWaterPrices.csv");
            durhamUserClassesWaterPrices =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/durhamUserClassesWaterPrices.csv");
            owasaUserClassesWaterPrices =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/owasaUserClassesWaterPrices.csv");

            owasaPriceSurcharges =
                    Utils::parse2DCsvFile(output_directory + "/" +
                                          test_files_directory + "/owasaPriceRestMultipliers.csv");
        }

    }

}

void Triangle::setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, string rof_tables_directory) {
    if (std::abs(import_export_rof_tables) > 1)
        __throw_invalid_argument("Import/export ROF tables can be assigned as:\n"
                                         "-1 - import tables\n"
                                         "0 - ignore tables\n"
                                         "1 - export tables.\n"
                                         "The value entered is invalid.");
    Triangle::import_export_rof_tables = import_export_rof_tables;
//    this->rof_tables_directory = output_directory + "/TestFiles/" + rof_tables_directory;
    this->rof_tables_directory = rof_tables_directory;

    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        Triangle::setRofTables(n_realizations, n_utilities, this->rof_tables_directory);
    } else {
        const string mkdir_command = "mkdir";
        //system((mkdir_command + " " + this->rof_tables_directory).c_str());
    }
}
