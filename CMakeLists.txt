cmake_minimum_required(VERSION 3.7)
project(penumbra)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include(cmake/compiler-flags.cmake)

# GLAD library
add_subdirectory(vendor/glad)

# GLFW libaray
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if (APPLE)
  set(GLFW_USE_MENUBAR OFF CACHE BOOL "" FORCE)
endif()

add_subdirectory(vendor/glfw-3.2.1)
add_subdirectory(vendor/libtess2)

add_subdirectory(src)
add_subdirectory(examples)
