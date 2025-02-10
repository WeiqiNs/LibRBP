# Set cmake version.
cmake_minimum_required(VERSION 3.20)

# Project name and language.
project(
        LibRBP
        VERSION 0.1
        LANGUAGES CXX
        DESCRIPTION "This library implements a bilinear pairing group using the RELIC library."
)

# Set language standards.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include desired modules.
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Enable the testing.
enable_testing()

# Find the relic library.
find_package(RELIC REQUIRED)

# Find the google test library for testing.
find_package(GTest)
if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/5ed21863955149a5a877a53d7d5045b6919090ed.zip
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
endif()

# Add desired subdirectories.
add_subdirectory(src)
add_subdirectory(test)

# Install targets and files.
install(TARGETS RBP
        EXPORT RBPTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RBP)

install(EXPORT RBPTargets
        FILE RBPTargets.cmake
        NAMESPACE RBP::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)

# Install the configuration file.
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RBPConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/RBPConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)

# Install the dependency file.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RBPDependencies.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/RBPDependencies.cmake"
        @ONLY)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/RBPDependencies.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)
