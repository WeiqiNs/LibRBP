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

file(READ ${relic_SOURCE_DIR}/src/relic_core.c relic_core)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${relic_SOURCE_DIR}/src/relic_core.c)
string(REPLACE "rlc_thread ctx_t first_ctx;" "ctx_t first_ctx;" RBP_RELIC_PATCHED_CORE "${relic_core}")
if (RBP_RELIC_PATCHED_CORE STREQUAL relic_core)
    message(FATAL_ERROR "LibRBP: RELIC's src/relic_core.c no longer declares 'rlc_thread ctx_t first_ctx;', which "
            "LibRBP turns into an ordinary global so that threads which never use RELIC do not reserve a context.")
endif ()
string(SHA256 RBP_RELIC_CORE_DIGEST "${RBP_RELIC_PATCHED_CORE}")

function(rbp_run step)
    execute_process(${ARGN} RESULT_VARIABLE result OUTPUT_QUIET ERROR_VARIABLE errors)
    if (NOT result EQUAL 0)
        message(FATAL_ERROR "LibRBP: ${step} failed:\n${errors}")
    endif ()
endfunction()

set(RBP_RELIC_OPTIONS
        -DQUIET=on -DCHECK=off -DRAND=HASHD -DSEED=UDEV -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DSHLIB=off -DSTLIB=on -DTESTS=0 -DBENCH=0 -DDOCUM=off -DCMAKE_BUILD_TYPE=Release -DMULTI=PTHREAD
)

function(rbp_build_relic name)
    set(preset ${relic_SOURCE_DIR}/preset/${RBP_CURVE_${name}_PRESET}.sh)
    if (NOT EXISTS ${preset})
        message(FATAL_ERROR "RELIC has no preset ${RBP_CURVE_${name}_PRESET} for curve '${name}'.")
    endif ()
    set(binary_dir ${PROJECT_BINARY_DIR}/relic/${name})
    set(source_dir ${PROJECT_BINARY_DIR}/relic/${name}-source)
    set(stamp ${binary_dir}/rbp-configured.stamp)
    file(GLOB relic_configuration_inputs ${relic_SOURCE_DIR}/CMakeLists.txt ${relic_SOURCE_DIR}/cmake/*.cmake
            ${relic_SOURCE_DIR}/include/*.h ${relic_SOURCE_DIR}/include/*.in ${relic_SOURCE_DIR}/include/low/*.h)
    file(GLOB entries CONFIGURE_DEPENDS RELATIVE ${relic_SOURCE_DIR} ${relic_SOURCE_DIR}/* ${relic_SOURCE_DIR}/src/*)
    list(REMOVE_ITEM entries src src/relic_core.c)
    set(relic_configuration "${entries}")
    foreach (input IN LISTS preset relic_configuration_inputs)
        file(SHA256 ${input} digest)
        string(APPEND relic_configuration ${digest})
    endforeach ()
    set(options ${RBP_RELIC_OPTIONS} -DBN_PRECI=${RBP_CURVE_${name}_BN_PRECISION})
    list(JOIN RBP_SANITIZER_FLAGS " " sanitizer_flags)
    string(SHA256 fingerprint
            "${relic_configuration};${options};${RBP_RELIC_CORE_DIGEST};${CMAKE_C_COMPILER};${sanitizer_flags}")
    if (EXISTS ${stamp})
        file(READ ${stamp} previous)
    endif ()
    if (NOT previous STREQUAL fingerprint)
        file(REMOVE_RECURSE ${binary_dir})
        file(MAKE_DIRECTORY ${binary_dir})
        file(REMOVE_RECURSE ${source_dir})
        file(MAKE_DIRECTORY ${source_dir}/src)
        foreach (entry IN LISTS entries)
            file(CREATE_LINK ${relic_SOURCE_DIR}/${entry} ${source_dir}/${entry} SYMBOLIC)
        endforeach ()
        file(WRITE ${source_dir}/src/relic_core.c "${RBP_RELIC_PATCHED_CORE}")
        rbp_run("RELIC preset for ${name}"
                COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} sh ${preset} ${source_dir}
                WORKING_DIRECTORY ${binary_dir})
        load_cache(${binary_dir} READ_WITH_PREFIX preset_ CFLAGS)
        rbp_run("RELIC options for ${name}"
                COMMAND ${CMAKE_COMMAND} -DLABEL=${name} ${options} "-DCFLAGS=${preset_CFLAGS} ${sanitizer_flags}" .
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
