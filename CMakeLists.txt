# Set cmake version.
cmake_minimum_required(VERSION 3.20)

# Project name and language.
project(
        LibRBP
        VERSION 0.1
        DESCRIPTION "This library implements a bilinear pairing group using the RELIC library."
        LANGUAGES CXX
)

# Set C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include useful CMake modules.
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Enable unit testing.
enable_testing()

# Define install directory for CMake package files.
set(RBP_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/RBP")

# Find RELIC dependency.
find_package(RELIC REQUIRED)

# Find or fetch GoogleTest for unit testing.
find_package(GTest)
if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(googletest)
endif ()

# Add source and test directories.
add_subdirectory(src)
add_subdirectory(test)

# Install the RBP target.
install(TARGETS RBP
        EXPORT RBPTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers.
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RBP)

# Install and export CMake targets.
install(EXPORT RBPTargets
        FILE RBPTargets.cmake
        NAMESPACE RBP::
        DESTINATION ${RBP_CMAKE_INSTALL_DIR}
)

# Generate and install package config files.
configure_package_config_file(cmake/RBPConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfig.cmake"
        INSTALL_DESTINATION ${RBP_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfigVersion.cmake"
        DESTINATION ${RBP_CMAKE_INSTALL_DIR}
)
