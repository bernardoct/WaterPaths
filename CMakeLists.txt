cmake_minimum_required(VERSION 3.6)
project(TriangleModel)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O3 -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

set(SOURCE_FILES
        src/ContinuityModels/Base/ContinuityModel.cpp
        src/ContinuityModels/Base/ContinuityModel.h
        src/ContinuityModels/ContinuityModelRealization.cpp
        src/ContinuityModels/ContinuityModelRealization.h
        src/ContinuityModels/ContinuityModelROF.cpp
        src/ContinuityModels/ContinuityModelROF.h
        src/Controls/Base/MinEnvironFlowControl.cpp
        src/Controls/Base/MinEnvironFlowControl.h
        src/Controls/Custom/JordanLakeMinEnvFlowControl.cpp
        src/Controls/Custom/JordanLakeMinEnvFlowControl.h
        src/Controls/Custom/FallsLakeMinEnvFlowControl.cpp
        src/Controls/Custom/FallsLakeMinEnvFlowControl.h
        src/Controls/InflowMinEnvFlowControl.cpp
        src/Controls/InflowMinEnvFlowControl.h
        src/Controls/SeasonalMinEnvFlowControl.cpp
        src/Controls/SeasonalMinEnvFlowControl.h
        src/Controls/StorageMinEnvFlowControl.cpp
        src/Controls/StorageMinEnvFlowControl.h
        src/Controls/FixedMinEnvFlowControl.cpp
        src/Controls/FixedMinEnvFlowControl.h
        src/Controls/WwtpDischargeRule.cpp
        src/Controls/WwtpDischargeRule.h
        src/DroughtMitigationInstruments/Base/DroughtMitigationPolicy.cpp
        src/DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h
        #        src/DroughtMitigationInstruments/InsurancePseudoROF.cpp
        #        src/DroughtMitigationInstruments/InsurancePseudoROF.h
        src/DroughtMitigationInstruments/InsuranceStorageToROF.cpp
        src/DroughtMitigationInstruments/InsuranceStorageToROF.h
        src/DroughtMitigationInstruments/Restrictions.cpp
        src/DroughtMitigationInstruments/Restrictions.h
        src/DroughtMitigationInstruments/Transfers.cpp
        src/DroughtMitigationInstruments/Transfers.h
        src/main.cpp
        src/Simulation/Simulation.cpp
        src/Simulation/Simulation.h
        src/SystemComponents/Catchment.cpp
        src/SystemComponents/Catchment.h
        src/SystemComponents/Utility.cpp
        src/SystemComponents/Utility.h
        src/SystemComponents/WaterSources/Base/WaterSource.cpp
        src/SystemComponents/WaterSources/Base/WaterSource.h
        src/SystemComponents/WaterSources/Intake.cpp
        src/SystemComponents/WaterSources/Intake.h
        src/SystemComponents/WaterSources/Quarry.cpp
        src/SystemComponents/WaterSources/Quarry.h
        src/SystemComponents/WaterSources/Reservoir.cpp
        src/SystemComponents/WaterSources/Reservoir.h
        src/SystemComponents/WaterSources/ReservoirExpansion.cpp
        src/SystemComponents/WaterSources/ReservoirExpansion.h
        src/SystemComponents/WaterSources/WaterReuse.cpp
        src/SystemComponents/WaterSources/WaterReuse.h
        src/Utils/Constants.h
        src/Utils/DataCollector.cpp
        src/Utils/DataCollector.h
        src/Utils/DataSeries.cpp
        src/Utils/DataSeries.h
        src/Utils/EvaporationSeries.cpp
        src/Utils/EvaporationSeries.h
        src/Utils/Graph/Graph.cpp
        src/Utils/Graph/Graph.h
        src/Utils/QPSolver/Array.cpp
        src/Utils/QPSolver/Array.h
        src/Utils/QPSolver/QuadProg++.cpp
        src/Utils/QPSolver/QuadProg++.h
        src/Utils/Utils.cpp
        src/Utils/Utils.h src/SystemComponents/WaterSources/AllocatedReservoir.cpp src/SystemComponents/WaterSources/AllocatedReservoir.h src/SystemComponents/WaterSources/JointWaterTreatmentPlant.cpp src/SystemComponents/WaterSources/JointWaterTreatmentPlant.h)
set(TEST_SOURCE_FILES src/SystemComponents/WaterSources/Reservoir.cpp src/SystemComponents/WaterSources/Reservoir.h src/ContinuityModels/Base/ContinuityModel.cpp src/ContinuityModels/Base/ContinuityModel.h src/Utils/Constants.h src/SystemComponents/Utility.cpp src/SystemComponents/Utility.h src/SystemComponents/Catchment.cpp src/SystemComponents/Catchment.h src/Utils/Utils.cpp src/Utils/Utils.h src/SystemComponents/WaterSources/Base/WaterSource.cpp src/SystemComponents/WaterSources/Base/WaterSource.h src/Simulation/Simulation.cpp src/Simulation/Simulation.h src/ContinuityModels/ContinuityModelROF.cpp src/ContinuityModels/ContinuityModelROF.h src/ContinuityModels/ContinuityModelRealization.cpp src/ContinuityModels/ContinuityModelRealization.h src/Utils/DataCollector.cpp src/Utils/DataCollector.h src/Utils/Graph/Graph.cpp src/Utils/Graph/Graph.h src/DroughtMitigationInstruments/Restrictions.cpp src/DroughtMitigationInstruments/Restrictions.h src/DroughtMitigationInstruments/Base/DroughtMitigationPolicy.cpp src/DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h src/SystemComponents/WaterSources/Intake.cpp src/SystemComponents/WaterSources/Intake.h src/DroughtMitigationInstruments/Transfers.cpp src/DroughtMitigationInstruments/Transfers.h src/Utils/QPSolver/Array.h src/Utils/QPSolver/Array.cpp src/Utils/QPSolver/QuadProg++.cpp src/Utils/QPSolver/QuadProg++.h TestFiles/tests.cpp src/DroughtMitigationInstruments/InsuranceStorageToROF.cpp src/DroughtMitigationInstruments/InsuranceStorageToROF.h src/SystemComponents/WaterSources/ReservoirExpansion.cpp src/SystemComponents/WaterSources/ReservoirExpansion.h src/SystemComponents/WaterSources/Quarry.cpp src/SystemComponents/WaterSources/Quarry.h src/SystemComponents/WaterSources/WaterReuse.cpp src/SystemComponents/WaterSources/WaterReuse.h src/Utils/Matrices.h src/Utils/DataSeries.cpp src/Utils/DataSeries.h src/Controls/Base/ControlRules.h src/Controls/WwtpDischargeRule.cpp src/Controls/WwtpDischargeRule.h src/Controls/Base/MinEnvironFlowControl.cpp src/Controls/Base/MinEnvironFlowControl.h src/Controls/SeasonalMinEnvFlowControl.cpp src/Controls/SeasonalMinEnvFlowControl.h src/Controls/StorageMinEnvFlowControl.cpp src/Controls/StorageMinEnvFlowControl.h src/Controls/InflowMinEnvFlowControl.cpp src/Controls/InflowMinEnvFlowControl.h src/Controls/Custom/JordanLakeMinEnvFlowControl.cpp src/Controls/Custom/JordanLakeMinEnvFlowControl.h src/Controls/Custom/FallsLakeMinEnvFlowControl.cpp src/Controls/Custom/FallsLakeMinEnvFlowControl.h src/SystemComponents/WaterSources/AllocatedReservoir.cpp src/SystemComponents/WaterSources/AllocatedReservoir.h src/SystemComponents/WaterSources/JointWaterTreatmentPlant.cpp src/SystemComponents/WaterSources/JointWaterTreatmentPlant.h)
add_executable(TriangleModel ${SOURCE_FILES})
add_executable(TestTriangleModel ${TEST_SOURCE_FILES})
