# Enable Google Test framework.
include(GoogleTest)

# List of test source files.
set(TEST_SOURCES
        test_fp.cpp
        test_field.cpp
        test_gp.cpp
        test_group.cpp
        test_bp.cpp
)

# Create test executables and register them.
foreach(TEST_SRC IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME}
            PRIVATE
            RBP
            GTest::gtest
            GTest::gtest_main
    )
    gtest_discover_tests(${TEST_NAME})

    # Set folder for IDEs.
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Tests")
endforeach()