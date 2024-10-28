# Set cmake version.
cmake_minimum_required(VERSION 3.20)

# Project name and language.
project(
        RBP
        VERSION 0.1
        LANGUAGES CXX
        DESCRIPTION "This library implements a bilinear pairing group using the RELIC library."
)

# Set standards and include tests.
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 17)
    include(CTest)
endif ()

# Find the relic library.
find_package(RELIC REQUIRED)
find_library(RELIC_LIB relic)

# Add the GNUInstallDirs module.
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Add desired subdirectories.
add_subdirectory(src)
add_subdirectory(tests)

# Install targets and files.
install(TARGETS BP_LIB
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

# Add the RBPDependencies.cmake file.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RBPDependencies.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/RBPDependencies.cmake"
        @ONLY)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/RBPDependencies.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)
