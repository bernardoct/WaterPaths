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
#include "../SystemComponents/WaterSources/IntakeExpansion.h"

#ifdef PARALLEL
void Triangle::setProblemDefinition(BORG_Problem &problem)
{
    // The parameter bounds are the same for all formulations
    BORG_Problem_set_bounds(problem, 0, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 1, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 2, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 3, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 4, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 5, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 6, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 7, 0.1, 0.47);
    BORG_Problem_set_bounds(problem, 8, 0.0, 0.37);
    BORG_Problem_set_bounds(problem, 9, 0.05, 0.42);
    BORG_Problem_set_bounds(problem, 10, 0.355, 0.725);
    BORG_Problem_set_bounds(problem, 11, 0.0, 0.1);
    BORG_Problem_set_bounds(problem, 12, 0.0, 0.1);
    BORG_Problem_set_bounds(problem, 13, 0.0, 0.1);
    BORG_Problem_set_bounds(problem, 14, 0.0, 0.1);
    BORG_Problem_set_bounds(problem, 15, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 16, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 17, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 18, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 19, 0.0, 0.02);
    BORG_Problem_set_bounds(problem, 20, 0.0, 0.02);
    BORG_Problem_set_bounds(problem, 21, 0.0, 0.02);
    BORG_Problem_set_bounds(problem, 22, 0.0, 0.02);
    BORG_Problem_set_bounds(problem, 23, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 24, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 25, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 26, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 27, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 28, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 29, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 30, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 31, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 32, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 33, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 34, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 35, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 36, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 37, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 38, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 39, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 40, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 41, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 42, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 43, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 44, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 45, 0.0, 1.0);
    BORG_Problem_set_bounds(problem, 46, 0.0, 100.0);
    BORG_Problem_set_bounds(problem, 47, 0.0, 100.0);
    BORG_Problem_set_bounds(problem, 48, 0.0, 100.0);
    BORG_Problem_set_bounds(problem, 49, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 50, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 51, 0.001, 1.0);
    BORG_Problem_set_bounds(problem, 52, 0.0, 10000.0);
    BORG_Problem_set_bounds(problem, 53, 0.0, 20.0);
    BORG_Problem_set_bounds(problem, 54, 0.0, 20.0);
    BORG_Problem_set_bounds(problem, 55, 0.0, 20.0);
    BORG_Problem_set_bounds(problem, 56, 0.0, 20.0);

    // Set epsilons for objectives
    BORG_Problem_set_epsilon(problem, 0, 0.002);
    BORG_Problem_set_epsilon(problem, 1, 0.02);
    BORG_Problem_set_epsilon(problem, 2, 10.);
    BORG_Problem_set_epsilon(problem, 3, 0.02);
    BORG_Problem_set_epsilon(problem, 4, 0.01);
    BORG_Problem_set_epsilon(problem, 5, 0.025);
}
#endif


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
    // ===================== SET UP DECISION VARIABLES  =====================

    Simulation *s = nullptr;
//    try {
	//throw invalid_argument("Test error");

    /// July 2019: added Pittsboro and Chatham County triggers, mapped to OWASA levels for now

        double Durham_restriction_trigger = vars[0];
        double OWASA_restriction_trigger = vars[1];
        double raleigh_restriction_trigger = vars[2];
        double cary_restriction_trigger = vars[3];
        double pittsboro_restriction_trigger = OWASA_restriction_trigger;
        double chatham_restriction_trigger = OWASA_restriction_trigger;

        double durham_transfer_trigger = vars[4];
        double owasa_transfer_trigger = vars[5];
        double raleigh_transfer_trigger = vars[6];
        double pittsboro_transfer_trigger = owasa_transfer_trigger;
        double chatham_transfer_trigger = owasa_transfer_trigger;

        double OWASA_JLA = vars[7];
        double Raleigh_JLA = vars[8];
        double Durham_JLA = vars[9];
        double Cary_JLA = vars[10];
        double Pittsboro_JLA = OWASA_JLA;
        double Chatham_JLA = OWASA_JLA;

        double durham_annual_payment = vars[11]; // contingency fund
        double owasa_annual_payment = vars[12];
        double raleigh_annual_payment = vars[13];
        double cary_annual_payment = vars[14];
        double pittsboro_annual_payment = owasa_annual_payment;
        double chatham_annual_payment = owasa_annual_payment;

        double durham_insurance_use = vars[15]; // insurance st_rof
        double owasa_insurance_use = vars[16];
        double raleigh_insurance_use = vars[17];
        double cary_insurance_use = vars[18];
        double pittsboro_insurance_use = owasa_insurance_use;
        double chatham_insurance_use = owasa_insurance_use;

        double durham_insurance_payment = vars[19];
        double owasa_insurance_payment = vars[20];
        double raleigh_insurance_payment = vars[21];
        double cary_insurance_payment = vars[22];
        double pittsboro_insurance_payment = owasa_insurance_payment;
        double chatham_insurance_payment = owasa_insurance_payment;

        double durham_inftrigger = vars[23];
        double owasa_inftrigger = vars[24];
        double raleigh_inftrigger = vars[25];
        double cary_inftrigger = vars[26];
        double pittsboro_inftrigger = owasa_inftrigger;
        double chatham_inftrigger = owasa_inftrigger;
        if (import_export_rof_tables == EXPORT_ROF_TABLES) {
            durham_inftrigger = 1.1;
            owasa_inftrigger = 1.1;
            raleigh_inftrigger = 1.1;
            cary_inftrigger = 1.1;
            pittsboro_inftrigger = 1.1;
            chatham_inftrigger = 1.1;
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
        double little_river_reservoir_ranking = vars[36]; // 7
        double richland_creek_quarry_rank = vars[37]; // 8
        double neuse_river_intake_rank = vars[38]; // 10
        double reallocate_falls_lake_rank = vars[39]; // 17
        double western_wake_treatment_plant_rank_OWASA_low = vars[40]; // 20
        double western_wake_treatment_plant_rank_OWASA_high = vars[41]; // 21
        double western_wake_treatment_plant_rank_durham_low = vars[42]; // 20
        double western_wake_treatment_plant_rank_durham_high = vars[43]; // 21
        double western_wake_treatment_plant_rank_raleigh_low = vars[44]; // 20
        double western_wake_treatment_plant_rank_raleigh_high = vars[45]; // 21
    //    double caryupgrades_1 = vars[46]; // not used: already built.
        double caryupgrades_2 = vars[47];
        double caryupgrades_3 = vars[48];
        double western_wake_treatment_plant_owasa_frac = vars[49];
        double western_wake_treatment_frac_durham = vars[50];
        double western_wake_treatment_plant_raleigh_frac = vars[51];
        double falls_lake_reallocation = vars[52];

        double durham_inf_buffer = vars[53];
        double owasa_inf_buffer = vars[54];
        double raleigh_inf_buffer = vars[55];
        double cary_inf_buffer = vars[56];
        double pittsboro_inf_buffer = 2;
        double chatham_inf_buffer = 2;

        /// July 2019: pittsboro and chatham county infrastructure projects
        ///             where there are references to input vars, owasa values are used as placeholders
        // FIXME: THESE ARE HARD-CODED VALUES FOR NOW THROUGHOUT TO BE LINKED TO DECISION VARIABLES
        double western_wake_treatment_plant_pittsboro_frac = 0.167; // 16.7% of ultimate 2060 capacity
        double western_wake_treatment_plant_chatham_frac = 0.296; // 29.6% of ultimate 2060 capacity
        double western_wake_treatment_plant_rank_pittsboro_low = vars[40]; // 20
        double western_wake_treatment_plant_rank_pittsboro_high = vars[41]; // 21
        double western_wake_treatment_plant_rank_chatham_low = vars[40]; // 20
        double western_wake_treatment_plant_rank_chatham_high = vars[41]; // 21

        /// potential independent Pittsboro project(s)
        double haw_river_intake_expansion_rank_low = 0.9;
        double haw_river_intake_expansion_rank_high = 0.9;

        /// potential independent Chatham County project(s)
        double cape_fear_river_intake_rank = 0.8;

        /// July 2019: Because of the way infrastructure connectivity needs to be IDd
        ///     this sub-section of ID variables helps keep things in order
        /// reminder that geographically-independent sources (built or potential)
        /// must be linked in the graph below and must take the id values 0 ---> {# of sources}
        /// before expansions or other potential projects are numbered

        // Existing sources
        int durham_reservoirs_id = 0;
        int falls_lake_id = 1;
        int wheeler_benson_id = 2;
        int stone_quarry_id = 3;
        int cane_creek_reservoir_id = 4;
        int university_lake_id = 5;
        int jordan_lake_id = 6;
        int haw_river_intake_id = 7;
        int dummy_node_id = 13;

        // Potential sources (geographically separate from above)
        int little_river_raleigh_reservoir_id = 8;
        int richland_creek_quarry_id = 9;
        int teer_quarry_id = 10;
        int neuse_river_intake_id = 11;
        int harnett_county_intake_id = 12;

        // Potential sources (expansions of above sources)
        int stone_quarry_expansion_low_id = 14;
        int stone_quarry_expansion_high_id = 15;
        int university_lake_expansion_id = 16;
        int michie_expansion_low_id = 17;
        int michie_expansion_high_id = 18;
        int falls_lake_reallocation_id = 19;
        int reclaimed_water_low_id = 20;
        int reclaimed_water_high_id = 21;
        int cane_creek_reservoir_expansion_id = 22;
        int haw_river_intake_expansion_low_id = 23;
        int haw_river_intake_expansion_high_id = 24;
        int cary_wtp_upgrades_low_base_id = 25;
        int cary_wtp_upgrades_high_base_id = 26;
        int wjlwtp_low_base_id = 27;
        int wjlwtp_high_base_id = 28;

        vector<infraRank> pittsboro_infra_order_raw = {
                infraRank(haw_river_intake_expansion_low_id, haw_river_intake_expansion_rank_low),
                infraRank(haw_river_intake_expansion_high_id, haw_river_intake_expansion_rank_high),
                infraRank(wjlwtp_low_base_id, western_wake_treatment_plant_rank_pittsboro_low),
                infraRank(wjlwtp_high_base_id, western_wake_treatment_plant_rank_pittsboro_high)
        };

        vector<infraRank> chatham_infra_order_raw = {
                infraRank(harnett_county_intake_id, cape_fear_river_intake_rank),
                infraRank(wjlwtp_low_base_id, western_wake_treatment_plant_rank_chatham_low),
                infraRank(wjlwtp_high_base_id, western_wake_treatment_plant_rank_chatham_high)
        };

        vector<infraRank> durham_infra_order_raw = {
                infraRank(teer_quarry_id, Teer_quarry_expansion_ranking),
                infraRank(michie_expansion_low_id, lake_michie_expansion_ranking_low),
                infraRank(michie_expansion_high_id, lake_michie_expansion_ranking_high),
                infraRank(reclaimed_water_low_id, reclaimed_water_ranking_low),
                infraRank(reclaimed_water_high_id, reclaimed_water_high),
                infraRank(wjlwtp_low_base_id, western_wake_treatment_plant_rank_durham_low),
                infraRank(wjlwtp_high_base_id, western_wake_treatment_plant_rank_durham_high)
        };

        vector<infraRank> owasa_infra_order_raw = {
                infraRank(stone_quarry_expansion_low_id, Stone_quarry_reservoir_expansion_shallow_ranking),
                infraRank(stone_quarry_expansion_high_id, Stone_quarry_reservoir_expansion_deep_ranking),
                infraRank(university_lake_expansion_id, university_lake_expansion_ranking),
                infraRank(cane_creek_reservoir_expansion_id, Cane_creek_expansion_ranking),
                infraRank(wjlwtp_low_base_id, western_wake_treatment_plant_rank_OWASA_low),
                infraRank(wjlwtp_high_base_id, western_wake_treatment_plant_rank_OWASA_high)
        };

        vector<infraRank> raleigh_infra_order_raw = {
                infraRank(little_river_raleigh_reservoir_id, little_river_reservoir_ranking),
                infraRank(richland_creek_quarry_id, richland_creek_quarry_rank),
                infraRank(neuse_river_intake_id, neuse_river_intake_rank),
                infraRank(falls_lake_reallocation_id, reallocate_falls_lake_rank),
                infraRank(wjlwtp_low_base_id, western_wake_treatment_plant_rank_raleigh_low),
                infraRank(wjlwtp_high_base_id, western_wake_treatment_plant_rank_raleigh_high)
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
        sort(raleigh_infra_order_raw.begin(),
             raleigh_infra_order_raw.end(),
             by_xreal());

        sort(pittsboro_infra_order_raw.begin(),
             pittsboro_infra_order_raw.end(),
             by_xreal());
        sort(chatham_infra_order_raw.begin(),
             chatham_infra_order_raw.end(),
             by_xreal());

        vector<int> rof_triggered_infra_order_durham =
                vecInfraRankToVecInt(durham_infra_order_raw);
        vector<int> rof_triggered_infra_order_owasa =
                vecInfraRankToVecInt(owasa_infra_order_raw);
        vector<int> rof_triggered_infra_order_raleigh =
                vecInfraRankToVecInt(raleigh_infra_order_raw);

        vector<int> rof_triggered_infra_order_pittsboro =
                vecInfraRankToVecInt(pittsboro_infra_order_raw);
        vector<int> rof_triggered_infra_order_chatham =
                vecInfraRankToVecInt(chatham_infra_order_raw);

    // Create vectors with each utility's long-term ROF values assigned to all
    // infrastructure options.
    vector<double> rofs_infra_durham = vector<double>
            (rof_triggered_infra_order_durham.size(), durham_inftrigger);
    vector<double> rofs_infra_owasa = vector<double>
            (rof_triggered_infra_order_owasa.size(), owasa_inftrigger);
    vector<double> rofs_infra_raleigh = vector<double>
            (rof_triggered_infra_order_raleigh.size(), raleigh_inftrigger);

    vector<double> rofs_infra_pittsboro = vector<double>
            (rof_triggered_infra_order_pittsboro.size(), pittsboro_inftrigger);
    vector<double> rofs_infra_chatham = vector<double>
            (rof_triggered_infra_order_chatham.size(), chatham_inftrigger);

        /// Remove small expansions being built after big expansions that would
        /// encompass the smal expansions.
        added_storage_michie_expansion_high =
                checkAndFixInfraExpansionHighLowOrder(
                        &rof_triggered_infra_order_durham,
                        &rofs_infra_durham,
                        michie_expansion_low_id,
                        michie_expansion_high_id,
                        added_storage_michie_expansion_low,
                        added_storage_michie_expansion_high);

        reclaimed_capacity_high =
                checkAndFixInfraExpansionHighLowOrder(
                        &rof_triggered_infra_order_durham,
                        &rofs_infra_durham,
                        reclaimed_water_low_id,
                        reclaimed_water_high_id,
                        reclaimed_capacity_low,
                        reclaimed_capacity_high);

        /// July 2019: two levels of Haw River Intake expansion possible
        double added_capacity_haw_river_intake_expansion_low = 2.0 * 7;
        double added_capacity_haw_river_intake_expansion_high = 2.0 * 7;

        added_capacity_haw_river_intake_expansion_high =
                checkAndFixInfraExpansionHighLowOrder(
                        &rof_triggered_infra_order_pittsboro,
                        &rofs_infra_pittsboro,
                        haw_river_intake_expansion_low_id,
                        haw_river_intake_expansion_high_id,
                        added_capacity_haw_river_intake_expansion_low,
                        added_capacity_haw_river_intake_expansion_high);

        /// Normalize Jordan Lake Allocations in case they exceed 1.
        double sum_jla_allocations = OWASA_JLA + Durham_JLA + Cary_JLA +
                                     Raleigh_JLA + Pittsboro_JLA + Chatham_JLA;
        if (sum_jla_allocations == 0.)
            throw invalid_argument("JLA allocations cannot be all "
                                             "zero.");
        // At the time this ORIGINAL study was done, 31% of JL had been allocated to other utilities.
        /// July 2019: with Pittsboro and Chatham County added, almost all of JLAs are accounted for
        ///     only missing smaller communities of Holly Springs, Hillsborough and Orange County (about 5% of 100 MGD)
        double total_allocated_fraction = 1 - 0.05;
        if (sum_jla_allocations > total_allocated_fraction) {
            OWASA_JLA /= sum_jla_allocations / total_allocated_fraction;
            Durham_JLA /= sum_jla_allocations / total_allocated_fraction;
            Cary_JLA /= sum_jla_allocations / total_allocated_fraction;
            Raleigh_JLA /= sum_jla_allocations / total_allocated_fraction;

            Pittsboro_JLA /= sum_jla_allocations / total_allocated_fraction;
            Chatham_JLA /= sum_jla_allocations / total_allocated_fraction;
        }

        /// Normalize West Jordan Lake WTP allocations.
        double sum_wjlwtp = western_wake_treatment_frac_durham +
                            western_wake_treatment_plant_owasa_frac +
                            western_wake_treatment_plant_raleigh_frac +
                            western_wake_treatment_plant_pittsboro_frac +
                            western_wake_treatment_plant_chatham_frac;
        if (sum_wjlwtp == 0.)
            throw invalid_argument("Treatment fractions for WJLWTP cannot be all "
                                             "zero.");
        western_wake_treatment_frac_durham /= sum_wjlwtp;
        western_wake_treatment_plant_owasa_frac /= sum_wjlwtp;
        western_wake_treatment_plant_raleigh_frac /= sum_wjlwtp;

        western_wake_treatment_plant_pittsboro_frac /= sum_wjlwtp;
        western_wake_treatment_plant_chatham_frac /= sum_wjlwtp;


        // ==================== SET UP RDM FACTORS ============================

        if (utilities_rdm.empty()) {
            /// All matrices below have dimensions n_realizations x nr_rdm_factors
            utilities_rdm = std::vector<vector<double>>(
                    n_realizations, vector<double>(6, 1.));
            water_sources_rdm = std::vector<vector<double>>(
                    n_realizations, vector<double>(60, 1.));
            policies_rdm = std::vector<vector<double>>(
                    n_realizations, vector<double>(8, 1.));
        }


        // ===================== SET UP PROBLEM COMPONENTS =====================

        //cout << "BEGINNING TRIANGLE TEST" << endl << endl;
        // cout << "Using " << omp_get_num_threads() << " cores." << endl;
    //    cout << getexepath() << endl;

        /// Read streamflows
        int streamflow_n_weeks = 52 * (70 + 50);

        /// In case a vector containing realizations numbers to be calculated is passed, set
        /// number of realizations to number of realizations in that vector.

    //    vector<double> sewageFractions = Utils::parse1DCsvFile(
    //            io_directory + "/TestFiles/sewageFractions.csv");

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

        SeasonalMinEnvFlowControl durham_min_env_control(durham_reservoirs_id, dlr_weeks,
                                                         dlr_releases);
    //    FixedMinEnvFlowControl falls_min_env_control(1, 38.78 * 7);
        FallsLakeMinEnvFlowControl falls_min_env_control(falls_lake_id,
                                                         neuse_river_intake_id,
                                                         falls_controls_weeks,
                                                         falls_base_releases,
                                                         falls_min_gage,
                                                         crabtree_creek);

        StorageMinEnvFlowControl wheeler_benson_min_env_control(wheeler_benson_id,
                                                                vector<int>(1, wheeler_benson_id),
                                                                wb_storage,
                                                                wb_releases);
        FixedMinEnvFlowControl sq_min_env_control(stone_quarry_id, 0);
        InflowMinEnvFlowControl ccr_min_env_control(cane_creek_reservoir_id,
                                                    ccr_inflows,
                                                    ccr_releases);
        FixedMinEnvFlowControl university_min_env_control(university_lake_id, 0);
    //    FixedMinEnvFlowControl jordan_min_env_control(6,
    //                                                  25.8527 * 7);
        JordanLakeMinEnvFlowControl jordan_min_env_control(
                jordan_lake_id, cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
                290.84, 387.79, 30825.0, 14924.0);
        SeasonalMinEnvFlowControl little_river_min_env_control(little_river_raleigh_reservoir_id, dlr_weeks,
                                                               dlr_releases);
        FixedMinEnvFlowControl richland_min_env_control(richland_creek_quarry_id, 0);
        FixedMinEnvFlowControl teer_min_env_control(teer_quarry_id, 0);
        FixedMinEnvFlowControl neuse_intake_min_env_control(neuse_river_intake_id, 38.78 * 7);

        // FIXME: MAY NEED INTAKE CONTROLS FOR PITTSBORO AND CHATHAM HERE

    //    vector<int> eno_weeks = {7, 16, 53};
    //    vector<double> eno_releases = {6.49 * 7, 19.48 * 7, 6.49 * 7};
    //    SeasonalMinEnvFlowControl eno_min_env_control(&eno_weeks, &eno_releases);

        vector<MinEnvFlowControl *> min_env_flow_controls;
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
        /// July 2019: INCREASED SIZE OF ROF TRIGGER AND BOND VECTORS FOR PITTSBORO AND CHATHAM COUNTY ADDITIONS
        vector<double> construction_time_interval = {3.0, 5.0};
        vector<double> city_infrastructure_rof_triggers = {owasa_inftrigger,
                                                           durham_inftrigger,
                                                           cary_inftrigger,
                                                           raleigh_inftrigger,
                                                           chatham_inftrigger,
                                                           pittsboro_inftrigger};
        vector<double> bond_term = {25, 25, 25, 25, 25, 25};
        vector<double> bond_rate = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
        double discount_rate = 0.05;

        /// Jordan Lake parameters
        double jl_supply_capacity = 14924.0 * table_gen_storage_multiplier;
        double jl_wq_capacity = 30825.0 * table_gen_storage_multiplier;
        double jl_storage_capacity = jl_wq_capacity + jl_supply_capacity;
        vector<int> jl_allocations_ids = {0, 1, 2, 3, 4, 5, WATER_QUALITY_ALLOCATION}; // added pittsboro/cc
        vector<double> jl_allocation_fractions = {
                OWASA_JLA * jl_supply_capacity / jl_storage_capacity,
                Durham_JLA * jl_supply_capacity / jl_storage_capacity,
                Cary_JLA * jl_supply_capacity / jl_storage_capacity,
                Raleigh_JLA * jl_supply_capacity / jl_storage_capacity,
                Pittsboro_JLA * jl_supply_capacity / jl_storage_capacity,
                Chatham_JLA * jl_supply_capacity / jl_storage_capacity,
                jl_wq_capacity / jl_storage_capacity};

        /// July 2019: with Pittsboro (5th) and Chatham County (6th) added, Chatham's existing treatment capacity
        ///             on JL must be accounted for (i.e. not just Cary has initial capacity to treat JL water)
        ///             64 MGD for Cary/Apex/Morrisville WTP, 3 MGD permitting for Chatham County WTP
        ///             FIXME: IS THIS THE CORRECT LEVEL OF WTP CAPACITY FOR CARY????
        vector<double> jl_treatment_allocation_fractions = {0.0, 0.0, 0.955224, 0.0, 0.044776, 0.0};

        /// Falls Lake parameters
        double fl_supply_capacity = 14700.0 * table_gen_storage_multiplier;
        double fl_wq_capacity = 20000.0 * table_gen_storage_multiplier;
        double fl_storage_capacity = fl_wq_capacity + fl_supply_capacity;
        vector<int> fl_allocations_ids = {3, WATER_QUALITY_ALLOCATION};
        vector<double> fl_allocation_fractions = {
                fl_supply_capacity / fl_storage_capacity,
                fl_wq_capacity / fl_storage_capacity};
//        vector<double> fl_treatment_allocation_fractions = {0.0, 0.0, 0.0, 1.0};
        vector<double> fl_treatment_allocation_fractions = {1.0};

        /// July 2019: Pittsboro and Chatham County draw currently from the (X1) Haw River and (6) Jordan Lake
        ///             respectively. Pittsboro has the option to expand its Haw intake (X1a) or partner up on JL,
        ///             while Chatham can join the JL coalition and/or build another intake south of JL on Cape Fear River (X2)
        /// ALSO: CHANGED THE DIAGRAM TO REFLECT NAMES OF SOURCES RATHER THAN HARD-CODED ID NUMBERS
        ///         SEE THE ABOVE ASSIGNMENTS TO UNDERSTAND HOW THE SYSTEM CONNECTIVITY WORKS
        ///         OLD NUMBERS WERE KEPT FOR CONSISTENCY WITH PAST MODEL DEVELOPMENT AND TO DOUBLE-CHECK ASSIGNMENT ERRORS

        Graph g(14); // number of geographically-separate sources (built or unbuilt) ignoring reclaimed water

        // Durham ---> Raleigh Flow
        g.addEdge(durham_reservoirs_id, teer_quarry_id);
        g.addEdge(teer_quarry_id, falls_lake_id);
        g.addEdge(falls_lake_id, richland_creek_quarry_id);
        g.addEdge(richland_creek_quarry_id, neuse_river_intake_id);
        g.addEdge(neuse_river_intake_id, dummy_node_id);

        // Raleigh Alternate Flow
        g.addEdge(wheeler_benson_id, dummy_node_id);
        g.addEdge(little_river_raleigh_reservoir_id, dummy_node_id);

        // OWASA ---> Cary Flow
        g.addEdge(stone_quarry_id, university_lake_id);
        g.addEdge(cane_creek_reservoir_id, haw_river_intake_id);
        g.addEdge(university_lake_id, jordan_lake_id);

        // Pittsboro and Chatham County --> Cary Alternate Flow
        g.addEdge(haw_river_intake_id, jordan_lake_id);
        g.addEdge(jordan_lake_id, harnett_county_intake_id);
        g.addEdge(harnett_county_intake_id, dummy_node_id);

        /*
         * System connection diagram (water
         * flows from top to bottom)
         * Potential projects and expansions
         * of existing sources in parentheses
         *
         *
         *      Haw River       Stone Quarry 3(12,13)
         *          |                  |
         *          |        Cane     Univ.            Michie+     Reclaimed
         *          |       Cr. Res.  Lake             LRR         Water
         *          |         4(24)   5(14)            0(15,16)    (18,19)
         *           \         /      /                  |
         *            \       /      /                   |
         *             \     /      /                    |
         *              \   /      /                    (9) Teer Quarry
         *               \ /      /                      \
         *                \      /                        \            WB Lakes    LR Res. Raleigh
         *   Haw Riv.  X1(X1a)  /              Falls Lake  1(17)              2   (7)
         *    Intake      |    /                             |                |    |
         *                |   /                              |                |    |
         *             6(20-25) Jordan Lake        Richland (8) Quarry        |    |
         *                |                                   \               |    |
         *       Harnett (X2) Intake                          |               |    |
         *                |                                    \              |    |
         *          Lillington Gage                Neuse River (10) Intake    |    |
         *                |                                      |            |    |
         *                |                                      |           /    /
         *                |                                      |          /    /
         *                |                                 Clayton Gage   /    /
         *                |                                      |        /    /
         *                 \                                     |   -----    /
         *                  \                                     \ /        /
         *                   \                                     |    -----
         *                    \                                    |   /
         *                     \                                   |  /
         *                      \                                   \/
         *                       -------                            /
         *                              \             --------------
         *                               \           /
         *                                \     -----
         *                                 \   /
         *                                  \ /
         *                                  11 Dummy Endpoint
         */


//        Graph g(12);
//        g.addEdge(0, 9);
//        g.addEdge(9, 1);
//        g.addEdge(1, 8);
//        g.addEdge(8, 10);
//        g.addEdge(10, 11);
//        g.addEdge(2, 11);
//        g.addEdge(7, 11);
//
//        g.addEdge(3, 6);
//        g.addEdge(4, 6);
//        g.addEdge(5, 6);
//        g.addEdge(6, 11);

        // Existing Sources
        Reservoir durham_reservoirs("Lake Michie & Little River Res. (Durham)",
                                    durham_reservoirs_id,
                                    catchment_durham,
                                    6349.0 * table_gen_storage_multiplier,
                                    ILLIMITED_TREATMENT_CAPACITY,
                                    evaporation_durham, 1069);
    //    Reservoir falls_lake("Falls Lake", 1, catchment_flat,
    //                         34700.0, 99999,
    //                         &evaporation_falls_lake, &falls_lake_storage_area);
        AllocatedReservoir falls_lake("Falls Lake",
                                      falls_lake_id,
                                      catchment_flat,
                                      fl_storage_capacity,
                                      ILLIMITED_TREATMENT_CAPACITY,
                                      evaporation_falls_lake,
                                      &falls_lake_storage_area,
                                      &fl_allocations_ids,
                                      &fl_allocation_fractions,
                                      &fl_treatment_allocation_fractions);

        Reservoir wheeler_benson_lakes("Wheeler-Benson Lakes", wheeler_benson_id, catchment_swift,
                                       2789.66 * table_gen_storage_multiplier,
                                       ILLIMITED_TREATMENT_CAPACITY,
                                       evaporation_wheeler_benson,
                                       &wheeler_benson_storage_area);
        Reservoir stone_quarry("Stone Quarry",
                               stone_quarry_id,
                               catchment_phils,
                               200.0 * table_gen_storage_multiplier,
                               ILLIMITED_TREATMENT_CAPACITY,
                               evaporation_owasa,
                               10);
        Reservoir ccr("Cane Creek Reservoir",
                      cane_creek_reservoir_id,
                      catchment_cane,
                      2909.0 * table_gen_storage_multiplier,
                      ILLIMITED_TREATMENT_CAPACITY,
                      evaporation_owasa,
                      500);
        Reservoir university_lake("University Lake", university_lake_id, catchment_morgan,
                                  449.0 * table_gen_storage_multiplier,
                                  ILLIMITED_TREATMENT_CAPACITY,
                                  evaporation_owasa,
                                  212);
        AllocatedReservoir jordan_lake("Jordan Lake",
                                       jordan_lake_id,
                                       catchment_haw,
                                       jl_storage_capacity,
                                       469, // was 448, now 469 to reflect Cary (64 MGD) and Chatham (3 MGD) treatment capacity
                                       evaporation_jordan_lake,
                                       13940,
                                       &jl_allocations_ids,
                                       &jl_allocation_fractions,
                                       &jl_treatment_allocation_fractions);

        // other than Cary WTP for Jordan Lake, assume no WTP constraints - each
        // city can meet its daily demands with available treatment infrastructure

        /// Pittsboro Haw River Intake with 2.0 MGD capacity
        Intake haw_river_intake("Haw River Intake", haw_river_intake_id, catchment_haw, 2.0*7, 2.0*7);

        // Potential Sources
        // FIXME: THIS EXPLANATION IS NO LONGER CONSISTENT WITH THE CODE
        // The capacities listed here for expansions are what additional capacity is gained relative to existing capacity,
        // NOT the total capacity after the expansion is complete. For infrastructure with a high and low option, this means
        // the capacity for both is relative to current conditions - if Lake Michie is expanded small it will gain 2.5BG,
        // and a high expansion will provide 7.7BG more water than current. if small expansion happens, followed by a large
        // expansion, the maximum capacity through expansion is 7.7BG, NOT 2.5BG + 7.7BG.
        LevelDebtServiceBond lrr_bond(little_river_raleigh_reservoir_id, 263.0, 25, 0.05, vector<int>(1, 0));
        Reservoir little_river_reservoir("Little River Reservoir (Raleigh)", little_river_raleigh_reservoir_id, catchment_little_river_raleigh, 3700.0,
                                         ILLIMITED_TREATMENT_CAPACITY, evaporation_little_river, &little_river_storage_area,
                                         construction_time_interval, 17 * WEEKS_IN_YEAR, lrr_bond);

        LevelDebtServiceBond rcq_bond(richland_creek_quarry_id, 400.0, 25, 0.05, vector<int>(1, 0));
        Quarry richland_creek_quarry("Richland Creek Quarry", richland_creek_quarry_id, gage_clayton, 4000.0, ILLIMITED_TREATMENT_CAPACITY,
                                     evaporation_falls_lake, 100., construction_time_interval, 17 * WEEKS_IN_YEAR, rcq_bond,
                                     50. * 7);

        BalloonPaymentBond tq_bond(teer_quarry_id, 22.6, 25, 0.05, vector<int>(1, 0), 3);
        Quarry teer_quarry("Teer Quarry", teer_quarry_id, vector<Catchment *>(), 1315.0, ILLIMITED_TREATMENT_CAPACITY,
                           evaporation_falls_lake, &teer_storage_area, construction_time_interval, 7 * WEEKS_IN_YEAR, tq_bond,
                           15 * 7);

        LevelDebtServiceBond neuse_bond(neuse_river_intake_id, 225.5, 25, 0.05, vector<int>(1, 0));
        Intake neuse_river_intake("Neuse River Intake", neuse_river_intake_id, catchment_flat, 16 * 7, construction_time_interval,
                                  17 * WEEKS_IN_YEAR, neuse_bond);

//        auto it10 = std::find(rof_triggered_infra_order_raleigh.begin(),
//                             rof_triggered_infra_order_raleigh.end(), 10);
//        rof_triggered_infra_order_raleigh.erase(it10);

        // diversions to Teer Quarry for Durham based on inflows to downstream Falls Lake from the Flat River
        // FYI: spillage from Eno River also helps determine Teer quarry diversion, but Eno spillage isn't factored into
        // downstream water balance?


        // FIXME: ADJUST TO ACCESS THE BUILT_SEQUENCE/CONNECTED_SOURCES PARAMETERS IN THE CONSTRUCTORS
        // SO THAT SEQUENTIAL PROJECTS ARE APPLIED PROPERLY??
        /// July 2019: three new Pittsboro/Chatham projects:
        ///             (1ab) two stages of expansion to existing Haw River Intake for Pittsboro, above JL
        ///             (2)   construction of intake for Chatham County on Cape Fear river below JL, above Lillington
        LevelDebtServiceBond low_haw_river_intake_expansion_bond(haw_river_intake_expansion_low_id, 14.2, 25, 0.05, vector<int>(1, 0), 3);
        IntakeExpansion haw_river_intake_expansion_low("Haw River Intake Low Expansion", haw_river_intake_expansion_low_id, haw_river_intake_id,
                                                       added_capacity_haw_river_intake_expansion_low, added_capacity_haw_river_intake_expansion_low,
                                                       construction_time_interval, 7 * WEEKS_IN_YEAR, low_haw_river_intake_expansion_bond);

        LevelDebtServiceBond high_haw_river_intake_expansion_bond(haw_river_intake_expansion_high_id, 28.4, 25, 0.05, vector<int>(1, 0), 3);
        IntakeExpansion haw_river_intake_expansion_high("Haw River Intake High Expansion", haw_river_intake_expansion_high_id, haw_river_intake_id,
                                                        added_capacity_haw_river_intake_expansion_high, added_capacity_haw_river_intake_expansion_high,
                                                        construction_time_interval, 7 * WEEKS_IN_YEAR, high_haw_river_intake_expansion_bond);

        LevelDebtServiceBond cape_fear_river_intake_bond(harnett_county_intake_id, 221.43, 25, 0.05, vector<int>(1, 0), 3);
        Intake cape_fear_river_intake("Cape Fear River Intake", harnett_county_intake_id, catchment_haw, 12.2 * 7, construction_time_interval,
                                  13 * WEEKS_IN_YEAR, cape_fear_river_intake_bond); // possible by 2031

        vector<double> fl_relocation_fractions = {
                (fl_supply_capacity + falls_lake_reallocation) /
                fl_storage_capacity,
                (fl_wq_capacity - falls_lake_reallocation) / fl_storage_capacity};

        LevelDebtServiceBond fl_bond(falls_lake_reallocation_id, 68.2, 25, 0.05, vector<int>(1, 0));
        Relocation fl_reallocation("Falls Lake Reallocation", falls_lake_reallocation_id, falls_lake_id, &fl_relocation_fractions, &fl_allocations_ids,
                                   construction_time_interval, 12 * WEEKS_IN_YEAR, fl_bond);

        LevelDebtServiceBond ccr_exp_bond(cane_creek_reservoir_expansion_id, 127.0, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion ccr_expansion("Cane Creek Reservoir Expansion", cane_creek_reservoir_expansion_id, cane_creek_reservoir_id, 3000.0, construction_time_interval,
                                         17 * WEEKS_IN_YEAR, ccr_exp_bond);

        LevelDebtServiceBond low_sq_exp_bond(stone_quarry_expansion_low_id, 1.4, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion low_sq_expansion("Low Stone Quarry Expansion", stone_quarry_expansion_low_id, stone_quarry_id, 1500.0, construction_time_interval,
                                            23 * WEEKS_IN_YEAR, low_sq_exp_bond);

        LevelDebtServiceBond high_sq_exp_bond(stone_quarry_expansion_high_id, 64.6, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion high_sq_expansion("High Stone Quarry Expansion", stone_quarry_expansion_high_id, stone_quarry_id, 2200.0, construction_time_interval,
                                             23 * WEEKS_IN_YEAR, high_sq_exp_bond);

        LevelDebtServiceBond ul_exp_bond(university_lake_expansion_id, 107.0, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion univ_lake_expansion("University Lake Expansion", university_lake_expansion_id, university_lake_id, 2550.0, construction_time_interval,
                                               17 * WEEKS_IN_YEAR, ul_exp_bond);

        LevelDebtServiceBond low_mi_exp_bond(michie_expansion_low_id, 158.3, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion low_michie_expansion("Low Lake Michie Expansion", michie_expansion_low_id, durham_reservoirs_id, added_storage_michie_expansion_low,
                                                construction_time_interval, 17 * WEEKS_IN_YEAR, low_mi_exp_bond);

        LevelDebtServiceBond high_mi_exp_bond(michie_expansion_high_id, 203.3, 25, 0.05, vector<int>(1, 0));
        ReservoirExpansion high_michie_expansion("High Lake Michie Expansion", michie_expansion_high_id, durham_reservoirs_id, added_storage_michie_expansion_high,
                                                 construction_time_interval, 17 * WEEKS_IN_YEAR, high_mi_exp_bond);

        LevelDebtServiceBond low_rec_bond(reclaimed_water_low_id, 27.5, 25, 0.05, vector<int>(1, 0));
        WaterReuse low_reclaimed("Low Reclaimed Water System", reclaimed_water_low_id, reclaimed_capacity_low, construction_time_interval,
                                 7 * WEEKS_IN_YEAR, low_rec_bond);

        LevelDebtServiceBond high_rec_bond(reclaimed_water_high_id, 104.4, 25, 0.05, vector<int>(1, 0));
        WaterReuse high_reclaimed("High Reclaimed Water System", reclaimed_water_high_id, reclaimed_capacity_high, construction_time_interval,
                                  7 * WEEKS_IN_YEAR, high_rec_bond);

        /// July 2019: add Pittsboro and Chatham County considerations here...
        vector<double> wjlwtp_treatment_capacity_fractions =
                {western_wake_treatment_plant_owasa_frac,
                 western_wake_treatment_frac_durham,
                 0., // cary
                 western_wake_treatment_plant_raleigh_frac,
                 western_wake_treatment_plant_chatham_frac,
                 western_wake_treatment_plant_pittsboro_frac,
                 0.}; // water quality pool
        vector<double> cary_upgrades_treatment_capacity_fractions = {0., 0., 1., 0., 0., 0., 0.};

        vector<double> capacities_wjlwtp_upgrade_1 = {
                33 * 7 * western_wake_treatment_plant_owasa_frac,
                33 * 7 * western_wake_treatment_frac_durham,
                0.,
                33 * 7 * western_wake_treatment_plant_raleigh_frac,
                33 * 7 * western_wake_treatment_plant_chatham_frac,
                33 * 7 * western_wake_treatment_plant_pittsboro_frac
        };
        vector<double> cost_wjlwtp_upgrade_1 = {
                243.3 * western_wake_treatment_plant_owasa_frac,
                243.3 * western_wake_treatment_frac_durham,
                0.,
                243.3 * western_wake_treatment_plant_raleigh_frac,
                243.3 * western_wake_treatment_plant_chatham_frac,
                243.3 * western_wake_treatment_plant_pittsboro_frac
        };
        vector<double> capacities_wjlwtp_upgrade_2 = {
                (54 * 7 - 33 * 7) * western_wake_treatment_plant_owasa_frac,
                (54 * 7 - 33 * 7) * western_wake_treatment_frac_durham,
                0.,
                (54 * 7 - 33 * 7) * western_wake_treatment_plant_raleigh_frac,
                (54 * 7 - 33 * 7) * western_wake_treatment_plant_chatham_frac,
                (54 * 7 - 33 * 7) * western_wake_treatment_plant_pittsboro_frac
        };
        vector<double> cost_wjlwtp_upgrade_2 = {
                (316.8 - 243.3) * western_wake_treatment_plant_owasa_frac,
                (316.8 - 243.3) * western_wake_treatment_frac_durham,
                0.,
                (316.8 - 243.3) * western_wake_treatment_plant_raleigh_frac,
                (316.8 - 243.3) * western_wake_treatment_plant_chatham_frac,
                (316.8 - 243.3) * western_wake_treatment_plant_pittsboro_frac
        };

        /// Bonds West Jordan Lake treatment plant
        vector<Bond *> wjlwtp_bonds_capacity_1;
        int uid = 0;
        for (double &cost : cost_wjlwtp_upgrade_1) {
            wjlwtp_bonds_capacity_1.emplace_back(new LevelDebtServiceBond(wjlwtp_low_base_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        vector<Bond *> wjlwtp_bonds_capacity_2;
        uid = 0;
        for (double &cost : cost_wjlwtp_upgrade_2) {
            wjlwtp_bonds_capacity_2.emplace_back(new LevelDebtServiceBond(wjlwtp_high_base_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        /// West Jordan Lake treatment plant
        SequentialJointTreatmentExpansion low_wjlwtp("Low WJLWTP", wjlwtp_low_base_id, jordan_lake_id, 0, {wjlwtp_low_base_id, wjlwtp_high_base_id}, capacities_wjlwtp_upgrade_1,
                                                     wjlwtp_bonds_capacity_1, construction_time_interval, 12 * WEEKS_IN_YEAR);
        SequentialJointTreatmentExpansion high_wjlwtp("High WJLWTP", wjlwtp_high_base_id, jordan_lake_id, 1, {wjlwtp_low_base_id, wjlwtp_high_base_id}, capacities_wjlwtp_upgrade_2,
                                                      wjlwtp_bonds_capacity_2, construction_time_interval, 12 * WEEKS_IN_YEAR);

        /// Bonds Cary treatment plant expansion
        /// July 2019: Pittsboro/CC added to vectors here
        vector<double> cost_cary_wtp_upgrades = {0, 0, 243. / 2, 0, 0, 0};

        vector<Bond *> bonds_cary_wtp_upgrades_1;
        uid = 0;
        for (double &cost : cost_cary_wtp_upgrades) {
            bonds_cary_wtp_upgrades_1.emplace_back(new LevelDebtServiceBond(cary_wtp_upgrades_low_base_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        vector<Bond *> bonds_cary_wtp_upgrades_2;
        uid = 0;
        for (double &cost : cost_cary_wtp_upgrades) {
            bonds_cary_wtp_upgrades_2.emplace_back(new LevelDebtServiceBond(cary_wtp_upgrades_high_base_id + uid, cost, 25, 0.05, vector<int>(1, 0)));
            uid++;
        }
        /// Cary treatment plant expansion
        vector<double> capacity_cary_wtp_upgrades = {0, 0, 56, 0, 0, 0};
        SequentialJointTreatmentExpansion caryWtpUpgrade1("Cary WTP upgrade 1", cary_wtp_upgrades_low_base_id, jordan_lake_id, 0, {cary_wtp_upgrades_low_base_id, cary_wtp_upgrades_high_base_id},
                                                          capacity_cary_wtp_upgrades, bonds_cary_wtp_upgrades_1,
                                                          construction_time_interval, NONE);
        SequentialJointTreatmentExpansion caryWtpUpgrade2("Cary WTP upgrade 2", cary_wtp_upgrades_high_base_id, jordan_lake_id, 1, {cary_wtp_upgrades_low_base_id, cary_wtp_upgrades_high_base_id},
                                                          capacity_cary_wtp_upgrades, bonds_cary_wtp_upgrades_2,
                                                          construction_time_interval, NONE);

        LevelDebtServiceBond dummy_bond(dummy_node_id, 1., 1, 1., vector<int>(1, 0));
        Reservoir dummy_endpoint("Dummy Node", dummy_node_id, vector<Catchment *>(), 1., 0, evaporation_durham, 1,
                                 construction_time_interval, 0, dummy_bond);

        vector<WaterSource *> water_sources;
        water_sources.push_back(&durham_reservoirs);
        water_sources.push_back(&falls_lake);
        water_sources.push_back(&wheeler_benson_lakes);
        water_sources.push_back(&stone_quarry);
        water_sources.push_back(&ccr);
        water_sources.push_back(&university_lake);
        water_sources.push_back(&jordan_lake);

        water_sources.push_back(&haw_river_intake);

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

        water_sources.push_back(&cape_fear_river_intake);
        water_sources.push_back(&haw_river_intake_expansion_low);
        water_sources.push_back(&haw_river_intake_expansion_high);

        water_sources.push_back(&dummy_endpoint);

        auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

        /// July 2019: get Pittsboro and Chatham set up for WW discharge
        ///         Pittsboro discharges to Jordan Lake
        ///         Chatham County discharges "no wastewater of consequence", served mostly by septic?
        ///             I can assume that small portion of demand (1-5%) is discharged to Jordan Lake????????????

        vector<int> cary_ws_return_id;
        vector<vector<double>> cary_discharge_fraction_series;
        WwtpDischargeRule wwtp_discharge_cary(
                cary_discharge_fraction_series,
                cary_ws_return_id);
        vector<int> owasa_ws_return_id = {jordan_lake_id};
        WwtpDischargeRule wwtp_discharge_owasa(
                demand_to_wastewater_fraction_owasa_raleigh,
                owasa_ws_return_id);
        vector<int> raleigh_ws_return_id = {richland_creek_quarry_id};
        WwtpDischargeRule wwtp_discharge_raleigh(
                demand_to_wastewater_fraction_owasa_raleigh,
                raleigh_ws_return_id);
        vector<int> durham_ws_return_id = {falls_lake_id, jordan_lake_id};
        WwtpDischargeRule wwtp_discharge_durham(
                demand_to_wastewater_fraction_durham,
                durham_ws_return_id);

        vector<int> pittsboro_ws_return_id = {jordan_lake_id};
        WwtpDischargeRule wwtp_discharge_pittsboro(
                demand_to_wastewater_fraction_owasa_raleigh,
                pittsboro_ws_return_id);
        vector<int> chatham_ws_return_id = {jordan_lake_id};
        WwtpDischargeRule wwtp_discharge_chatham(
                demand_to_wastewater_fraction_owasa_raleigh,
                chatham_ws_return_id);

        vector<vector<int>> wjlwtp_remove_from_to_build_list;// = {{21, 20}};

        vector<int> demand_triggered_infra_order_cary = {cary_wtp_upgrades_low_base_id, cary_wtp_upgrades_high_base_id};
        vector<double> demand_infra_cary = {caryupgrades_2 * 7, caryupgrades_3 * 7};

        /// Utility IDs: 0 - OWASA, 1 - Durham, 2 - Cary, 3 - Raleigh, 4 - Chatham County, 5 - Pittsboro
        int uid_owasa = 0;
        int uid_durham = 1;
        int uid_cary = 2;
        int uid_raleigh = 3;
        int uid_chatham = 4;
        int uid_pittsboro = 5;

        Utility cary((char *) "Cary", uid_cary, demand_cary, demand_projection_cary,
                     demand_n_weeks, cary_annual_payment, caryDemandClassesFractions,
                     caryUserClassesWaterPrices, wwtp_discharge_cary, cary_inf_buffer, vector<int>(),
                     demand_triggered_infra_order_cary, demand_infra_cary, discount_rate, bond_term[0], bond_rate[0]);
        Utility durham((char *) "Durham", uid_durham, demand_durham, demand_projection_durham,
                       demand_n_weeks, durham_annual_payment,
                       durhamDemandClassesFractions, durhamUserClassesWaterPrices, wwtp_discharge_durham,
                       durham_inf_buffer, rof_triggered_infra_order_durham,
                       vector<int>(), rofs_infra_durham, discount_rate, wjlwtp_remove_from_to_build_list, bond_term[1], bond_rate[1]);
        Utility owasa((char *) "OWASA", uid_owasa, demand_owasa, demand_projection_owasa,
                      demand_n_weeks, owasa_annual_payment,
                      owasaDemandClassesFractions, owasaUserClassesWaterPrices, wwtp_discharge_owasa, owasa_inf_buffer,
                      rof_triggered_infra_order_owasa,
                      vector<int>(), rofs_infra_owasa, discount_rate, wjlwtp_remove_from_to_build_list, bond_term[2], bond_rate[2]);
        Utility raleigh((char *) "Raleigh", uid_raleigh, demand_raleigh, demand_projection_raleigh,
                        demand_n_weeks, raleigh_annual_payment,
                        raleighDemandClassesFractions, raleighUserClassesWaterPrices, wwtp_discharge_raleigh,
                        raleigh_inf_buffer, rof_triggered_infra_order_raleigh,
                        vector<int>(), rofs_infra_raleigh, discount_rate,
                        wjlwtp_remove_from_to_build_list, bond_term[3], bond_rate[3]);

        // FIXME: SETUP NEW UTILITIES HERE
        /// July 2019: Add Pittsboro and Chatham County utilities with placeholder parameters and input variables
        ///             from OWASA until numbers can be determined
        Utility chatham((char *) "Chatham County", uid_chatham, demand_chatham, demand_projection_chatham,
                        demand_n_weeks, chatham_annual_payment,
                        chathamDemandClassesFractions, chathamUserClassesWaterPrices, wwtp_discharge_chatham,
                        chatham_inf_buffer, rof_triggered_infra_order_chatham,
                        vector<int>(), rofs_infra_chatham, discount_rate, wjlwtp_remove_from_to_build_list, bond_term[4], bond_rate[4]);
        Utility pittsboro((char *) "Pittsboro", uid_pittsboro, demand_pittsboro, demand_projection_pittsboro,
                      demand_n_weeks, pittsboro_annual_payment,
                      pittsboroDemandClassesFractions, pittsboroUserClassesWaterPrices, wwtp_discharge_pittsboro,
                      pittsboro_inf_buffer, rof_triggered_infra_order_pittsboro,
                      vector<int>(), rofs_infra_pittsboro, discount_rate, wjlwtp_remove_from_to_build_list, bond_term[5], bond_rate[5]);

        vector<Utility *> utilities;
        utilities.push_back(&cary);
        utilities.push_back(&durham);
        utilities.push_back(&owasa);
        utilities.push_back(&raleigh);
        utilities.push_back(&chatham);
        utilities.push_back(&pittsboro);

        /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
        /// sources IDs.
        vector<vector<int>> reservoir_utility_connectivity_matrix = {
                {cane_creek_reservoir_id, stone_quarry_id,
                 university_lake_id, jordan_lake_id,
                 stone_quarry_expansion_low_id, stone_quarry_expansion_high_id,
                 university_lake_expansion_id, cane_creek_reservoir_expansion_id,
                 wjlwtp_low_base_id, wjlwtp_high_base_id}, //OWASA
                {durham_reservoirs_id, jordan_lake_id,  teer_quarry_id,
                 michie_expansion_low_id, michie_expansion_high_id,
                 reclaimed_water_low_id, reclaimed_water_high_id,
                 wjlwtp_low_base_id, wjlwtp_high_base_id}, //Durham
                {jordan_lake_id, cary_wtp_upgrades_low_base_id, cary_wtp_upgrades_high_base_id}, //Cary
                {falls_lake_id, wheeler_benson_id,
                 jordan_lake_id, little_river_raleigh_reservoir_id,
                 richland_creek_quarry_id, falls_lake_reallocation_id, neuse_river_intake_id,
                 wjlwtp_low_base_id, wjlwtp_high_base_id},  //Raleigh
                {jordan_lake_id, harnett_county_intake_id,
                 wjlwtp_low_base_id, wjlwtp_high_base_id}, //Chatham County
                {haw_river_intake_id, jordan_lake_id,
                 haw_river_intake_expansion_low_id, haw_river_intake_expansion_high_id,
                 wjlwtp_low_base_id, wjlwtp_high_base_id} //Pittsboro
        };

        auto table_storage_shift = vector<vector<double>>(4, vector<double>(25, 0.));
        table_storage_shift[uid_raleigh][falls_lake_reallocation_id] = 2000.;
        table_storage_shift[uid_raleigh][richland_creek_quarry_id] = 5000.;
        table_storage_shift[uid_owasa][university_lake_expansion_id] = 100.;
        table_storage_shift[uid_durham][wjlwtp_low_base_id] = 500.;
        table_storage_shift[uid_durham][wjlwtp_high_base_id] = 500.;
        table_storage_shift[uid_durham][michie_expansion_low_id] = 700.;
        table_storage_shift[uid_durham][teer_quarry_id] = 700.;

        vector<DroughtMitigationPolicy *> drought_mitigation_policies;
        /// Restriction policies
        vector<double> initial_restriction_triggers = {OWASA_restriction_trigger,
                                                       Durham_restriction_trigger,
                                                       cary_restriction_trigger,
                                                       raleigh_restriction_trigger,
                                                       chatham_restriction_trigger,
                                                       pittsboro_restriction_trigger};

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
        vector<double> restriction_stage_multipliers_raleigh = {0.9, 0.8, 0.7, 0.6};
        vector<double> restriction_stage_triggers_raleigh = {initial_restriction_triggers[3],
                                                             initial_restriction_triggers[3] + 0.15f,
                                                             initial_restriction_triggers[3] + 0.35f,
                                                             initial_restriction_triggers[3] + 0.6f};

        // July 2019: Same as OWASA for now
        vector<double> restriction_stage_multipliers_pittsboro = {0.9, 0.8, 0.7, 0.6};
        vector<double> restriction_stage_triggers_pittsboro = {initial_restriction_triggers[2],
                                                               initial_restriction_triggers[2] + 0.15f,
                                                               initial_restriction_triggers[2] + 0.35f,
                                                               initial_restriction_triggers[2] + 0.6f};
        vector<double> restriction_stage_multipliers_chatham = {0.9, 0.8, 0.7, 0.6};
        vector<double> restriction_stage_triggers_chatham = {initial_restriction_triggers[2],
                                                               initial_restriction_triggers[2] + 0.15f,
                                                               initial_restriction_triggers[2] + 0.35f,
                                                               initial_restriction_triggers[2] + 0.6f};

        Restrictions restrictions_c(uid_cary,
                                    restriction_stage_multipliers_cary,
                                    restriction_stage_triggers_cary);
        Restrictions restrictions_d(uid_durham,
                                    restriction_stage_multipliers_durham,
                                    restriction_stage_triggers_durham);
        Restrictions restrictions_o(uid_owasa,
                                    restriction_stage_multipliers_owasa,
                                    restriction_stage_triggers_owasa,
                                    &owasaDemandClassesFractions,
                                    &owasaUserClassesWaterPrices,
                                    &owasaPriceSurcharges);
        Restrictions restrictions_r(uid_raleigh,
                                    restriction_stage_multipliers_raleigh,
                                    restriction_stage_triggers_raleigh);

        Restrictions restrictions_p(uid_pittsboro,
                                    restriction_stage_multipliers_pittsboro,
                                    restriction_stage_triggers_pittsboro);
        Restrictions restrictions_cc(uid_chatham,
                                    restriction_stage_multipliers_chatham,
                                    restriction_stage_triggers_chatham);

        // FIXME: MAKE SURE SEQUENTIAL NUMBERING OF ALL DROUGHT MITIGATION POLICIES, ERROR MESSAGING HERE IS LIMITED
        drought_mitigation_policies = {&restrictions_c, &restrictions_d,
                                       &restrictions_o, &restrictions_r,
                                       &restrictions_cc, &restrictions_p};

        // FIXME: IMPLEMENT CC AND PITTSBORO TRANSFER CAPABILITY
        /// Sept 2019: Transfer Policy 1: Original Triangle setup with added Durham -> Chatham connection with Cary as originator
        ///              - Chatham has 3 MGD interconnect with Durham, contracted until 2028
        /*
        *         CARY           CHATHAM
        *       /      \        /
        *    0 v        v 1    ^ 4
        *     /          \    /
        *  RALEIGH---><--DURHAM--><--OWASA
        *             2           3
        */

        vector<int> buyers_ids_scheme_1 = {uid_owasa, uid_durham, uid_raleigh, uid_chatham};
        //FIXME: CHECK IF TRANSFER CAPACITIES MATCH IDS IN BUYERS_IDS.
        //FIXME: CANT BE ANY GAP IN TRANFER IDS (ALL UTILITIES MUST BE ACCOUNTED FOR IN A PARTICULAR SCHEME...)
        vector<double> buyers_transfers_capacities_scheme_1 = {10.8 * 7, 10.0 * 7, 11.5 * 7,
                                                               7.0 * 7, 3.0 * 7};
        vector<double> buyers_transfers_trigger_scheme_1 = {owasa_transfer_trigger,
                                                            durham_transfer_trigger,
                                                            raleigh_transfer_trigger,
                                                            chatham_transfer_trigger};

        Graph ug(5);
        ug.addEdge(uid_cary, uid_durham);
        ug.addEdge(uid_cary, uid_raleigh);
        ug.addEdge(uid_durham, uid_raleigh);
        ug.addEdge(uid_durham, uid_owasa);
        ug.addEdge(uid_durham, uid_chatham);

        Transfers t_one(6, uid_cary, jordan_lake_id, 35,
                        buyers_ids_scheme_1,
                        buyers_transfers_capacities_scheme_1,
                        buyers_transfers_trigger_scheme_1,
                        ug,
                        vector<double>(),
                        vector<int>());
        drought_mitigation_policies.push_back(&t_one);

        /// Sept 2019: Transfer Policy 2: Emergency line between Chatham and Pittsboro, Chatham originator
        ///              - Pittsboro has emergency interconnect of 0.5 MGD with CC, can only receive water through it (check this)
        //FIXME: GRAPH REQUIRES SEQUENTIAL UTILITY IDS STARTING FROM 0, SO ALL UTILITIES MUST BE INCLUDED IN SCHEME
        // AS A WORKAROUND, WILL SET ANY CAPACITY TO UTILITIES THAT DON'T ACTUALLY HAVE CONNECTIONS TO 0
        /*
        *    CHATHAM---->----PITTSBORO
        *               5
        */
        vector<int> buyers_ids_scheme_2 = {0, 1, 2, 3, uid_pittsboro};
        //FIXME: CHECK IF TRANSFER CAPACITIES MATCH IDS IN BUYERS_IDS.
        vector<double> buyers_transfers_capacities_scheme_2 = {0,0,0,0, 0.5 * 7}; // only one pipe, implies other connections are 0 capacity
        vector<double> buyers_transfers_trigger_scheme_2 = {1.1,1.1,1.1,1.1, pittsboro_transfer_trigger};

        Graph ug_two(6);
        ug_two.addEdge(0, 1);
        ug_two.addEdge(1, 2);
        ug_two.addEdge(2, 3);
        ug_two.addEdge(3, 4);
        ug_two.addEdge(uid_chatham, uid_pittsboro); // 4 to 5 or vice versa

        Transfers t_two(7, uid_chatham, jordan_lake_id, 2.1, // SET SOURCE BUFFER SO THAT 10% OF TRMT CAP ALWAYS LEFT OPEN
                        buyers_ids_scheme_2,
                        buyers_transfers_capacities_scheme_2,
                        buyers_transfers_trigger_scheme_2,
                        ug_two,
                        vector<double>(),
                        vector<int>());
        drought_mitigation_policies.push_back(&t_two);


        /// Handle insurance
        vector<double> insurance_triggers = {owasa_insurance_use,
                                             durham_insurance_use,
                                             cary_insurance_use,
                                             raleigh_insurance_use,
                                             chatham_insurance_use,
                                             pittsboro_insurance_use}; //FIXME: Change per solution
        vector<double> fixed_payouts = {owasa_insurance_payment,
                                   durham_insurance_payment,
                                   cary_insurance_payment,
                                   raleigh_insurance_payment,
                                   chatham_insurance_payment,
                                   pittsboro_insurance_payment};
        vector<int> insured_utilities = {0, 1, 2, 3, 4, 5};
        double insurance_premium = 1.2;
        InsuranceStorageToROF in(8, water_sources, g, reservoir_utility_connectivity_matrix, utilities, drought_mitigation_policies,
                                 min_env_flow_controls, utilities_rdm, water_sources_rdm, policies_rdm, insurance_triggers,
                                 insurance_premium, fixed_payouts, n_weeks);

        drought_mitigation_policies.push_back(&in);

        /// Creates simulation object depending on use (or lack thereof) ROF tables
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
            //realization_start = omp_get_wtime();
    	    this->master_data_collector = s->runFullSimulation(n_threads, vars);
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
            //realization_start = omp_get_wtime();
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
            //realization_start = omp_get_wtime();
            this->master_data_collector = s->runFullSimulation(n_threads, vars);
        }
        double end_time = omp_get_wtime();
//	printf("Function evaluation time: %f s\n", end_time - start_time);

        //double realization_end = omp_get_wtime();
        //std::cout << "Simulation took  " << realization_end - realization_start
        //      << "s" << std::endl;

    /// Calculate objectives and store them in Borg decision variables array.
#ifdef  PARALLEL
        objectives = calculateAndPrintObjectives(false);

        int i = 0;
        objs[i] = min(min(objectives[i], objectives[5 + i]),
        		   min(objectives[10 + i], objectives[15 + i])) - 1.;
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
        int num_dec_var = 56;
//        printf("Exception called during calculations. Decision variables are below:\n");
        ofstream sol;
	int world_rank;
    
#ifdef  PARALLEL
	// int mpi_initialized;
	// MPI_Initialized(&mpi_initialized);
	// if (mpi_initialized)
 //            MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	// else
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


void Triangle::readInputData() {
    cout << "Reading input data." << endl;
    string data_dir = DEFAULT_DATA_DIR + BAR;

#pragma omp parallel num_threads(omp_get_thread_num())
    {
#pragma omp single
        streamflows_durham = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "durham_inflows.csv", n_realizations);
#pragma omp single
        streamflows_flat = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "falls_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_swift = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "lake_wb_inflows.csv", n_realizations);
#pragma omp single
        streamflows_llr = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "little_river_raleigh_inflows.csv", n_realizations);
        // }
#pragma omp single
        streamflows_crabtree = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "crabtree_inflows.csv", n_realizations);
#pragma omp single
        streamflows_phils = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "stone_quarry_inflows.csv", n_realizations);
#pragma omp single
        streamflows_cane = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "cane_creek_inflows.csv", n_realizations);
#pragma omp single
        streamflows_morgan = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "university_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_haw = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "jordan_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_clayton = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "clayton_inflows.csv", n_realizations);
#pragma omp single
        streamflows_lillington = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "inflows" + evap_inflows_suffix +
                BAR + "lillington_inflows.csv", n_realizations);
// };
        //cout << "Reading evaporations." << endl;
#pragma omp single
        evap_durham = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "evaporation" + evap_inflows_suffix +
                BAR + "durham_evap.csv", n_realizations);
#pragma omp single
        evap_falls_lake = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "evaporation" + evap_inflows_suffix +
                BAR + "falls_lake_evap.csv", n_realizations);
#pragma omp single
        evap_owasa = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "evaporation" + evap_inflows_suffix +
                BAR + "owasa_evap.csv", n_realizations);
#pragma omp single
        evap_little_river = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "evaporation" + evap_inflows_suffix +
                BAR + "little_river_raleigh_evap.csv", n_realizations);
#pragma omp single
        {
            evap_wheeler_benson = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "evaporation" + evap_inflows_suffix +
                    BAR + "wb_evap.csv", n_realizations);
            evap_jordan_lake = evap_owasa;
        }

        //cout << "Reading demands." << endl;
#pragma omp single
        demand_cary = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "cary_demands_scenario_as_projected.csv", n_realizations);
#pragma omp single
        demand_durham = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "durham_demands_scenario_as_projected.csv", n_realizations);
#pragma omp single
        demand_raleigh = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "raleigh_demands_scenario_as_projected.csv", n_realizations);
#pragma omp single
        demand_owasa = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "owasa_demands_scenario_as_projected.csv", n_realizations);
#pragma omp single
        demand_pittsboro = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "pittsboro_demands_scenario_as_projected.csv", n_realizations);
#pragma omp single
        demand_chatham = Utils::parse2DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "chatham_demands_scenario_as_projected.csv", n_realizations);

        //cout << "Reading demand projections." << endl;
#pragma omp single
        demand_projection_cary = Utils::parse1DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "cary_annual_demand_projections.csv", 46);
#pragma omp single
        demand_projection_durham = Utils::parse1DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "durham_annual_demand_projections.csv", 46);
#pragma omp single
        demand_projection_raleigh = Utils::parse1DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "raleigh_annual_demand_projections.csv", 46);
#pragma omp single
        demand_projection_owasa = Utils::parse1DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "owasa_annual_demand_projections.csv", 46);
#pragma omp single
        demand_projection_pittsboro = Utils::parse1DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "pittsboro_annual_demand_projections.csv", 46);
#pragma omp single
        demand_projection_chatham = Utils::parse1DCsvFile(
                io_directory + DEFAULT_DATA_DIR + "demands" + evap_inflows_suffix +
                BAR + "chatham_annual_demand_projections.csv", 46);

        //cout << "Reading others." << endl;
#pragma omp single
        {
            demand_to_wastewater_fraction_owasa_raleigh = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "demand_to_wastewater_fraction_owasa_raleigh.csv");
            demand_to_wastewater_fraction_durham = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "demand_to_wastewater_fraction_durham.csv");

            caryDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "caryDemandClassesFractions.csv");
            durhamDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "durhamDemandClassesFractions.csv");
            raleighDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "raleighDemandClassesFractions.csv");
            owasaDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "owasaDemandClassesFractions.csv");
            pittsboroDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "pittsboroDemandClassesFractions.csv");
            chathamDemandClassesFractions = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "chathamDemandClassesFractions.csv");

            caryUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "caryUserClassesWaterPrices.csv");
            durhamUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "durhamUserClassesWaterPrices.csv");
            raleighUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "raleighUserClassesWaterPrices.csv");
            owasaUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "owasaUserClassesWaterPrices.csv");
            pittsboroUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "pittsboroUserClassesWaterPrices.csv");
            chathamUserClassesWaterPrices = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "chathamUserClassesWaterPrices.csv");

            owasaPriceSurcharges = Utils::parse2DCsvFile(
                    io_directory + DEFAULT_DATA_DIR + "owasaPriceRestMultipliers.csv");
        }
//    cout << "Done reading input data." << endl;
    }

}
