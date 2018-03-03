//
// Created by David on 2/26/2018.
//

#include "DurhamModel.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>
#include <fstream>
#include "../Controls/SeasonalMinEnvFlowControl.h"
#include "../Controls/FixedMinEnvFlowControl.h"
#include "../Controls/Custom/JordanLakeMinEnvFlowControl.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "../SystemComponents/WaterSources/Quarry.h"
#include "../SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/SequentialJointTreatmentExpansion.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "../Simulation/Simulation.h"

/**
 * Runs durham-only problem.
 * @param vars
 * @param n_realizations
 * @param n_weeks
 * @param sol_number
 * @param output_directory
 * @todo check for solutions in which a utility does not have an allocation
 * on Jordan Lake (or any generic lake) but still pay for joint treatment
 * infrastructure).
 */
void DurhamModel::functionEvaluation(const double *vars, double *objs, double *consts) {
    cout << "Building Durham Problem." << endl;
    //srand(0);//(unsigned int) time(nullptr));

    // ===================== SET UP DECISION VARIABLES  =====================

    double Durham_restriction_trigger = vars[0];
    double cary_restriction_trigger = vars[3];
    double durham_transfer_trigger = vars[4];
    double Durham_JLA = vars[9];
    double Cary_JLA = vars[10];
    double durham_annual_payment = vars[11]; // contingency fund
    double cary_annual_payment = vars[14];
    double durham_insurance_use = vars[15]; // insurance st_rof
    double cary_insurance_use = vars[18];
    double durham_insurance_payment = vars[19];
    double cary_insurance_payment = vars[22];
    double durham_inftrigger = vars[23];
    double cary_inftrigger = vars[26];
    if (import_export_rof_tables == EXPORT_ROF_TABLES) {
        durham_inftrigger = 1.1;
        cary_inftrigger = 1.1;
    }
    double Teer_quarry_expansion_ranking = vars[31]; // 9
    double reclaimed_water_ranking_low = vars[32]; // 18
    double reclaimed_water_high = vars[33]; // 19
    double lake_michie_expansion_ranking_low = vars[34]; // 15
    double lake_michie_expansion_ranking_high = vars[35]; // 16
    double western_wake_treatment_plant_rank_durham_low = vars[42]; // 20
    double western_wake_treatment_plant_rank_durham_high = vars[43]; // 21
//    double caryupgrades_1 = vars[46]; // not used: already built.
    double caryupgrades_2 = vars[47];
    double caryupgrades_3 = vars[48];
    double western_wake_treatment_frac_durham = vars[50];
    double durham_inf_buffer = vars[53];
    double cary_inf_buffer = vars[56];

    vector<infraRank> durham_infra_order_raw = {
            infraRank(2, Teer_quarry_expansion_ranking),
            infraRank(4, lake_michie_expansion_ranking_low),
            infraRank(5, lake_michie_expansion_ranking_high),
            infraRank(6, reclaimed_water_ranking_low),
            infraRank(7, reclaimed_water_high),
            infraRank(8, western_wake_treatment_plant_rank_durham_low),
            infraRank(9, western_wake_treatment_plant_rank_durham_high)
    };

    double added_storage_michie_expansion_low = 2500;
    double added_storage_michie_expansion_high = 5200;
    double reclaimed_capacity_low = 2.2 * 7;
    double reclaimed_capacity_high = 9.1 * 7;

    /// get infrastructure construction order based on decision variables
    sort(durham_infra_order_raw.begin(),
         durham_infra_order_raw.end(),
         by_xreal());

    vector<int> rof_triggered_infra_order_durham =
            vecInfraRankToVecInt(durham_infra_order_raw);
    vector<double> rofs_infra_durham = vector<double>
            (rof_triggered_infra_order_durham.size(), durham_inftrigger);

    /// Remove small expansions being built after big expansions that would
    /// encompass the smal expansions.
    added_storage_michie_expansion_high =
            checkAndFixInfraExpansionHighLowOrder(
                    &rof_triggered_infra_order_durham,
                    4,
                    5,
                    added_storage_michie_expansion_low,
                    added_storage_michie_expansion_high);

    reclaimed_capacity_high =
            checkAndFixInfraExpansionHighLowOrder(
                    &rof_triggered_infra_order_durham,
                    6,
                    7,
                    reclaimed_capacity_low,
                    reclaimed_capacity_high);


    /// Normalize Jordan Lake Allocations in case they exceed 1.
    double sum_jla_allocations = Durham_JLA + Cary_JLA;
    if (sum_jla_allocations > 1.) {
        Durham_JLA /= sum_jla_allocations;
        Cary_JLA /= sum_jla_allocations;
    }

    /// Normalize West Jordan Lake WTP allocations.
    double sum_wjlwtp = western_wake_treatment_frac_durham;
    western_wake_treatment_frac_durham /= sum_wjlwtp;

    /// Read streamflows
    int streamflow_n_weeks = 52 * (70 + 50);

    /// In case a vector containing realizations numbers to be calculated is passed, set
    /// number of realizations to number of realizations in that vector.

    EvaporationSeries evaporation_durham(&evap_durham, streamflow_n_weeks);
    EvaporationSeries evaporation_jordan_lake(&evap_jordan_lake, streamflow_n_weeks);

    /// Create catchments and corresponding vectors
    //  Durham (Upper Neuse River Basin)
    Catchment durham_inflows(&streamflows_durham, streamflow_n_weeks);

    // Cary (Lower Cape Fear Basin)
    Catchment lower_haw_river(&streamflows_haw, streamflow_n_weeks);

    // Downstream Gages
    Catchment cape_fear_river_at_lillington(&streamflows_lillington, streamflow_n_weeks);

    vector<Catchment *> catchment_durham;

    vector<Catchment *> catchment_haw;

    vector<Catchment *> gage_clayton;
    vector<Catchment *> gage_lillington;

    catchment_durham.push_back(&durham_inflows);

    catchment_haw.push_back(&lower_haw_river);

    gage_lillington.push_back(&cape_fear_river_at_lillington);

    /// Storage vs. area reservoir curves.
    vector<double> teer_storage = {0, 1315.0};
    vector<double> teer_area = {20, 50};

    DataSeries teer_storage_area(&teer_storage,
                                 &teer_area);

    /// Minimum environmental flow rules (controls)
    vector<int> dlr_weeks = {0, 21, 47, 53};
    vector<double> dlr_releases = {3.877 * 7, 9.05, 3.877 * 7};

    SeasonalMinEnvFlowControl durham_min_env_control(0, &dlr_weeks,
                                                     &dlr_releases);
    JordanLakeMinEnvFlowControl jordan_min_env_control(
            6, &cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
            290.84, 387.79, 30825.0, 14924.0);
    FixedMinEnvFlowControl teer_min_env_control(9, 0);

    vector<MinEnvironFlowControl *> min_env_flow_controls;
    min_env_flow_controls.push_back(&durham_min_env_control);
    min_env_flow_controls.push_back(&jordan_min_env_control);
    min_env_flow_controls.push_back(&teer_min_env_control);

    /// Create reservoirs and corresponding vector
    vector<double> construction_time_interval = {3.0, 5.0};
    vector<double> city_infrastructure_rof_triggers = {durham_inftrigger, cary_inftrigger};
    vector<double> bond_term = {25, 25};
    vector<double> bond_rate = {0.05, 0.05};
    double discount_rate = 0.08;

    /// Jordan Lake parameters
    double jl_supply_capacity = 14924.0 * table_gen_storage_multiplier;
    double jl_wq_capacity = 30825.0 * table_gen_storage_multiplier;
    double jl_storage_capacity = jl_wq_capacity + jl_supply_capacity;
    vector<int> jl_allocations_ids = {0, 1, WATER_QUALITY_ALLOCATION};
    vector<double> jl_allocation_fractions = {
            Cary_JLA * jl_supply_capacity / jl_storage_capacity,
            Durham_JLA * jl_supply_capacity / jl_storage_capacity,
            jl_wq_capacity / jl_storage_capacity};
    vector<double> jl_treatment_allocation_fractions = {0.0, 1.0, 0.0};

    // Existing Sources
    Reservoir durham_reservoirs("Lake Michie & Little River Res. (Durham)",
                                0,
                                catchment_durham,
                                6349.0 * table_gen_storage_multiplier,
                                ILLIMITED_TREATMENT_CAPACITY,
                                evaporation_durham, 1069);

    AllocatedReservoir jordan_lake("Jordan Lake",
                                   1,
                                   catchment_haw,
                                   jl_storage_capacity,
                                   448,
                                   evaporation_jordan_lake,
                                   13940,
                                   &jl_allocations_ids,
                                   &jl_allocation_fractions,
                                   &jl_treatment_allocation_fractions);

    double WEEKS_IN_YEAR = 0;

    // Potential Sources
    // The capacities listed here for expansions are what additional capacity is gained relative to existing capacity,
    // NOT the total capacity after the expansion is complete. For infrastructure with a high and low option, this means
    // the capacity for both is relative to current conditions - if Lake Michie is expanded small it will gain 2.5BG,
    // and a high expansion will provide 7.7BG more water than current. if small expansion happens, followed by a large
    // expansion, the maximum capacity through expansion is 7.7BG, NOT 2.5BG + 7.7BG.

    Quarry teer_quarry("Teer Quarry",
                       2,
                       vector<Catchment *>(),
                       1315.0,
                       ILLIMITED_TREATMENT_CAPACITY,
                       evaporation_durham,
                       &teer_storage_area,
                       city_infrastructure_rof_triggers[0],
                       construction_time_interval,
                       7 *
                       WEEKS_IN_YEAR,
                       22.6,
                       15 * 7); //FIXME: MAX PUMPING CAPACITY?

    // diversions to Teer Quarry for Durham based on inflows to downstream Falls Lake from the Flat River
    // FYI: spillage from Eno River also helps determine Teer quarry diversion, but Eno spillage isn't factored into
    // downstream water balance?

    ReservoirExpansion low_michie_expansion("Low Lake Michie Expansion",
                                            4,
                                            0,
                                            added_storage_michie_expansion_low,
                                            city_infrastructure_rof_triggers[0],
                                            construction_time_interval,
                                            17 * WEEKS_IN_YEAR,
                                            158.3);
    ReservoirExpansion high_michie_expansion("High Lake Michie Expansion",
                                             5,
                                             0,
                                             added_storage_michie_expansion_high,
                                             city_infrastructure_rof_triggers[0],
                                             construction_time_interval,
                                             17 * WEEKS_IN_YEAR,
                                             203.3);

    WaterReuse low_reclaimed("Low Reclaimed Water System",
                             6,
                             reclaimed_capacity_low,
                             city_infrastructure_rof_triggers[0],
                             construction_time_interval,
                             7 * WEEKS_IN_YEAR,
                             27.5);
    WaterReuse high_reclaimed("High Reclaimed Water System",
                              7,
                              reclaimed_capacity_high,
                              city_infrastructure_rof_triggers[0],
                              construction_time_interval,
                              7 * WEEKS_IN_YEAR,
                              104.4);

    WEEKS_IN_YEAR = Constants::WEEKS_IN_YEAR;

    // FIXME: TEMPORARY SCALAR MULTIPLIERS HERE TO TEST IF TREATMENT PLANT CAPACITY IS PROPERLY ACCOUNTED FOR
    vector<double> wjlwtp_treatment_capacity_fractions =
            {western_wake_treatment_frac_durham * 0.8, 0.0};
    vector<double> cary_upgrades_treatment_capacity_fractions = {0.0,1.0}; // allocations for durham and cary

    auto *shared_added_wjlwtp_treatment_pool = new vector<double>();
    auto *shared_added_wjlwtp_price = new vector<double>();
    SequentialJointTreatmentExpansion low_wjlwtp("Low WJLWTP",
                                                 8,
                                                 1,
                                                 0,
                                                 33 * 7,
                                                 &wjlwtp_treatment_capacity_fractions,
                                                 shared_added_wjlwtp_treatment_pool,
                                                 shared_added_wjlwtp_price,
                                                 city_infrastructure_rof_triggers[0],
                                                 construction_time_interval,
                                                 NONE, // was 12 years
                                                 243.3);
    SequentialJointTreatmentExpansion high_wjlwtp("High WJLWTP",
                                                  9,
                                                  1,
                                                  0,
                                                  54 * 7,
                                                  &wjlwtp_treatment_capacity_fractions,
                                                  shared_added_wjlwtp_treatment_pool,
                                                  shared_added_wjlwtp_price,
                                                  city_infrastructure_rof_triggers[0],
                                                  construction_time_interval,
                                                  NONE, // was 12 years
                                                  316.8);
    SequentialJointTreatmentExpansion caryWtpUpgrade1("Cary WTP upgrade 1",
                                                      10,
                                                      1,
                                                      56, // (72*7 - 448 = 56)
                                                      &cary_upgrades_treatment_capacity_fractions,
                                                      caryupgrades_2 * 7,
                                                      construction_time_interval,
                                                      NONE,
                                                      243. / 2);
    SequentialJointTreatmentExpansion caryWtpUpgrade2("Cary WTP upgrade 2",
                                                      11,
                                                      1,
                                                      56, // (7*80 - 72*7 = 56)
                                                      &cary_upgrades_treatment_capacity_fractions,
                                                      caryupgrades_3 * 7,
                                                      construction_time_interval,
                                                      NONE,
                                                      316.8 / 2);

    Reservoir dummy_endpoint("Dummy Node", 3, vector<Catchment *>(), 1., 0,
                             evaporation_durham, 1, 1,
                             construction_time_interval,
                             0,
                             0);

    vector<WaterSource *> water_sources;
    water_sources.push_back(&durham_reservoirs);
    water_sources.push_back(&jordan_lake);

    water_sources.push_back(&teer_quarry);
    water_sources.push_back(&low_michie_expansion);
    water_sources.push_back(&low_reclaimed);
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
     *          Jordan Lake     Lake Michie & LRR     Teer Quarry
     * SOURCE:      1      (6,7)     0(4,5)               (2)
     * TREATMT:   (8-11) Reclaimed
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
     *                    3: Dummy Endpoint
     */

    Graph g(4); // including dummy endpoint
    g.addEdge(0, 3);
    g.addEdge(1, 3);
    g.addEdge(2, 3);

    auto demand_n_weeks = (int) round(46 * WEEKS_IN_YEAR);

    vector<int> cary_ws_return_id = {};
    auto *cary_discharge_fraction_series =
            new vector<vector<double>>();
    WwtpDischargeRule wwtp_discharge_cary(
            cary_discharge_fraction_series,
            &cary_ws_return_id);
    vector<int> durham_ws_return_id = {1, 11};
    WwtpDischargeRule wwtp_discharge_durham(
            &demand_to_wastewater_fraction_durham,
            &durham_ws_return_id);

    vector<vector<int>> wjlwtp_remove_from_to_build_list;// = {{21, 20}};


    Utility durham((char *) "Durham", 0, &demand_durham, demand_n_weeks, durham_annual_payment,
                   &durhamDemandClassesFractions, &durhamUserClassesWaterPrices, wwtp_discharge_durham,
                   durham_inf_buffer, rof_triggered_infra_order_durham, vector<int>(),
                   rofs_infra_durham, discount_rate, &wjlwtp_remove_from_to_build_list, bond_term[0], bond_rate[0]);

    vector<int> demand_triggered_infra_order_cary = {10, 11};
    vector<double> demand_infra_cary = {caryupgrades_2 * 7, caryupgrades_3 * 7};
    Utility cary((char *) "Cary", 1, &demand_cary, demand_n_weeks, cary_annual_payment, &caryDemandClassesFractions,
                 &caryUserClassesWaterPrices, wwtp_discharge_cary, cary_inf_buffer, vector<int>(),
                 demand_triggered_infra_order_cary, demand_infra_cary, discount_rate, bond_term[1], bond_rate[1]);


    vector<Utility *> utilities;
    utilities.push_back(&durham);
    utilities.push_back(&cary);

    /// Water-source-utility connectivity matrix (each row corresponds to a utility and numbers are water
    /// sources IDs.
    vector<vector<int>> reservoir_utility_connectivity_matrix = {
            {0, 1, 2, 4, 5, 6, 7, 8, 9}, // Durham
            {1, 10, 11}                   // Cary
    };

    auto table_storage_shift = std::vector<std::vector<double>>(2, vector<double>(24, 0.));
    table_storage_shift[0][14] = 100.;
    table_storage_shift[0][20] = 500.;
    table_storage_shift[0][21] = 500.;

    vector<DroughtMitigationPolicy *> drought_mitigation_policies;
    /// Restriction policies
    vector<double> initial_restriction_triggers = {Durham_restriction_trigger,
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

    Restrictions restrictions_d(0,
                                restriction_stage_multipliers_durham,
                                restriction_stage_triggers_durham);
    Restrictions restrictions_c(1,
                                restriction_stage_multipliers_cary,
                                restriction_stage_triggers_cary);

    drought_mitigation_policies = {&restrictions_d, &restrictions_c};

    /// Transfer policy
    /*
     *   C(1) ---> D(0)
     */

    vector<int> buyers_ids = {0};
    //FIXME: CHECK IF TRANSFER CAPACITIES MATCH IDS IN BUYERS_IDS.
    vector<double> buyers_transfers_capacities = {10.8 * 7};
    vector<double> buyers_transfers_trigger = {durham_transfer_trigger};

    Graph ug(2);
    ug.addEdge(1, 0);

    Transfers t(2,
                1,
                1,
                35,
                buyers_ids,
                buyers_transfers_capacities,
                buyers_transfers_trigger,
                ug,
                vector<double>(),
                vector<int>());
    drought_mitigation_policies.push_back(&t);

    if (utilities_rdm.empty()) {
        /// All matrices below have dimensions n_realizations x nr_rdm_factors
        utilities_rdm = std::vector<vector<double>>(n_realizations, vector<double>(4, 1.));
        water_sources_rdm = std::vector<vector<double>>(n_realizations, vector<double>(49, 1.));
        policies_rdm = std::vector<vector<double>>(n_realizations, vector<double>(2, 1.));
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
                 import_export_rof_tables,
                 n_weeks,
                 realizations_to_run);

    if (import_export_rof_tables == IMPORT_ROF_TABLES)
        s.setPrecomputed_rof_tables(rof_tables, table_storage_shift);

    cout << "Beginning simulation." << endl;

    this->master_data_collector = s.runFullSimulation(n_threads);

    if (import_export_rof_tables != EXPORT_ROF_TABLES) {
        objectives = calculateAndPrintObjectives(false);

        int i = 0;
        objs[i] = min(min(objectives[i * 4], objectives[i * 4 + 5]),
                      min(objectives[i * 4 + 10], objectives[i * 4 + 15]));
        for (i = 1; i < 5; ++i) {
            objs[i] = max(max(objectives[i * 4], objectives[i * 4 + 5]),
                          max(objectives[i * 4 + 10], objectives[i * 4 + 15]));
        }

        objs[5] = max(Cary_JLA, Durham_JLA);

        objectives.push_back(objs[5]);
        cout << "objectives returned" << endl;
        objectives.clear();
    }

//    delete master_data_collector;
}

DurhamModel::~DurhamModel() {}

DurhamModel::DurhamModel(unsigned long n_weeks, int import_export_rof_table)
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
 * @param n_weeks number of weeks.
 */
void
DurhamModel::setRofTables(unsigned long n_realizations, int n_weeks, int n_utilities) {

    cout << "Loading ROF tables" << endl;

    string folder = "rof_tables";
    int n_tiers = NO_OF_INSURANCE_STORAGE_TIERS + 1;
    rof_tables = vector<Matrix3D<double>>(
            n_realizations, Matrix3D<double>(n_weeks, n_utilities, n_tiers));

//    double start_loading = omp_get_wtime();
//#pragma omp parallel for
    for (int r = 0; r < n_realizations; ++r) {
//        double start_realization = omp_get_wtime();
        string file_name = folder + "/tables_r" + to_string(r);
        ifstream in(file_name, ios_base::binary);
        if (!in.good()) {
            string error_table_file = "Tables file not found: " + file_name;
            __throw_invalid_argument(error_table_file.c_str());
        }

        unsigned stringsize;
        in.read(reinterpret_cast<char *>(&stringsize), sizeof(unsigned));

        double data[stringsize];
        in.read(reinterpret_cast<char *>(&data),
                stringsize * sizeof(double));

        rof_tables[r].setData(data, stringsize);

        for (int i = 0; i < stringsize; ++i) {
            double d = data[i];
            if (std::isnan(d) || d > 1.01 || d < 0) {
                string error_m = "nan or out of [0,1] rof imported "
                                         "tables. Realization " +
                                 to_string(r);
                printf("%s", error_m.c_str());
                __throw_logic_error(error_m.c_str());
            }
        }

        in.close();

//        printf("Tables for realization %d loaded in %f s.\n", r,
//               omp_get_wtime () - start_realization);
    }
//    printf("ROF Tables loaded in %f s.\n",
//           omp_get_wtime () - start_loading);
}

void DurhamModel::readInputData(){

    cout << "Reading input data." << endl;
    cout << "Scenario " << scenario << endl;

// #pragma omp parallel {
    // #pragma omp single {
    //streamflows_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/durham_inflows.csv", n_realizations);
    //streamflows_haw = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/jordan_lake_inflows.csv", n_realizations);

    streamflows_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/DurhamInflows_scenario1.csv", n_realizations);
    streamflows_haw = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/Jordan_scenario1.csv", n_realizations);

    streamflows_lillington = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/LillingtonInflows_Averages.csv", n_realizations);
// };
    //cout << "Reading evaporations." << endl;
    evap_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/durham_evap.csv", n_realizations);
    evap_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/owasa_evap.csv", n_realizations);
    evap_jordan_lake = evap_owasa;

    //cout << "Reading demands." << endl;
    demand_cary = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/cary_demand.csv", n_realizations);
    demand_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands/durham_demand.csv", n_realizations);

    //cout << "Reading others." << endl;
    demand_to_wastewater_fraction_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_durham.csv");
//
    caryDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/caryDemandClassesFractions.csv");
    durhamDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamDemandClassesFractions.csv");

    caryUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/caryUserClassesWaterPrices.csv");
    durhamUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamUserClassesWaterPrices.csv");

    cout << "Done reading input data." << endl;

}

void DurhamModel::setImport_export_rof_tables(int import_export_rof_tables, int n_weeks) {
    if (std::abs(import_export_rof_tables) > 1)
        __throw_invalid_argument("Import/export ROF tables can be assigned as:\n"
                                         "-1 - import tables\n"
                                         "0 - ignore tables\n"
                                         "1 - export tables.\n"
                                         "The value entered is invalid.");
    DurhamModel::import_export_rof_tables = import_export_rof_tables;

    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        DurhamModel::setRofTables(n_realizations, n_weeks, n_utilities);
    }
}

void DurhamModel::setScenario(int scen) {
    scenario = scen;
}
