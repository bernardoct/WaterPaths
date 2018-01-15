//
// Created by Bernardo on 11/23/2017.
//

#include <algorithm>
#include "Triangle.h"
#include "../Utils/Utils.h"
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
#include "../DataCollector/MasterDataCollector.h"
#include "../Simulation/Simulation.h"

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
void Triangle::functionEvaluation(const double *vars, double *objs, double *consts) {
    //srand(0);//(unsigned int) time(nullptr));

    // ===================== SET UP DECISION VARIABLES  =====================

    double Durham_restriction_trigger = vars[0];
    double OWASA_restriction_trigger = vars[1];
    double raleigh_restriction_trigger = vars[2];
    double cary_restriction_trigger = vars[3];
    double durham_transfer_trigger = vars[4];
    double owasa_transfer_trigger = vars[5];
    double raleigh_transfer_trigger = vars[6];
    double OWASA_JLA = vars[7];
    double Raleigh_JLA = vars[8];
    double Durham_JLA = vars[9];
    double Cary_JLA = vars[10];
    double durham_annual_payment = vars[11]; // contingency fund
    double owasa_annual_payment = vars[12];
    double raleigh_annual_payment = vars[13];
    double cary_annual_payment = vars[14];
    double durham_insurance_use = vars[15]; // insurance st_rof
    double owasa_insurance_use = vars[16];
    double raleigh_insurance_use = vars[17];
    double cary_insurance_use = vars[18];
    double durham_insurance_payment = vars[19];
    double owasa_insurance_payment = vars[20];
    double raleigh_insurance_payment = vars[21];
    double cary_insurance_payment = vars[22];
    double durham_inftrigger = vars[23];
    double owasa_inftrigger = vars[24];
    double raleigh_inftrigger = vars[25];
    double cary_inftrigger = vars[26];
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

    vector<infraRank> durham_infra_order_raw = {
            infraRank(9, Teer_quarry_expansion_ranking),
            infraRank(15, lake_michie_expansion_ranking_low),
            infraRank(16, lake_michie_expansion_ranking_high),
            infraRank(18, reclaimed_water_ranking_low),
            infraRank(19, reclaimed_water_high),
            infraRank(20, western_wake_treatment_plant_rank_durham_low),
            infraRank(21, western_wake_treatment_plant_rank_durham_high)
    };

    vector<infraRank> owasa_infra_order_raw = {
            infraRank(12, Stone_quarry_reservoir_expansion_shallow_ranking),
            infraRank(13, Stone_quarry_reservoir_expansion_deep_ranking),
            infraRank(14, university_lake_expansion_ranking),
            infraRank(24, Cane_creek_expansion_ranking),
            infraRank(20, western_wake_treatment_plant_rank_OWASA_low),
            infraRank(21, western_wake_treatment_plant_rank_OWASA_high)
    };

    vector<infraRank> raleigh_infra_order_raw = {
            infraRank(7, little_river_reservoir_ranking),
            infraRank(8, richland_creek_quarry_rank),
            infraRank(10, neuse_river_intake_rank),
            infraRank(17, reallocate_falls_lake_rank),
            infraRank(20, western_wake_treatment_plant_rank_raleigh_low),
            infraRank(21, western_wake_treatment_plant_rank_raleigh_high)
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

    vector<int> rof_triggered_infra_order_durham =
            vecInfraRankToVecInt(durham_infra_order_raw);
    vector<double> rofs_infra_durham = vector<double>
            (rof_triggered_infra_order_durham.size(), durham_inftrigger);
    vector<int> rof_triggered_infra_order_owasa =
            vecInfraRankToVecInt(owasa_infra_order_raw);
    vector<double> rofs_infra_owasa = vector<double>
            (rof_triggered_infra_order_owasa.size(), owasa_inftrigger);
    vector<int> rof_triggered_infra_order_raleigh =
            vecInfraRankToVecInt(raleigh_infra_order_raw);
    vector<double> rofs_infra_raleigh = vector<double>
            (rof_triggered_infra_order_raleigh.size(), raleigh_inftrigger);

    /// Remove small expansions being built after big expansions that would
    /// encompass the smal expansions.
    added_storage_michie_expansion_high =
            checkAndFixInfraExpansionHighLowOrder(
                    &rof_triggered_infra_order_durham,
                    15,
                    16,
                    added_storage_michie_expansion_low,
                    added_storage_michie_expansion_high);

    reclaimed_capacity_high =
            checkAndFixInfraExpansionHighLowOrder(
                    &rof_triggered_infra_order_durham,
                    18,
                    19,
                    reclaimed_capacity_low,
                    reclaimed_capacity_high);


    /// Normalize Jordan Lake Allocations in case they exceed 1.
    double sum_jla_allocations = OWASA_JLA + Durham_JLA + Cary_JLA +
                                 Raleigh_JLA;
    if (sum_jla_allocations > 1.) {
        OWASA_JLA /= sum_jla_allocations;
        Durham_JLA /= sum_jla_allocations;
        Cary_JLA /= sum_jla_allocations;
        Raleigh_JLA /= sum_jla_allocations;
    }

    /// Normalize West Jordan Lake WTP allocations.
    double sum_wjlwtp = western_wake_treatment_frac_durham +
                        western_wake_treatment_plant_owasa_frac +
                        western_wake_treatment_plant_raleigh_frac;
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
    unsigned long max_lines = n_realizations;
    vector<unsigned long> realizations_to_read;
    if (!realizations.empty()) {
        realizations_to_read = this->realizations;
        n_realizations = (int) realizations_to_read.size();
    } else
        realizations_to_read = vector<unsigned long>();

    vector<vector<double>> streamflows_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/durham_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_flat = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/falls_lake_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_swift = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/lake_wb_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_llr = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/little_river_raleigh_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_crabtree = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/crabtree_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_phils = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/stone_quarry_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_cane = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/cane_creek_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_morgan = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/university_lake_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_haw = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/jordan_lake_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_clayton = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/clayton_inflows.csv", max_lines, realizations_to_read);
    vector<vector<double>> streamflows_lillington = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/lillington_inflows.csv", max_lines, realizations_to_read);

    //cout << "Reading demands." << endl;
    vector<vector<double>> demand_cary = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/cary_demand.csv", max_lines, realizations_to_read);
    vector<vector<double>> demand_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/durham_demand.csv", max_lines, realizations_to_read);
    vector<vector<double>> demand_raleigh = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/raleigh_demand.csv", max_lines, realizations_to_read);
    vector<vector<double>> demand_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/owasa_demand.csv", max_lines, realizations_to_read);

    //cout << "Reading evaporations." << endl;
    vector<vector<double>> evap_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/durham_evap.csv", max_lines, realizations_to_read);
    vector<vector<double>> evap_falls_lake = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/falls_lake_evap.csv", max_lines, realizations_to_read);
    vector<vector<double>> evap_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/owasa_evap.csv", max_lines, realizations_to_read);
    vector<vector<double>> evap_little_river = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/little_river_raleigh_evap.csv", max_lines, realizations_to_read);
    vector<vector<double>> evap_wheeler_benson = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/wb_evap.csv", max_lines, realizations_to_read);
    vector<vector<double>> evap_jordan_lake = evap_owasa;

    //cout << "Reading others." << endl;
    vector<vector<double>> demand_to_wastewater_fraction_owasa_raleigh = Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");
    vector<vector<double>> demand_to_wastewater_fraction_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_durham.csv");
//
    vector<vector<double>> caryDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/caryDemandClassesFractions.csv");
    vector<vector<double>> durhamDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamDemandClassesFractions.csv");
    vector<vector<double>> raleighDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/raleighDemandClassesFractions.csv");
    vector<vector<double>> owasaDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/owasaDemandClassesFractions.csv");

    vector<vector<double>> caryUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/caryUserClassesWaterPrices.csv");
    vector<vector<double>> durhamUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamUserClassesWaterPrices.csv");
    vector<vector<double>> raleighUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/raleighUserClassesWaterPrices.csv");
    vector<vector<double>> owasaUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/owasaUserClassesWaterPrices.csv");

    vector<vector<double>> owasaPriceSurcharges = Utils::parse2DCsvFile(output_directory + "/TestFiles/owasaPriceRestMultipliers.csv");

//    vector<double> sewageFractions = Utils::parse1DCsvFile(
//            output_directory + "/TestFiles/sewageFractions.csv");

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
    vector<double> falls_lake_storage = {0, 23266, 34700};
    vector<double> falls_lake_area = {0.32 * 5734, 0.32 * 29000, 0.28 * 40434};
    vector<double> wheeler_benson_storage = {0, 2789.66};
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
    int falls_controls_weeks[2] = {13, 43};
    double falls_base_releases[2] = {64.64 * 7, 38.78 * 7};
    double falls_min_gage[2] = {180 * 7, 130 * 7};

    SeasonalMinEnvFlowControl durham_min_env_control(0, &dlr_weeks,
                                                     &dlr_releases);
//    FixedMinEnvFlowControl falls_min_env_control(1, 38.78 * 7);
    FallsLakeMinEnvFlowControl falls_min_env_control(1,
                                                     10,
                                                     falls_controls_weeks,
                                                     falls_base_releases,
                                                     falls_min_gage,
                                                     crabtree_creek);

    StorageMinEnvFlowControl wheeler_benson_min_env_control(2,
                                                            vector<int>(1, 2),
                                                            &wb_storage,
                                                            &wb_releases);
    FixedMinEnvFlowControl sq_min_env_control(3, 0);
    InflowMinEnvFlowControl ccr_min_env_control(4, vector<int>(1, 4),
                                                &ccr_inflows,
                                                &ccr_releases);
    FixedMinEnvFlowControl university_min_env_control(5, 0);
//    FixedMinEnvFlowControl jordan_min_env_control(6,
//                                                  25.8527 * 7);
    JordanLakeMinEnvFlowControl jordan_min_env_control(
            6, &cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
            290.84, 387.79, 30825.0, 14924.0);
    SeasonalMinEnvFlowControl little_river_min_env_control(7, &dlr_weeks,
                                                           &dlr_releases);
    FixedMinEnvFlowControl richland_min_env_control(8, 0);
    FixedMinEnvFlowControl teer_min_env_control(9, 0);
    FixedMinEnvFlowControl neuse_intake_min_env_control(10, 38.78 * 7);

//    vector<int> eno_weeks = {7, 16, 53};
//    vector<double> eno_releases = {6.49 * 7, 19.48 * 7, 6.49 * 7};
//    SeasonalMinEnvFlowControl eno_min_env_control(&eno_weeks, &eno_releases);

    vector<MinEnvironFlowControl *> min_env_flow_controls;
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
    vector<double> construction_time_interval = {3.0, 5.0};
    vector<double> city_infrastructure_rof_triggers = {owasa_inftrigger,
                                                       durham_inftrigger,
                                                       cary_inftrigger,
                                                       raleigh_inftrigger};
    vector<double> bond_term = {25, 25, 25, 25};
    vector<double> bond_rate = {0.05, 0.05, 0.05, 0.05};
    double discount_rate = 0.05;

    /// Jordan Lake parameters
    double jl_supply_capacity = 14924.0;
    double jl_wq_capacity = 30825.0;
    double jl_storage_capacity = jl_wq_capacity + jl_supply_capacity;
    vector<int> jl_allocations_ids = {0, 1, 2, 3, WATER_QUALITY_ALLOCATION};
    vector<double> jl_allocation_fractions = {
            OWASA_JLA * jl_supply_capacity / jl_storage_capacity,
            Durham_JLA * jl_supply_capacity / jl_storage_capacity,
            Cary_JLA * jl_supply_capacity / jl_storage_capacity,
            Raleigh_JLA * jl_supply_capacity / jl_storage_capacity,
            jl_wq_capacity / jl_storage_capacity};
    vector<double> jl_treatment_allocation_fractions = {0.0, 0.0, 1.0, 0.0};

    /// Jordan Lake parameters
    double fl_supply_capacity = 14700.0;
    double fl_wq_capacity = 20000.0;
    double fl_storage_capacity = fl_wq_capacity + fl_supply_capacity;
    vector<int> fl_allocations_ids = {3, WATER_QUALITY_ALLOCATION};
    vector<double> fl_allocation_fractions = {
            fl_supply_capacity / fl_storage_capacity,
            fl_wq_capacity / fl_storage_capacity};
    vector<double> fl_treatment_allocation_fractions = {0.0, 0.0, 0.0, 1.0};

    // Existing Sources
    Reservoir durham_reservoirs("Lake Michie & Little River Res. (Durham)",
                                0,
                                catchment_durham,
                                6349.0,
                                ILLIMITED_TREATMENT_CAPACITY,
                                &evaporation_durham, 1069);
//    Reservoir falls_lake("Falls Lake", 1, catchment_flat,
//                         34700.0, 99999,
//                         &evaporation_falls_lake, &falls_lake_storage_area);
    AllocatedReservoir falls_lake("Falls Lake",
                                  1,
                                  catchment_flat,
                                  fl_storage_capacity,
                                  ILLIMITED_TREATMENT_CAPACITY,
                                  &evaporation_falls_lake,
                                  &falls_lake_storage_area,
                                  &fl_allocations_ids,
                                  &fl_allocation_fractions,
                                  &fl_treatment_allocation_fractions);

    Reservoir wheeler_benson_lakes("Wheeler-Benson Lakes", 2, catchment_swift,
                                   2789.66,
                                   ILLIMITED_TREATMENT_CAPACITY,
                                   &evaporation_wheeler_benson,
                                   &wheeler_benson_storage_area);
    Reservoir stone_quarry("Stone Quarry",
                           3,
                           catchment_phils,
                           200.0,
                           ILLIMITED_TREATMENT_CAPACITY,
                           &evaporation_owasa,
                           10);
    Reservoir ccr("Cane Creek Reservoir",
                  4,
                  catchment_cane,
                  2909.0,
                  ILLIMITED_TREATMENT_CAPACITY,
                  &evaporation_owasa,
                  500);
    Reservoir university_lake("University Lake", 5, catchment_morgan, 449.0,
                              ILLIMITED_TREATMENT_CAPACITY,
                              &evaporation_owasa,
                              212);
    AllocatedReservoir jordan_lake("Jordan Lake",
                                   6,
                                   catchment_haw,
                                   jl_storage_capacity,
                                   448,
                                   &evaporation_jordan_lake,
                                   13940,
                                   &jl_allocations_ids,
                                   &jl_allocation_fractions,
                                   &jl_treatment_allocation_fractions);

    // other than Cary WTP for Jordan Lake, assume no WTP constraints - each
    // city can meet its daily demands with available treatment infrastructure

    double WEEKS_IN_YEAR = 0;

    // Potential Sources
    // The capacities listed here for expansions are what additional capacity is gained relative to existing capacity,
    // NOT the total capacity after the expansion is complete. For infrastructure with a high and low option, this means
    // the capacity for both is relative to current conditions - if Lake Michie is expanded small it will gain 2.5BG,
    // and a high expansion will provide 7.7BG more water than current. if small expansion happens, followed by a large
    // expansion, the maximum capacity through expansion is 7.7BG, NOT 2.5BG + 7.7BG.
    Reservoir little_river_reservoir("Little River Reservoir (Raleigh)", 7,
                                     catchment_little_river_raleigh, 3700.0,
                                     ILLIMITED_TREATMENT_CAPACITY,
                                     &evaporation_little_river,
                                     &little_river_storage_area,
                                     city_infrastructure_rof_triggers[3],
                                     construction_time_interval,
                                     17 *
                                     WEEKS_IN_YEAR,
                                     263.0);
    Quarry richland_creek_quarry("Richland Creek Quarry", 8, gage_clayton,
                                 4000.0,
                                 ILLIMITED_TREATMENT_CAPACITY,
                                 &evaporation_falls_lake,
                                 100.,
                                 city_infrastructure_rof_triggers[3],
                                 construction_time_interval,
                                 17 *
                                 WEEKS_IN_YEAR,
                                 400.0,
                                 50. * 7);
    // diversions to Richland Creek Quarry based on ability to meet downstream flow target at Clayton, NC
    Quarry teer_quarry("Teer Quarry",
                       9,
                       vector<Catchment *>(),
                       1315.0,
                       ILLIMITED_TREATMENT_CAPACITY,
                       &evaporation_falls_lake,
                       &teer_storage_area,
                       city_infrastructure_rof_triggers[1],
                       construction_time_interval,
                       7 *
                       WEEKS_IN_YEAR,
                       22.6,
                       15 * 7); //FIXME: MAX PUMPING CAPACITY?
    //Reservoir rNeuseRiverIntake("rNeuseRiverIntake", 10, 0, catchment_flat, 16.0, 99999, city_infrastructure_rof_triggers[3], construction_time_interval, 5000, 20, 0.05);
    Intake neuse_river_intake("Neuse River Intake", 10, catchment_flat, 16 * 7,
                              city_infrastructure_rof_triggers[3],
                              construction_time_interval,
                              17 * WEEKS_IN_YEAR,
                              225.5);
    // diversions to Teer Quarry for Durham based on inflows to downstream Falls Lake from the Flat River
    // FYI: spillage from Eno River also helps determine Teer quarry diversion, but Eno spillage isn't factored into
    // downstream water balance?

    vector<double> fl_relocation_fractions = {
            (fl_supply_capacity + falls_lake_reallocation) /
            fl_storage_capacity,
            (fl_wq_capacity - falls_lake_reallocation) / fl_storage_capacity};
    Relocation fl_reallocation("Falls Lake Reallocation",
                               17,
                               1,
                               &fl_relocation_fractions,
                               &fl_allocations_ids,
                               city_infrastructure_rof_triggers[3],
                               construction_time_interval,
                               12 * WEEKS_IN_YEAR,
                               68.2);
    ReservoirExpansion ccr_expansion("Cane Creek Reservoir Expansion",
                                     24,
                                     4,
                                     3000.0,
                                     city_infrastructure_rof_triggers[2],
                                     construction_time_interval,
                                     17 * WEEKS_IN_YEAR,
                                     127.0);
    ReservoirExpansion low_sq_expansion("Low Stone Quarry Expansion", 12, 3,
                                        1500.0,
                                        city_infrastructure_rof_triggers[2],
                                        construction_time_interval,
                                        23 * WEEKS_IN_YEAR,
                                        1.4);
    ReservoirExpansion high_sq_expansion("High Stone Quarry Expansion", 13, 3,
                                         2200.0,
                                         city_infrastructure_rof_triggers[2],
                                         construction_time_interval,
                                         23 * WEEKS_IN_YEAR,
                                         64.6);
    ReservoirExpansion univ_lake_expansion("University Lake Expansion",
                                           14,
                                           5,
                                           2550.0,
                                           city_infrastructure_rof_triggers[2],
                                           construction_time_interval,
                                           17 * WEEKS_IN_YEAR,
                                           107.0);
    ReservoirExpansion low_michie_expansion("Low Lake Michie Expansion",
                                            15,
                                            0,
                                            added_storage_michie_expansion_low,
                                            city_infrastructure_rof_triggers[1],
                                            construction_time_interval,
                                            17 * WEEKS_IN_YEAR,
                                            158.3);
    ReservoirExpansion high_michie_expansion("High Lake Michie Expansion",
                                             16,
                                             0,
                                             added_storage_michie_expansion_high,
                                             city_infrastructure_rof_triggers[1],
                                             construction_time_interval,
                                             17 * WEEKS_IN_YEAR,
                                             203.3);
    WaterReuse low_reclaimed("Low Reclaimed Water System",
                             18,
                             reclaimed_capacity_low,
                             city_infrastructure_rof_triggers[1],
                             construction_time_interval,
                             7 * WEEKS_IN_YEAR,
                             27.5);
    WaterReuse high_reclaimed("High Reclaimed Water System",
                              19,
                              reclaimed_capacity_high,
                              city_infrastructure_rof_triggers[1],
                              construction_time_interval,
                              7 * WEEKS_IN_YEAR,
                              104.4);

    WEEKS_IN_YEAR = Constants::WEEKS_IN_YEAR;

    vector<double> wjlwtp_treatment_capacity_fractions =
            {western_wake_treatment_plant_owasa_frac,
             western_wake_treatment_frac_durham,
             0.,
             western_wake_treatment_plant_raleigh_frac,
             0.};
    vector<double> cary_upgrades_treatment_capacity_fractions = {0., 0., 1.,
                                                                 0., 0.};

    auto *shared_added_wjlwtp_treatment_pool = new vector<double>();
    auto *shared_added_wjlwtp_price = new vector<double>();
    SequentialJointTreatmentExpansion low_wjlwtp("Low WJLWTP",
                                                 20,
                                                 6,
                                                 0,
                                                 33 * 7,
                                                 &wjlwtp_treatment_capacity_fractions,
                                                 shared_added_wjlwtp_treatment_pool,
                                                 shared_added_wjlwtp_price,
                                                 city_infrastructure_rof_triggers[1],
                                                 construction_time_interval,
                                                 12 * WEEKS_IN_YEAR,
                                                 243.3);
    SequentialJointTreatmentExpansion high_wjlwtp("High WJLWTP",
                                                  21,
                                                  6,
                                                  0,
                                                  54 * 7,
                                                  &wjlwtp_treatment_capacity_fractions,
                                                  shared_added_wjlwtp_treatment_pool,
                                                  shared_added_wjlwtp_price,
                                                  city_infrastructure_rof_triggers[1],
                                                  construction_time_interval,
                                                  12 * WEEKS_IN_YEAR,
                                                  316.8);
    SequentialJointTreatmentExpansion caryWtpUpgrade1("Cary WTP upgrade 1",
                                                      22,
                                                      6,
                                                      56, // (72*7 - 448 = 56)
                                                      &cary_upgrades_treatment_capacity_fractions,
                                                      caryupgrades_2 * 7,
                                                      construction_time_interval,
                                                      NONE,
                                                      243. / 2);
    SequentialJointTreatmentExpansion caryWtpUpgrade2("Cary WTP upgrade 2",
                                                      23,
                                                      6,
                                                      56, // (7*80 - 72*7 = 56)
                                                      &cary_upgrades_treatment_capacity_fractions,
                                                      caryupgrades_3 * 7,
                                                      construction_time_interval,
                                                      NONE,
                                                      316.8 / 2);

    Reservoir dummy_endpoint("Dummy Node", 11, vector<Catchment *>(), 1., 0,
                             &evaporation_durham, 1, 1,
                             construction_time_interval,
                             0,
                             0);

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

    water_sources.push_back(&dummy_endpoint);

    /*
     * System connection diagram (water
     * flows from top to bottom)
     * Potential projects and expansions
     * of existing sources in parentheses
     *
     *      3(12,13)   4(24)   5(14)            0(15,16)    (18,19)
     *         \         /      /                  |
     *          \       /      /                   |
     *           \     /      /                    |
     *           |    /      /                    (9)
     *           |   /      /                      \
     *           |   |     /                        \
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

    Graph g(12);
    g.addEdge(0, 9);
    g.addEdge(9, 1);
    g.addEdge(1, 8);
    g.addEdge(8, 10);
    g.addEdge(10, 11);
    g.addEdge(2, 11);
    g.addEdge(7, 11);

    g.addEdge(3, 6);
    g.addEdge(4, 6);
    g.addEdge(5, 6);
    g.addEdge(6, 11);

    auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

    vector<int> cary_ws_return_id = {};
    auto *cary_discharge_fraction_series =
            new vector<vector<double>>();
    WwtpDischargeRule wwtp_discharge_cary(
            cary_discharge_fraction_series,
            &cary_ws_return_id);
    vector<int> owasa_ws_return_id = {6};
    WwtpDischargeRule wwtp_discharge_owasa(
            &demand_to_wastewater_fraction_owasa_raleigh,
            &owasa_ws_return_id);
    vector<int> raleigh_ws_return_id = {8};
    WwtpDischargeRule wwtp_discharge_raleigh(
            &demand_to_wastewater_fraction_owasa_raleigh,
            &raleigh_ws_return_id);
    vector<int> durham_ws_return_id = {1, 6};
    WwtpDischargeRule wwtp_discharge_durham(
            &demand_to_wastewater_fraction_durham,
            &durham_ws_return_id);

    vector<vector<int>> wjlwtp_remove_from_to_build_list;// = {{21, 20}};


    vector<int> demand_triggered_infra_order_cary = {22, 23};
    vector<double> demand_infra_cary = {caryupgrades_2 * 7, caryupgrades_3 * 7};
    Utility cary((char *) "Cary", 2, &demand_cary, demand_n_weeks, cary_annual_payment, &caryDemandClassesFractions,
                 &caryUserClassesWaterPrices, wwtp_discharge_cary, cary_inf_buffer, vector<int>(),
                 demand_triggered_infra_order_cary, demand_infra_cary, discount_rate, bond_term[0], bond_rate[0]);
    Utility durham((char *) "Durham", 1, &demand_durham, demand_n_weeks, durham_annual_payment,
                   &durhamDemandClassesFractions, &durhamUserClassesWaterPrices, wwtp_discharge_durham,
                   durham_inf_buffer, rof_triggered_infra_order_durham,
                   vector<int>(), rofs_infra_durham, discount_rate, &wjlwtp_remove_from_to_build_list, bond_term[1], bond_rate[1]);
    Utility owasa((char *) "OWASA", 0, &demand_owasa, demand_n_weeks, owasa_annual_payment,
                  &owasaDemandClassesFractions, &owasaUserClassesWaterPrices, wwtp_discharge_owasa, owasa_inf_buffer,
                  rof_triggered_infra_order_owasa,
                  vector<int>(), rofs_infra_owasa, discount_rate, &wjlwtp_remove_from_to_build_list, bond_term[2], bond_rate[2]);
    Utility raleigh((char *) "Raleigh", 3, &demand_raleigh, demand_n_weeks, raleigh_annual_payment,
                    &raleighDemandClassesFractions, &raleighUserClassesWaterPrices, wwtp_discharge_raleigh,
                    raleigh_inf_buffer, rof_triggered_infra_order_raleigh,
                    vector<int>(), rofs_infra_raleigh, discount_rate, &wjlwtp_remove_from_to_build_list, bond_term[3], bond_rate[3]);

    vector<Utility *> utilities;
    utilities.push_back(&cary);
    utilities.push_back(&durham);
    utilities.push_back(&owasa);
    utilities.push_back(&raleigh);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {3, 4,  5, 6,  12, 13, 14, 20, 21, 24}, //OWASA
            {0, 6,  9, 15, 16, 18, 19, 20, 21}, //Durham
            {6, 22, 23},                    //Cary
            {1, 2,  6, 7,  8,  17, 10, 20, 21}  //Raleigh
    };

    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    /// Restriction policies
    vector<double> initial_restriction_triggers = {OWASA_restriction_trigger,
                                                   Durham_restriction_trigger,
                                                   cary_restriction_trigger,
                                                   raleigh_restriction_trigger};

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
    Restrictions restrictions_r(3,
                                restriction_stage_multipliers_raleigh,
                                restriction_stage_triggers_raleigh);

    drought_mitigation_policies = {&restrictions_c, &restrictions_d,
                                   &restrictions_o, &restrictions_r};

    /// Transfer policy
    /*
     *      2
     *     / \
     *  0 v   v 1
     *   /     \
     *  3---><--1--><--0
     *      2       3
     */

    vector<int> buyers_ids = {0, 1, 3};
    //FIXME: CHECK IF TRANSFER CAPACITIES MATCH IDS IN BUYERS_IDS.
    vector<double> buyers_transfers_capacities = {10.8 * 7, 10.0 * 7, 11.5 * 7,
                                                  7.0 * 7};
    vector<double> buyers_transfers_trigger = {owasa_transfer_trigger,
                                               durham_transfer_trigger,
                                               raleigh_transfer_trigger};

    Graph ug(4);
    ug.addEdge(2, 1);
    ug.addEdge(2, 3);
    ug.addEdge(1, 3);
    ug.addEdge(1, 0);

    Transfers t(4,
                2,
                6,
                35,
                buyers_ids,
                buyers_transfers_capacities,
                buyers_transfers_trigger,
                ug,
                vector<double>(),
                vector<int>());
    drought_mitigation_policies.push_back(&t);

    vector<double> insurance_triggers = {owasa_insurance_use,
                                         durham_insurance_use, cary_insurance_use,
                                         raleigh_insurance_use}; //FIXME: Change per solution
    double fixed_payouts[4] = {owasa_insurance_payment,
                               durham_insurance_payment,
                               cary_insurance_payment,
                               raleigh_insurance_payment};
    vector<int> insured_utilities = {0, 1, 2, 3};
    InsuranceStorageToROF in(5,
                             water_sources,
                             g,
                             reservoir_utility_connectivity_matrix,
                             utilities, min_env_flow_controls,
                             &utilities_rdm,
                             &water_sources_rdm,
                             insurance_triggers, 1.2, fixed_payouts);

    drought_mitigation_policies.push_back(&in);

    if (utilities_rdm.empty()) {
        utilities_rdm = std::vector<vector<double>>(n_realizations, vector<double>(4, 1.));
        water_sources_rdm = std::vector<vector<double>>(n_realizations, vector<double>(49, 1.));
        policies_rdm = std::vector<vector<double>>(n_realizations, vector<double>(4, 1.));
    }

    /// Creates simulation object
    Simulation s(water_sources,
                 g,
                 reservoir_utility_connectivity_matrix,
                 utilities,
                 drought_mitigation_policies,
                 min_env_flow_controls,
                 &utilities_rdm,
                 &water_sources_rdm,
                 &policies_rdm,
                 n_weeks,
                 n_realizations);
    cout << "Beginning simulation." << endl;
    auto* mdc = s.runFullSimulation(n_threads, EXPORT_ROF_TABLES, DO_NOT_USE_PRE_CALCULATED_ROF_TABLES);
    this->master_data_collector = mdc;
    cout << "Ending simulation" << endl;

    vector<double> obj_not_jla = calculateObjectivesAndPrintOutput();
/*
    int i = 0;
    objs[i] = min(min(obj_not_jla[i*4], obj_not_jla[i*4+5]), min(obj_not_jla[i*4+10], obj_not_jla[i*4+15]));
    for (i = 1; i < 5; ++i) {
        objs[i] = max(max(obj_not_jla[i*4], obj_not_jla[i*4+5]), max(obj_not_jla[i*4+10], obj_not_jla[i*4+15]));
    }

    objs[5] = max(max(OWASA_JLA, Durham_JLA), max(Cary_JLA, Raleigh_JLA));*/

    cout << "objectives returned" << endl;
}

Triangle::Triangle(int solution_no, int rdm_no) {
}


