cmake_minimum_required(VERSION 3.6)
project(TriangleModel)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Model/main.cpp Model/WaterSources/Reservoir.cpp Model/WaterSources/Reservoir.h Model/Region.cpp Model/Region.h Model/Constants.h Model/Utility.cpp Model/Utility.h Model/Catchment.cpp Model/Catchment.h Model/Aux.cpp Model/Aux.h)
add_executable(TriangleModel ${SOURCE_FILES})