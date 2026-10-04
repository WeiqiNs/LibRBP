cmake_minimum_required(VERSION 3.25)

project(
        LibRBP
        VERSION 0.3
        DESCRIPTION "Typed C++20 bilinear pairing groups over one or more RELIC curves."
        LANGUAGES C CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)
if (PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

option(RBP_BUILD_TESTS "Build the LibRBP test suite" ${PROJECT_IS_TOP_LEVEL})
option(RBP_ENABLE_COVERAGE "Build LibRBP and its tests with --coverage" OFF)
set(RBP_CURVES "bls12_381;ss1536;bn254" CACHE STRING "Curves to build, from cmake/RBPCurves.cmake")

include(GNUInstallDirs)
include(cmake/RBPCurves.cmake)
include(cmake/RBPRelic.cmake)
find_library(RBP_GMP_LIBRARY gmp REQUIRED)

if (RBP_ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif ()

set(RBP_GENERATED_DIR ${PROJECT_BINARY_DIR}/generated)

add_library(RBP INTERFACE)
add_library(RBP::RBP ALIAS RBP)
set(RBP_CURVE_INCLUDES "")
foreach (curve IN LISTS RBP_CURVES)
    rbp_add_curve(${curve})
    target_link_libraries(RBP INTERFACE RBP_${curve})
    string(APPEND RBP_CURVE_INCLUDES "#include \"${curve}.hpp\"\n")
endforeach ()

configure_file(cmake/rbp.hpp.in ${RBP_GENERATED_DIR}/include/rbp/rbp.hpp @ONLY)

include(cmake/RBPInstall.cmake)

if (RBP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()
