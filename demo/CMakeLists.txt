cmake_minimum_required(VERSION 3.20)
project(LibRBPDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the RBP library.
find_package(LibRBP REQUIRED)

# Create an executable.
add_executable(demo demo.cpp)

# Link the executable with the library.
target_link_libraries(demo PUBLIC LibRBP::LibRBP)
