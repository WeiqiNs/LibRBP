# Get all header and source files.
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${RBP_SOURCE_DIR}/src/*.cpp")

# Make an automatic library - will be static or dynamic based on user setting.
add_library(BP_LIB STATIC ${SOURCE_LIST})

# Add this line:
add_library(RBP::BP_LIB ALIAS BP_LIB)

# We need this directory, and users of our library will need it too.
target_include_directories(BP_LIB PUBLIC
    $<BUILD_INTERFACE:${RBP_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(BP_LIB PUBLIC ${RELIC_LIB} m)

# Add this line to ensure RELIC's include directories are correctly passed
target_include_directories(BP_LIB PUBLIC ${RELIC_INCLUDE_DIRS})

# Add RPATH settings
set_target_properties(BP_LIB PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE)
