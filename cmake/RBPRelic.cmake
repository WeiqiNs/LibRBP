include(FetchContent)

set(RBP_RELIC_GIT_TAG "main" CACHE STRING "RELIC branch, tag or commit to build")

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

function(rbp_build_relic name)
    set(preset ${relic_SOURCE_DIR}/preset/${RBP_CURVE_${name}_PRESET}.sh)
    if (NOT EXISTS ${preset})
        message(FATAL_ERROR "RELIC has no preset ${RBP_CURVE_${name}_PRESET} for curve '${name}'.")
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
    string(SHA256 fingerprint "${relic_configuration};${RBP_RELIC_OPTIONS};${CMAKE_C_COMPILER}")
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
                COMMAND ${CMAKE_COMMAND} -DLABEL=${name} ${RBP_RELIC_OPTIONS} .
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
endfunction()
