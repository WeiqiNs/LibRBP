cmake_minimum_required(VERSION 3.20)
project(RBPTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(RELIC REQUIRED)
find_package(RBP REQUIRED)

add_executable(demo demo.cpp)

target_link_libraries(demo PUBLIC RBP::BP_LIB)

message(STATUS "RBP_INCLUDE_DIRS: ${RBP_INCLUDE_DIRS}")
message(STATUS "RBP_LIBRARIES: ${RBP_LIBRARIES}")
