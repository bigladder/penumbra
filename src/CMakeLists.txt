# Main library
file(GLOB_RECURSE sources "${PROJECT_SOURCE_DIR}/src/[a-zA-Z]*.cpp")
file(GLOB_RECURSE public_headers "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/[a-zA-Z]*.h")
file(GLOB_RECURSE private_headers "${PROJECT_SOURCE_DIR}/src/[a-zA-Z]*.h")

set(library_sources
  ${sources}
  ${public_headers}
  ${private_headers}
)

option( ${PROJECT_NAME}_STATIC_LIB "Make ${PROJECT_NAME} a static library" ON )

if (${PROJECT_NAME}_STATIC_LIB)
  add_library(${PROJECT_NAME} STATIC ${library_sources})
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-D${PROJECT_NAME}_STATIC_DEFINE")
else()
  set(CMAKE_MACOSX_RPATH 1)
  add_library(${PROJECT_NAME} SHARED ${library_sources})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
  SYSTEM ${PROJECT_SOURCE_DIR}/vendor/glad/include
  SYSTEM ${PROJECT_SOURCE_DIR}/vendor/glfw/include
  SYSTEM ${PROJECT_SOURCE_DIR}/vendor/glfw/deps
  SYSTEM ${PROJECT_SOURCE_DIR}/vendor/libtess2/Include
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_link_libraries(tess2 PRIVATE penumbra_common_interface)
target_link_libraries(glad PRIVATE penumbra_common_interface)
target_link_libraries(glfw PRIVATE penumbra_common_interface)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw tess2 penumbra_common_interface)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if (${PROJECT_NAME}_COVERAGE)
  add_coverage(${PROJECT_NAME})
endif()
