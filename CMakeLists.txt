cmake_minimum_required(VERSION 3.6)
project(TriangleModel)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/SystemComponents/WaterSources/Reservoir.cpp src/SystemComponents/WaterSources/Reservoir.h src/ContinuityModels/ContinuityModel.cpp src/ContinuityModels/ContinuityModel.h src/Utils/Constants.h src/SystemComponents/Utility.cpp src/SystemComponents/Utility.h src/SystemComponents/Catchment.cpp src/SystemComponents/Catchment.h src/Utils/Aux.cpp src/Utils/Aux.h src/SystemComponents/WaterSources/WaterSource.cpp src/SystemComponents/WaterSources/WaterSource.h src/Simulation.cpp src/Simulation.h src/ContinuityModels/ContinuityModelROF.cpp src/ContinuityModels/ContinuityModelROF.h src/ContinuityModels/ContinuityModelRealization.cpp src/ContinuityModels/ContinuityModelRealization.h src/Utils/DataCollector.cpp src/Utils/DataCollector.h)
add_executable(TriangleModel ${SOURCE_FILES})