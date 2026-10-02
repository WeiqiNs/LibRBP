include(CheckTypeSize)
include(CheckCSourceCompiles)

set(RBP_REGISTERED_CURVES "")

function(rbp_register_curve name preset)
    if (NOT name MATCHES "^[a-z_][a-z0-9_]*$")
        message(FATAL_ERROR "Curve name '${name}' must be a lowercase C identifier, because it becomes a RELIC label.")
    endif ()
    if (name IN_LIST RBP_REGISTERED_CURVES)
        message(FATAL_ERROR "Curve '${name}' is registered twice.")
    endif ()
    string(TOUPPER ${name} tag)
    set(RBP_REGISTERED_CURVES ${RBP_REGISTERED_CURVES} ${name} PARENT_SCOPE)
    set(RBP_CURVE_${name}_TAG ${tag} PARENT_SCOPE)
    set(RBP_CURVE_${name}_PRESET ${preset} PARENT_SCOPE)
endfunction()

rbp_register_curve(bls12_381 gmp-pbc-bls381)
rbp_register_curve(ss1536 gmp-pbc-ss1536)
rbp_register_curve(bn254 gmp-pbc-bn254)

function(rbp_measure_curve name)
    set(CMAKE_REQUIRED_INCLUDES ${RBP_RELIC_${name}_INCLUDE_DIRS})
    set(CMAKE_EXTRA_INCLUDE_FILES relic.h)
    set(CMAKE_REQUIRED_QUIET ON)
    set(key RBP_${name}_${RBP_RELIC_${name}_FINGERPRINT})
    foreach (kind IN ITEMS ZP:bn_st G1:g1_t G2:g2_t GT:gt_t)
        string(REPLACE ":" ";" pair ${kind})
        list(GET pair 0 field)
        list(GET pair 1 type)
        check_type_size(${type} ${key}_${field} LANGUAGE C)
        if (NOT ${key}_${field})
            message(FATAL_ERROR "Could not measure ${type} for curve '${name}'.")
        endif ()
        set(RBP_${field}_SIZE ${${key}_${field}} PARENT_SCOPE)
    endforeach ()
    check_c_source_compiles("
        #include <relic.h>
        #if !pc_map_is_type1()
        #error asymmetric
        #endif
        int main(){ return 0; }" ${key}_SYMMETRIC)
    if (${key}_SYMMETRIC)
        set(RBP_SYMMETRIC true PARENT_SCOPE)
    else ()
        set(RBP_SYMMETRIC false PARENT_SCOPE)
    endif ()
endfunction()

function(rbp_add_curve name)
    if (NOT name IN_LIST RBP_REGISTERED_CURVES)
        message(FATAL_ERROR "Unknown curve '${name}'. Registered curves: ${RBP_REGISTERED_CURVES}")
    endif ()
    rbp_build_relic(${name})
    rbp_measure_curve(${name})

    set(RBP_NAME ${name})
    set(RBP_TAG ${RBP_CURVE_${name}_TAG})
    string(SHA256 layout "${RBP_ZP_SIZE};${RBP_G1_SIZE};${RBP_G2_SIZE};${RBP_GT_SIZE};${RBP_SYMMETRIC}")
    string(SUBSTRING ${layout} 0 8 layout)
    configure_file(cmake/curve.hpp.in ${RBP_GENERATED_DIR}/include/rbp/${name}.hpp @ONLY)
    configure_file(cmake/curve_detail.hpp.in ${RBP_GENERATED_DIR}/private/${name}/curve.hpp @ONLY)

    set(target RBP_${name})
    add_library(${target} SHARED src/runtime.cpp src/zp.cpp src/point.cpp src/gt.cpp src/pairing.cpp)
    if (RBP_SYMMETRIC)
        target_sources(${target} PRIVATE src/pairing_symmetric.cpp)
    endif ()
    add_library(RBP::${RBP_TAG} ALIAS ${target})
    target_compile_features(${target} PUBLIC cxx_std_20)
    target_include_directories(${target}
            PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${RBP_GENERATED_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            PRIVATE
            src
            ${RBP_GENERATED_DIR}/private/${name}
            ${RBP_RELIC_${name}_INCLUDE_DIRS}
    )
    target_link_libraries(${target} PRIVATE $<BUILD_INTERFACE:relic_${name}> ${RBP_GMP_LIBRARY})
    target_link_options(${target} PRIVATE -Wl,--exclude-libs,ALL)
    set_target_properties(${target} PROPERTIES
            EXPORT_NAME ${RBP_TAG}
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION ${PROJECT_VERSION_MAJOR}.${layout}
    )
    install(TARGETS ${target} EXPORT RBPTargets)
    set(RBP_${name}_LAYOUT ${layout} PARENT_SCOPE)
endfunction()
