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
 double Watertown_LMA = 0.355;
 double Dryville_LMA = 0.10;
 double Fallsland_LMA = 0.05;
 double watertown_annual_payment = 0.05;
 double watertown_demand_buffer = 1.0;


 // ===================== SET UP PROBLEM COMPONENTS =====================
//Beginning with Reservoir continuity

 int streamflow_n_weeks = 52 * (70 + 50);


 //FIXME: Come back and sure these get loaded and are properly matched to res
 EvaporationSeries evaporation_durham(&evap_durham, streamflow_n_weeks); //Evaporation
 EvaporationSeries evaporation_falls_lake(&evap_falls_lake, streamflow_n_weeks); //Evaporation
 EvaporationSeries evaporation_jordan_lake(&evap_jordan_lake, streamflow_n_weeks); // Lake Michael
 EvaporationSeries evaporation_owasa(&evap_owasa, streamflow_n_weeks);

 /// Create catchments and corresponding vectors

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
/// CURRENTLY THIS IS THE STORAGE OF FALLS + DURHAM (MICHIE AND LR) + WB
/// ASSUMING DURHAM AND WB SCALE PROPORTIONALLY TO FALLS
 vector<double> autumn_lake_storage = {0, (23266+4257+1869) * table_gen_storage_multiplier, (34700+6349+2790) * table_gen_storage_multiplier};
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
/// Autumn is combining Falls+Durham+WB
 vector<int> autumn_controls_weeks = {13, 43};
 vector<double> autumn_releases = {(65+4+1)*7, (39+10+2)*7};

 SeasonalMinEnvFlowControl autumn_min_env_control(0, autumn_controls_weeks, autumn_releases);

 /// Lake Michael is based off the Jordan Lake and uses its class
 JordanLakeMinEnvFlowControl lake_michael_min_env_control(
        6, cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
        290.84, 387.79, 30825.0, 14924.0);

 //FIXME SUGAR CREEK BASED ON CCR, SO LEAVING AS IS, IS THIS A GOOD IDEA?
 vector<double> sugar_creek_inflows = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                  1.797 * 7};
 vector<double> sugar_creek_releases = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                   1.797 * 7};

 InflowMinEnvFlowControl sugar_creek_min_env_control(4, vector<int>(1, 4),
                                             sugar_creek_inflows,
                                             sugar_creek_releases);

 /// College Rock has no min flow
  FixedMinEnvFlowControl college_rock_min_env_control(1, 0);

 //FIXME made these numbers up, need to verify it works
  vector<int> new_river_controls_weeks = {13, 43};
  vector<double> new_river_releases = {3*7, 8*7};

  SeasonalMinEnvFlowControl new_river_min_env_control(0, new_river_controls_weeks, new_river_releases);

  vector<MinEnvFlowControl *> min_env_flow_controls;
  min_env_flow_controls.push_back(&autumn_min_env_control);
  min_env_flow_controls.push_back(&lake_michael_min_env_control);
  min_env_flow_controls.push_back(&sugar_creek_min_env_control);
  min_env_flow_controls.push_back(&college_rock_min_env_control);


    /// Lake Michael parameters
    double lake_michael_supply_capacity = 14924.0 * table_gen_storage_multiplier;
    double lake_michael_wq_capacity = 30825.0 * table_gen_storage_multiplier;
    double lake_michael_storage_capacity = lake_michael_wq_capacity + lake_michael_supply_capacity;
    vector<int> lake_michael_allocations_ids = {0, 1, 2, WATER_QUALITY_ALLOCATION};
    vector<double> lake_michael_allocation_fractions = {
            Watertown_LMA * lake_michael_supply_capacity / lake_michael_storage_capacity,
            Dryville_LMA * lake_michael_supply_capacity / lake_michael_storage_capacity,
            Fallsland_LMA * lake_michael_supply_capacity / lake_michael_storage_capacity,
            lake_michael_wq_capacity / lake_michael_storage_capacity};
    vector<double> lake_michael_treatment_allocation_fractions = {0.0, 0.0, 1.0, 0.0};


    /// Autumn Lake parameters
    /// WB and Durham storage added to water supply capacity
    double autumn_lake_supply_capacity = (14700.0+6349+2790) * table_gen_storage_multiplier;
    double autumn_lake_wq_capacity = 20000.0 * table_gen_storage_multiplier;
    double autumn_lake_storage_capacity = autumn_lake_wq_capacity + autumn_lake_supply_capacity;
    //FIXME LOOK INTO THIS
    vector<int> autumn_lake_allocations_ids = {3, 4, WATER_QUALITY_ALLOCATION};
    vector<double> autumn_lake_allocation_fractions = {
            0.5*autumn_lake_supply_capacity / autumn_lake_storage_capacity,
            0.5*autumn_lake_supply_capacity / autumn_lake_storage_capacity,
            autumn_lake_wq_capacity / autumn_lake_storage_capacity};
    vector<double> autumn_lake_treatment_allocation_fractions = {0.0, 0.5, 0.5};

// Create existing reservoirs

    /// combined university lake and stone quarry
    Reservoir college_rock_reservoir("College Rock Reservoir",
                                     0,
                                     catchment_college_rock,
                                     (449+200)*table_gen_storage_multiplier,
                                     ILLIMITED_TREATMENT_CAPACITY,
                                     evaporation_owasa,
                                     222);

    AllocatedReservoir lake_michael("Lake Michael",
                                    2,
                                    catchment_lower_haw_river,
                                    lake_michael_storage_capacity,
                                    448,
                                    evaporation_jordan_lake,
                                    13940,
                                    &lake_michael_allocations_ids,
                                    &lake_michael_allocation_fractions,
                                    &lake_michael_treatment_allocation_fractions);

    AllocatedReservoir autumn_lake("Autumn Lake",
                                  3,
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

    LevelDebtServiceBond sugar_bond(7, 263.0, 25, 0.05, vector<int>(1, 0));
    Reservoir sugar_creek_reservoir("Sugar Creek Reservoir",
                                    1,
                                    catchment_sugar_creek,
                                    2909,
                                    ILLIMITED_TREATMENT_CAPACITY,
                                    evaporation_owasa,
                                    &sugar_creek_storage_area,
                                    construction_time_interval,
                                    17 * WEEKS_IN_YEAR,
                                    sugar_bond);


    //FIXME check bond, this one is from little river raliegh
    LevelDebtServiceBond new_river_bond(7, 263.0, 25, 0.05, vector<int>(1, 0));
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



    LevelDebtServiceBond dummy_bond(11, 1., 1, 1., vector<int>(1, 0));
    Reservoir dummy_endpoint("Dummy Node", 6, vector<Catchment *>(), 1., 0, evaporation_durham, 1,
                            construction_time_interval, 0, dummy_bond);


    vector<WaterSource *> water_sources;
    water_sources.push_back(&autumn_lake);
    water_sources.push_back(&lake_michael);
    water_sources.push_back(&college_rock_reservoir);
    water_sources.push_back(&new_river_reservoir);
    water_sources.push_back(&sugar_creek_reservoir);


/*
 *
 *  0 College Rock Reservoir
 *   \
 *    \    (1) Sugar Creek Reservoir
 *     \   /
 *      \ /
 *       2 Lake Michael
 *        \
 *         \                 3 Autumn Lake
 *          \               /
 *    Lillington           /
 *            \           /
 *             \         /
 *              \       /
 *               \    (4) New River Reservoir
 *                \   /
 *                 \ /
 *                  |
 *                  |
 *                  5 Dummy Endpoint
 */

 Graph g(5);
 g.addEdge(0, 2);
 g.addEdge(1, 2);
 g.addEdge(2, 5);
 g.addEdge(3, 4);
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


//FIXME READ IN DEMAND, DEMANDCLASSESFRACTIONS, DEMANDCLASSPRICES,
 vector<int> demand_triggered_infra_order_watertown = {4};
 Utility watertown((char *) "Watertown", 0, demand_watertown, demand_n_weeks, watertown_annual_payment, &watertownDemandClassesFractions,
                &watertownUserClassesWaterPrices, wwtp_discharge_watertown, watertown_demand_buffer);


};

 PaperTestProblem::~PaperTestProblem() = default;

void PaperTestProblem::readInputData() {
    cout << "Reading input data." << endl;

#pragma omp parallel num_threads(20)
    {
#pragma omp single
        streamflows_durham = Utils::parse2DCsvFile(output_directory +
                                                   "/TestFiles/inflows" + evap_inflows_suffix + "/durham_inflows.csv", n_realizations);
#pragma omp single
        streamflows_flat = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/falls_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_swift = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/lake_wb_inflows.csv", n_realizations);
#pragma omp single
        streamflows_llr = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/little_river_raleigh_inflows.csv", n_realizations);
        // }
#pragma omp single
        streamflows_phils = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/stone_quarry_inflows.csv", n_realizations);
#pragma omp single
        streamflows_cane = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/cane_creek_inflows.csv", n_realizations);
#pragma omp single
        streamflows_morgan = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/university_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_haw = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/jordan_lake_inflows.csv", n_realizations);
#pragma omp single
        streamflows_lillington = Utils::parse2DCsvFile(output_directory + "/TestFiles/inflows" + evap_inflows_suffix + "/lillington_inflows.csv", n_realizations);
// };
        //cout << "Reading evaporations." << endl;
#pragma omp single
        evap_durham = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/durham_evap.csv", n_realizations);
#pragma omp single
        evap_falls_lake = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/falls_lake_evap.csv", n_realizations);
#pragma omp single
        evap_owasa = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/owasa_evap.csv", n_realizations);
#pragma omp single
        evap_little_river = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/little_river_raleigh_evap.csv", n_realizations);
#pragma omp single
        {
            evap_wheeler_benson = Utils::parse2DCsvFile(output_directory + "/TestFiles/evaporation" + evap_inflows_suffix + "/wb_evap.csv", n_realizations);
            evap_jordan_lake = evap_owasa;
        }

        //cout << "Reading demands." << endl;
#pragma omp single
        demand_watertown = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands" + evap_inflows_suffix + "/cary_demand.csv", n_realizations);
#pragma omp single
        demand_dryville = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands" + evap_inflows_suffix + "/durham_demand.csv", n_realizations);
#pragma omp single
        demand_fallsland = Utils::parse2DCsvFile(output_directory + "/TestFiles/demands" + evap_inflows_suffix + "/raleigh_demand.csv", n_realizations);

        //cout << "Reading others." << endl;
#pragma omp single
        {
            demand_to_wastewater_fraction_fallsland = Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_owasa_raleigh.csv");
            demand_to_wastewater_fraction_dryville = Utils::parse2DCsvFile(output_directory + "/TestFiles/demand_to_wastewater_fraction_durham.csv");

            watertownDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/caryDemandClassesFractions.csv");
            drvilleDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamDemandClassesFractions.csv");
            fallslandDemandClassesFractions = Utils::parse2DCsvFile(output_directory + "/TestFiles/raleighDemandClassesFractions.csv");

            watertownUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/caryUserClassesWaterPrices.csv");
            dryvilleUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/durhamUserClassesWaterPrices.csv");
            fallslandUserClassesWaterPrices = Utils::parse2DCsvFile(output_directory + "/TestFiles/raleighUserClassesWaterPrices.csv");
        }
//    cout << "Done reading input data." << endl;
    }

}