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
find_library(RELIC_LIB relic)

# Add desired subdirectories.
add_subdirectory(src)
add_subdirectory(tests)
