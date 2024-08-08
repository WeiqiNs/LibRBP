# Get all header and source files.
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${RBP_SOURCE_DIR}/src/*.cpp")

# Make an automatic library - will be static or dynamic based on user setting.
add_library(BP_LIB ${SOURCE_LIST})

# We need this directory, and users of our library will need it too.
target_include_directories(BP_LIB PUBLIC ${RBP_SOURCE_DIR}/include)

# This depends on the relic library we found.
target_link_libraries(BP_LIB PRIVATE ${RELIC_LIB})