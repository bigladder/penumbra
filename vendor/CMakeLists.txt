# GLAD
add_subdirectory(glad)

set(${PROJECT_NAME}_submodules
  glfw
  googletest
  libtess2
)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  # Initialize submodules
  foreach(submodule ${${PROJECT_NAME}_submodules})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${submodule}/.git")
      message(STATUS "Initialize ${submodule} submodule")
      execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive "${CMAKE_CURRENT_SOURCE_DIR}/${submodule}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT)
      if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive ${CMAKE_CURRENT_SOURCE_DIR}/${submodule} failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
      endif()
    endif()
  endforeach()
endif()

# GLFW library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if (APPLE)
  set(GLFW_USE_MENUBAR OFF CACHE BOOL "" FORCE)
endif()

add_subdirectory(glfw)

# tess2 library
file(GLOB libtess2_sources "${CMAKE_CURRENT_SOURCE_DIR}/libtess2/Source/[a-zA-Z]*.c")

if (UNIX)
  ADD_DEFINITIONS("-fPIC")
endif()

add_library(tess2 ${libtess2_sources})

target_include_directories(tess2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libtess2/Include)
target_include_directories(tess2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libtess2/Source)


# googletest library
if (BUILD_PENUMBRA_TESTING AND NOT TARGET gtest)

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(BUILD_GTEST ON CACHE BOOL "" FORCE)

  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/googletest ${googletest_BINARY_DIR})

endif()
