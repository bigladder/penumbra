# '*' indicates CMake default option
# '+' indicates default compiler behavior

# Remove unwanted CMake defaults from global flags
if (MSVC)
  # See https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Platform/Windows-MSVC.cmake
  set(CMAKE_CXX_FLAGS
      /EHsc         #*Specifies the model of exception handling (sc options).
      /DWIN32       #*Windows Platform (regardless of architecture)
      /D_WINDOWS    #*
      )
  set(CMAKE_CXX_FLAGS_RELEASE
      /O2           #*Creates fast code (Og+Oi+Ot+Oy+Ob2+GF+Gy).
      # /Ob2        #*Controls inline expansion (level 2). (part of O2)
      /DNDEBUG      #*Enables or disables compilation of assertions
      )
  set(CMAKE_CXX_FLAGS_DEBUG
      /Ob0          #*Controls inline expansion (level 0 -- disabled).
      /Od           #*Disables optimization.
      /Zi           #*Generates complete debugging information.
      /RTC1         #*Enables run-time error checking.
      )
else () # GCC or Clang or AppleClang
  # See https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/GNU.cmake
  set(CMAKE_CXX_FLAGS "")
  set(CMAKE_CXX_FLAGS_RELEASE
      -O3           #*Maximum optimization (see https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#Optimize-Options).
      -DNDEBUG      #*Enables or disables compilation of assertions
      )
  set(CMAKE_CXX_FLAGS_DEBUG
      -g            #*Produce debugging information in the operating system’s native format.
      )
endif()

set(test_sources
  ${PROJECT_NAME}_test.cpp)


add_executable(${PROJECT_NAME}_tests ${test_sources})


target_include_directories(${PROJECT_NAME}_tests PRIVATE
  ${gtest_SOURCE_DIR}/include
)
target_link_libraries(gtest PRIVATE penumbra_common_interface)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME} gtest penumbra_common_interface)

include(GoogleTest)

if (OPENGL_FOUND)
  gtest_discover_tests(${PROJECT_NAME}_tests)
else()
  message("Penumbra: OpenGL not found. Relevant unit tests will not be executed.")
endif()