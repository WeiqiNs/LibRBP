cmake_minimum_required(VERSION 3.25)
project(LibRBPDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(RBP REQUIRED COMPONENTS bls12_381 ss1536 bn254)

add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE RBP::RBP)
