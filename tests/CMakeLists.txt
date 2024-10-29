# -----------------------------------------------------
# Include google test here and enable testing.
include(GoogleTest)
# Set alias for google test.
set(GTEST_LIB gtest gtest_main)
# -----------------------------------------------------
add_executable(test_fp test_fp.cpp)
target_link_libraries(test_fp RBP ${GTEST_LIB})
gtest_discover_tests(test_fp)
# -----------------------------------------------------
add_executable(test_field test_field.cpp)
target_link_libraries(test_field RBP ${GTEST_LIB})
gtest_discover_tests(test_field)
# -----------------------------------------------------
add_executable(test_gp test_gp.cpp)
target_link_libraries(test_gp RBP ${GTEST_LIB})
gtest_discover_tests(test_gp)
# -----------------------------------------------------
add_executable(test_group test_group.cpp)
target_link_libraries(test_group RBP ${GTEST_LIB})
gtest_discover_tests(test_group)
# -----------------------------------------------------
add_executable(test_bp test_bp.cpp)
target_link_libraries(test_bp RBP ${GTEST_LIB})
gtest_discover_tests(test_bp)
# -----------------------------------------------------
