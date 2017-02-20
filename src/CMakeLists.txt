# Main library
file(GLOB sources "${penumbra_SOURCE_DIR}/src/[a-zA-Z]*.cpp")
file(GLOB_RECURSE public_headers "${penumbra_SOURCE_DIR}/include/penumbra/[a-zA-Z]*.h")
file(GLOB private_headers "${penumbra_SOURCE_DIR}/src/[a-zA-Z]*.h")

set(library_sources
  ${sources}
  ${public_headers}
  ${private_headers}
)

include_directories(${penumbra_SOURCE_DIR}/include)
include_directories(SYSTEM ${penumbra_SOURCE_DIR}/vendor/glad/include)
include_directories(SYSTEM ${penumbra_SOURCE_DIR}/vendor/glfw-3.2.1/include)
include_directories(SYSTEM ${penumbra_SOURCE_DIR}/vendor/glfw-3.2.1/deps)

add_library(penumbra ${library_sources})

target_link_libraries(penumbra glad glfw)
