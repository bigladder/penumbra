cmake_minimum_required(VERSION 3.7)
project(penumbra)

include(FindOpenGL)


# GLAD library
add_subdirectory(vendor/glad)

# GLFW libaray
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/glfw-3.2.1)

# Main library
file(GLOB sources "src/[a-zA-Z]*.cpp")
file(GLOB_RECURSE public_headers "include/penumbra/[a-zA-Z]*.h")
file(GLOB private_headers "src/[a-zA-Z]*.h")

set(library_sources
  ${sources}
  ${public_headers}
  ${private_headers}
)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/vendor/glfw-3.2.1/inlude)
include_directories(${PROJECT_SOURCE_DIR}/vendor/glfw-3.2.1/deps)

add_library(penumbra ${library_sources})

target_link_libraries(penumbra glad glfw)

add_subdirectory(examples)
