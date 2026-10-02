find_package(GTest QUIET)
if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif ()
include(GoogleTest)

set(fixture_includes "")
set(qualified_tags "")
set(point_types "")
set(curve_libraries "")
foreach (curve IN LISTS RBP_CURVES)
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/${curve}.hpp)
        message(FATAL_ERROR "Curve '${curve}' has no test fixture: add test/fixtures/${curve}.hpp.")
    endif ()
    string(APPEND fixture_includes "#include <fixtures/${curve}.hpp>\n")
    set(tag rbp::${RBP_CURVE_${curve}_TAG})
    list(APPEND qualified_tags ${tag})
    list(APPEND point_types "rbp::G1<${tag}>" "rbp::G2<${tag}>")
    list(APPEND curve_libraries $<TARGET_FILE:RBP_${curve}>)
endforeach ()
list(JOIN qualified_tags ", " tag_list)
list(JOIN point_types ", " point_list)
file(CONFIGURE OUTPUT ${RBP_GENERATED_DIR}/test/curves.hpp CONTENT "#ifndef RBP_TEST_CURVES_HPP
#define RBP_TEST_CURVES_HPP

#include <tuple>
#include <gtest/gtest.h>
#include <rbp/rbp.hpp>
${fixture_includes}
using CurveTuple = std::tuple<${tag_list}>;
using Curves = ::testing::Types<${tag_list}>;
using Points = ::testing::Types<${point_list}>;

#endif
")

set(TEST_SOURCES
        test_runtime.cpp
        test_zp.cpp
        test_point.cpp
        test_gt.cpp
        test_pairing.cpp
        test_linalg.cpp
        test_io.cpp
)

foreach (source IN LISTS TEST_SOURCES)
    get_filename_component(name ${source} NAME_WE)
    add_executable(${name} ${source})
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RBP_GENERATED_DIR}/test)
    target_link_libraries(${name} PRIVATE RBP::RBP GTest::gtest_main)
    gtest_discover_tests(${name})
endforeach ()

add_test(NAME exported_symbols COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/check_exports.sh ${curve_libraries})

foreach (curve IN LISTS RBP_CURVES)
    add_test(NAME soname_carries_layout_${curve}
            COMMAND sh -c "readelf -d \"$1\" | grep -q \"SONAME.*\\[libRBP_${curve}\\.so\\.${PROJECT_VERSION_MAJOR}\\.$2\\]\""
            sh $<TARGET_FILE:RBP_${curve}> ${RBP_${curve}_LAYOUT})
endforeach ()
