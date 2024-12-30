# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("cyclonev2.1")

# Include sub-projects.
add_subdirectory ("teleop_side")
add_subdirectory ("vehicle_side")
#add_subdirectory ("command_center")
