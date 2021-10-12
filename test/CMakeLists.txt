include_directories("${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/include"
  "${gtest_SOURCE_DIR}/include")

set(test_sources
  ${PROJECT_NAME}_test.cpp)


add_executable(${PROJECT_NAME}_tests ${test_sources})

target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME} gtest)

include(GoogleTest)

if (OPENGL_FOUND)
  gtest_discover_tests(${PROJECT_NAME}_tests)
else()
  message("Penumbra: OpenGL not found. Relevant unit tests will not be executed.")
endif()