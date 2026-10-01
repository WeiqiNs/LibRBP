cmake_minimum_required(VERSION 3.25)

project(
        LibRBP
        VERSION 0.2
        DESCRIPTION "Typed C++20 bilinear pairing groups over one or more RELIC curves."
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

option(RBP_BUILD_TESTS "Build the LibRBP test suite" ${PROJECT_IS_TOP_LEVEL})
option(RBP_ENABLE_COVERAGE "Build LibRBP and its tests with --coverage" OFF)
set(RBP_CURVES "bls12_381;ss1536;bn254" CACHE STRING "Curves to build, from cmake/RBPCurves.cmake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/RBPCurves.cmake)
include(cmake/RBPRelic.cmake)
find_library(RBP_GMP_LIBRARY gmp REQUIRED)

if (RBP_ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif ()

set(RBP_GENERATED_DIR ${PROJECT_BINARY_DIR}/generated)
set(RBP_CORE_SOURCES
        ${PROJECT_SOURCE_DIR}/src/runtime.cpp
        ${PROJECT_SOURCE_DIR}/src/zp.cpp
        ${PROJECT_SOURCE_DIR}/src/point.cpp
        ${PROJECT_SOURCE_DIR}/src/gt.cpp
        ${PROJECT_SOURCE_DIR}/src/pairing.cpp
)

function(rbp_add_curve name)
    if (NOT name IN_LIST RBP_REGISTERED_CURVES)
        message(FATAL_ERROR "Unknown curve '${name}'. Registered curves: ${RBP_REGISTERED_CURVES}")
    endif ()
    rbp_build_relic(${name})
    rbp_measure_curve(${name} ${RBP_RELIC_${name}_INCLUDE_DIRS})

    set(RBP_NAME ${name})
    set(RBP_TAG ${RBP_CURVE_${name}_TAG})
    set(RBP_ZP_SIZE ${RBP_${name}_zp_SIZE})
    set(RBP_G1_SIZE ${RBP_${name}_g1_SIZE})
    set(RBP_G2_SIZE ${RBP_${name}_g2_SIZE})
    set(RBP_GT_SIZE ${RBP_${name}_gt_SIZE})
    if (RBP_${name}_IS_SYMMETRIC)
        set(RBP_SYMMETRIC true)
    else ()
        set(RBP_SYMMETRIC false)
    endif ()
    string(SHA256 layout "${RBP_ZP_SIZE};${RBP_G1_SIZE};${RBP_G2_SIZE};${RBP_GT_SIZE};${RBP_SYMMETRIC}")
    string(SUBSTRING ${layout} 0 8 RBP_LAYOUT)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/curve.hpp.in ${RBP_GENERATED_DIR}/include/rbp/${name}.hpp @ONLY)
    file(CONFIGURE OUTPUT ${RBP_GENERATED_DIR}/private/${name}/curve.hpp CONTENT [=[
#pragma once

#include "rbp/${name}.hpp"

namespace rbp::detail{
    using Tag = ${RBP_TAG};
}
]=])

    set(sources ${RBP_CORE_SOURCES})
    if (RBP_SYMMETRIC)
        list(APPEND sources ${PROJECT_SOURCE_DIR}/src/pairing_symmetric.cpp)
    endif ()

    set(target RBP_${name})
    add_library(${target} SHARED ${sources})
    add_library(RBP::${RBP_TAG} ALIAS ${target})
    target_include_directories(${target}
            PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${RBP_GENERATED_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            PRIVATE
            ${PROJECT_SOURCE_DIR}/src
            ${RBP_GENERATED_DIR}/private/${name}
            ${RBP_RELIC_${name}_INCLUDE_DIRS}
    )
    target_link_libraries(${target} PRIVATE $<BUILD_INTERFACE:relic_${name}> ${RBP_GMP_LIBRARY})
    target_link_options(${target} PRIVATE -Wl,--exclude-libs,ALL)
    set_target_properties(${target} PROPERTIES
            EXPORT_NAME ${RBP_TAG}
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION ${PROJECT_VERSION_MAJOR}.${RBP_LAYOUT}
    )
    set(RBP_${name}_LAYOUT ${RBP_LAYOUT} PARENT_SCOPE)
endfunction()

foreach (curve IN LISTS RBP_CURVES)
    rbp_add_curve(${curve})
endforeach ()

list(TRANSFORM RBP_CURVES PREPEND "#include \"rbp/" OUTPUT_VARIABLE rbp_includes)
list(TRANSFORM rbp_includes APPEND ".hpp\"")
list(JOIN rbp_includes "\n" rbp_includes)
file(CONFIGURE OUTPUT ${RBP_GENERATED_DIR}/include/rbp/rbp.hpp CONTENT [=[
#pragma once

#include "rbp/core.hpp"
#include "rbp/dlog.hpp"
#include "rbp/io.hpp"
#include "rbp/linalg.hpp"
${rbp_includes}
]=])

add_library(RBP INTERFACE)
add_library(RBP::RBP ALIAS RBP)
list(TRANSFORM RBP_CURVES PREPEND "RBP_" OUTPUT_VARIABLE rbp_curve_targets)
target_link_libraries(RBP INTERFACE ${rbp_curve_targets})

install(TARGETS RBP ${rbp_curve_targets} EXPORT RBPTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/rbp ${RBP_GENERATED_DIR}/include/rbp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
set(RBP_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)
install(EXPORT RBPTargets NAMESPACE RBP:: DESTINATION ${RBP_CMAKE_INSTALL_DIR})
configure_package_config_file(cmake/RBPConfig.cmake.in ${PROJECT_BINARY_DIR}/RBPConfig.cmake
        INSTALL_DESTINATION ${RBP_CMAKE_INSTALL_DIR}
)
write_basic_package_version_file(${PROJECT_BINARY_DIR}/RBPConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion
)
install(FILES ${PROJECT_BINARY_DIR}/RBPConfig.cmake ${PROJECT_BINARY_DIR}/RBPConfigVersion.cmake
        DESTINATION ${RBP_CMAKE_INSTALL_DIR}
)

if (RBP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()
