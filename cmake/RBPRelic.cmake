include(FetchContent)

set(RBP_RELIC_GIT_TAG "9fc7356e3c304ec3b0f2f5c34c8ff4a12bac3783" CACHE STRING "RELIC branch, tag or commit to build")

FetchContent_Declare(relic
        GIT_REPOSITORY https://github.com/relic-toolkit/relic.git
        GIT_TAG ${RBP_RELIC_GIT_TAG}
        SOURCE_SUBDIR rbp-do-not-add-subdirectory
)
FetchContent_MakeAvailable(relic)

execute_process(
        COMMAND git -C ${relic_SOURCE_DIR} rev-parse HEAD
        OUTPUT_VARIABLE RBP_RELIC_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
message(STATUS "LibRBP: RELIC ${RBP_RELIC_GIT_TAG} resolved to '${RBP_RELIC_COMMIT}'")

function(rbp_run step)
    execute_process(${ARGN} RESULT_VARIABLE result OUTPUT_QUIET ERROR_VARIABLE errors)
    if (NOT result EQUAL 0)
        message(FATAL_ERROR "LibRBP: ${step} failed:\n${errors}")
    endif ()
endfunction()

set(RBP_RELIC_OPTIONS
        -DQUIET=on -DCHECK=off -DRAND=HASHD -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DSHLIB=off -DSTLIB=on -DTESTS=0 -DBENCH=0 -DDOCUM=off -DCMAKE_BUILD_TYPE=Release
)
set(RBP_RELIC_X64_OPTIONS "-DCFLAGS=-O3 -funroll-loops -fomit-frame-pointer -finline-small-functions")

set(RBP_ARITH auto CACHE STRING "RELIC field arithmetic: auto (x86-64 assembly where available) or gmp")
set_property(CACHE RBP_ARITH PROPERTY STRINGS auto gmp)
if (NOT RBP_ARITH MATCHES "^(auto|gmp)$")
    message(FATAL_ERROR "RBP_ARITH must be auto or gmp, not '${RBP_ARITH}'.")
endif ()

set(RBP_X64_AVAILABLE OFF)
set(RBP_X64_ADX OFF)
if (RBP_ARITH STREQUAL "auto" AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$" AND NOT CMAKE_CROSSCOMPILING)
    set(RBP_X64_AVAILABLE ON)
    try_run(adx_run adx_compiled SOURCE_FROM_CONTENT adx.c [[
        #include <cpuid.h>
        int main(void){
            unsigned a, b, c, d;
            if (!__get_cpuid_count(7, 0, &a, &b, &c, &d)) return 1;
            return (b & bit_BMI2) && (b & bit_ADX) ? 0 : 1;
        }
    ]])
    if (adx_compiled AND adx_run EQUAL 0)
        set(RBP_X64_ADX ON)
    endif ()
endif ()

function(rbp_build_relic name)
    set(preset_name ${RBP_CURVE_${name}_PRESET})
    set(options ${RBP_RELIC_OPTIONS})
    set(link_options "")
    if (RBP_X64_AVAILABLE AND RBP_CURVE_${name}_X64_PRESET AND (RBP_X64_ADX OR NOT RBP_CURVE_${name}_X64_NEEDS_ADX))
        set(preset_name ${RBP_CURVE_${name}_X64_PRESET})
        list(APPEND options ${RBP_RELIC_X64_OPTIONS})
        foreach (alias IN LISTS RBP_CURVE_${name}_X64_ALIASES)
            list(APPEND link_options -Wl,--undefined=${alias} -Wl,--defsym=${name}_${alias}=${alias})
        endforeach ()
    endif ()
    message(STATUS "LibRBP: ${name} builds RELIC with preset ${preset_name}")
    set(preset ${relic_SOURCE_DIR}/preset/${preset_name}.sh)
    if (NOT EXISTS ${preset})
        message(FATAL_ERROR "RELIC has no preset ${preset_name} for curve '${name}'.")
    endif ()
    set(binary_dir ${PROJECT_BINARY_DIR}/relic/${name})
    set(stamp ${binary_dir}/rbp-configured.stamp)
    file(GLOB relic_configuration_inputs ${relic_SOURCE_DIR}/CMakeLists.txt ${relic_SOURCE_DIR}/cmake/*.cmake
            ${relic_SOURCE_DIR}/include/*.h ${relic_SOURCE_DIR}/include/*.in ${relic_SOURCE_DIR}/include/low/*.h)
    set(relic_configuration "")
    foreach (input IN LISTS preset relic_configuration_inputs)
        file(SHA256 ${input} digest)
        string(APPEND relic_configuration ${digest})
    endforeach ()
    string(SHA256 fingerprint "${relic_configuration};${options};${CMAKE_C_COMPILER}")
    if (EXISTS ${stamp})
        file(READ ${stamp} previous)
    endif ()
    if (NOT previous STREQUAL fingerprint)
        file(REMOVE_RECURSE ${binary_dir})
        file(MAKE_DIRECTORY ${binary_dir})
        rbp_run("RELIC preset for ${name}"
                COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} sh ${preset} ${relic_SOURCE_DIR}
                WORKING_DIRECTORY ${binary_dir})
        rbp_run("RELIC options for ${name}"
                COMMAND ${CMAKE_COMMAND} -DLABEL=${name} ${options} .
                WORKING_DIRECTORY ${binary_dir})
        file(WRITE ${stamp} "${fingerprint}")
    endif ()

    set(archive ${binary_dir}/lib/librelic_s_${name}.a)
    add_custom_target(relic_${name}_build
            COMMAND ${CMAKE_COMMAND} --build ${binary_dir} --target relic_s_${name} --parallel
            BYPRODUCTS ${archive}
            COMMENT "Building RELIC for ${name}"
            VERBATIM
    )
    add_library(relic_${name} STATIC IMPORTED)
    set_target_properties(relic_${name} PROPERTIES IMPORTED_LOCATION ${archive})
    add_dependencies(relic_${name} relic_${name}_build)
    string(SUBSTRING ${fingerprint} 0 12 short_fingerprint)
    set(RBP_RELIC_${name}_FINGERPRINT ${short_fingerprint} PARENT_SCOPE)
    set(RBP_RELIC_${name}_INCLUDE_DIRS ${binary_dir}/include ${relic_SOURCE_DIR}/include PARENT_SCOPE)
    set(RBP_RELIC_${name}_LINK_OPTIONS ${link_options} PARENT_SCOPE)
endfunction()
