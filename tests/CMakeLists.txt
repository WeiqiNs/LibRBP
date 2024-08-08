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

# -----------------------------------------------------
add_executable(test_gp test_gp.cpp)
target_link_libraries(test_gp PRIVATE BP_LIB)
add_test(NAME test_gp COMMAND test_gp)
# -----------------------------------------------------

# -----------------------------------------------------
add_executable(test_group test_group.cpp)
target_link_libraries(test_group PRIVATE BP_LIB)
add_test(NAME test_group COMMAND test_group)
# -----------------------------------------------------

# -----------------------------------------------------
add_executable(test_bp test_bp.cpp)
target_link_libraries(test_bp PRIVATE BP_LIB)
add_test(NAME test_bp COMMAND test_bp)
# -----------------------------------------------------