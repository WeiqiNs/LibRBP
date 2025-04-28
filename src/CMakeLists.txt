# Collect all source files.
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Create the library.
add_library(RBP ${SOURCE_LIST})

# Create a target alias.
add_library(RBP::RBP ALIAS RBP)

# Set include directories for consumers.
target_include_directories(RBP PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Correctly include the relic library.
target_link_libraries(RBP PUBLIC ${RLC_LIBRARY})
target_include_directories(RBP PUBLIC ${RLC_INCLUDE_DIRS})

# Set install paths.
set_target_properties(RBP PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH_USE_LINK_PATH TRUE
)
