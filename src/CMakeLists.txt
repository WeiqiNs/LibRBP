# Collect all source files
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Create the library
add_library(RBP ${SOURCE_LIST})

# Create a target alias
add_library(RBP::RBP ALIAS RBP)

# Set include directories for consumers
target_include_directories(RBP PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(RBP PUBLIC ${RLC_LIBRARY})

# Add this line to ensure RELIC's include directories are correctly passed
target_include_directories(RBP PUBLIC ${RLC_INCLUDE_DIRS})

# Set safe PATHs (optional for Linux/macOS)
set_target_properties(RBP PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH_USE_LINK_PATH TRUE
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
)
