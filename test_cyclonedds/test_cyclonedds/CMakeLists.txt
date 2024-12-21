cmake_minimum_required(VERSION 3.8)
project(test_cyclonedds LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED)
endif()

idlcxx_generate(TARGET helloworlddata FILES HelloWorldData.idl WARNINGS no-implicit-extensibility)

# include header files' path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# add source files
set(SOURCES test_cyclonedds.cpp HelloWorldData.cpp shutdownsignal.cpp)

add_executable(helloworld ${SOURCES} "shutdownsignal.hpp")

# Link both executables to idl data type library and ddscxx.
target_link_libraries(helloworld PRIVATE CycloneDDS-CXX::ddscxx helloworlddata)

# link libraries about LogitecSteeringWheelLib
target_include_directories(helloworld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Disable the static analyzer in GCC to avoid crashing the GNU C++ compiler
# on Azure Pipelines
if(DEFINED ENV{SYSTEM_TEAMFOUNDATIONSERVERURI})
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND ANALYZER STREQUAL "on")
    target_compile_options(helloworld PRIVATE -fno-analyzer)
  endif()
endif()

set_property(TARGET helloworld PROPERTY CXX_STANDARD 17)
