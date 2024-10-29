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
                URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
endif()

# Add desired subdirectories.
add_subdirectory(src)
add_subdirectory(tests)

# Install targets and files.
install(TARGETS LibRBP
        EXPORT LibRBPTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RBP)

install(EXPORT LibRBPTargets
        FILE LibRBPTargets.cmake
        NAMESPACE LibRBP::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibRBP)

# Install the configuration file.
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LibRBPConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/LibRBPConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibRBP)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/LibRBPConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/LibRBPConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/LibRBPConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibRBP)

# Install the dependency file.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LibRBPDependencies.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/LibRBPDependencies.cmake"
        @ONLY)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/LibRBPDependencies.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibRBP)
