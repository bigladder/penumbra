# Set CMAKE flags for executable
SET_CXX_FLAGS()

set(test_sources
  ${PROJECT_NAME}_test.cpp)


add_executable(${PROJECT_NAME}_tests ${test_sources})


target_include_directories(${PROJECT_NAME}_tests PRIVATE
  ${gtest_SOURCE_DIR}/include
)
target_link_libraries(gtest PRIVATE penumbra_common_interface)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME} gtest penumbra_common_interface)
target_compile_features(${PROJECT_NAME}_tests PRIVATE cxx_std_17)

include(GoogleTest)

if (OPENGL_FOUND)
  gtest_discover_tests(${PROJECT_NAME}_tests)
else()
  message("Penumbra: OpenGL not found. Relevant unit tests will not be executed.")
endif()