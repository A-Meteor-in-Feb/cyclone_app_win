# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("cyclone_v1.2")

# Include sub-projects.
add_subdirectory ("teleop_side")
