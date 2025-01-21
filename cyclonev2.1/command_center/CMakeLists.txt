cmake_minimum_required(VERSION 3.8)
project(command_center LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED)
endif()

idlcxx_generate(TARGET controldata FILES ControlData.idl WARNINGS no-implicit-extensibility)

# include header files' path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# add source files
set(SOURCES entry.cpp center_subscriber.cpp ControlData.cpp shutdownsignal.cpp)


# add executable 
add_executable(command_center ${SOURCES} "shutdownsignal.hpp" "TimeStampLogger.h")

# Link both executables to idl data type library and ddscxx.
target_link_libraries(command_center PRIVATE CycloneDDS-CXX::ddscxx controldata)

# link libraries about LogitecSteeringWheelLib
target_include_directories(command_center PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Disable the static analyzer in GCC to avoid crashing the GNU C++ compiler
# on Azure Pipelines
if(DEFINED ENV{SYSTEM_TEAMFOUNDATIONSERVERURI})
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND ANALYZER STREQUAL "on")
    target_compile_options(command_center PRIVATE -fno-analyzer)
  endif()
endif()

set_property(TARGET command_center PROPERTY CXX_STANDARD 17)