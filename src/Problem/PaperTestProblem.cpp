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
#include "../SystemComponents/WaterSources/ReservoirExpansion.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/SequentialJointTreatmentExpansion.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "../DroughtMitigationInstruments/InsuranceStorageToROF.h"
#include "../Simulation/Simulation.h"
#include "../SystemComponents/Bonds/LevelDebtServiceBond.h"
#include "../SystemComponents/Bonds/BalloonPaymentBond.h"


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

// Create minimum environmental flow rules (controls)
/// Again, autumn is combining Falls+Durham+WB
 vector<int> autumn_controls_weeks = {13, 43};
 vector<double> autumn_releases = {(65+4+1)*7, (39+10+2)*7};

 SeasonalMinEnvFlowControl autumn_min_env_control(0, autumn_controls_weeks, autumn_releases);

 JordanLakeMinEnvFlowControl lake_michael_min_env_control(
        6, cape_fear_river_at_lillington, 64.63, 129.26, 25.85, 193.89,
        290.84, 387.79, 30825.0, 14924.0);

 //FIXME COLLEGE ROCK BASED ON CCR, SO LEAVING AS IS, IS THIS A GOOD IDEA?
 vector<double> college_rock_inflows = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                  1.797 * 7};
 vector<double> college_rock_releases = {0.1422 * 7, 0.5 * 7, 1 * 7, 1.5 * 7,
                                   1.797 * 7};

 InflowMinEnvFlowControl ccr_min_env_control(4, vector<int>(1, 4),
                                             college_rock_inflows,
                                             college_rock_releases);




// Create existing reservoirs

// Create potential sources

/*
 *
 *  1 College Rock Reservoir
 *   \
 *    \    (2) Sugar Creek Reservoir
 *     \   /
 *      \ /
 *       3 Lake Michael
 *        \
 *         \        4 Autumn Lake
 *          \      /
 *           \    /
 *            \  /
 *            (5) New River Reservoir
 *             |
 *             |
 *         lillington
 *             |
 *          6:Dummy Endpoint
 */




};
