# -----------------------------------------------------
add_executable(test_fp test_fp.cpp)
target_link_libraries(test_fp PRIVATE BP_LIB)
add_test(NAME test_fp COMMAND test_fp)
# -----------------------------------------------------

# -----------------------------------------------------
add_executable(test_field test_field.cpp)
target_link_libraries(test_field PRIVATE BP_LIB)
add_test(NAME test_field COMMAND test_field)
# -----------------------------------------------------