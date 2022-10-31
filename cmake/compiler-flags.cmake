# Empty default flags
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")


add_library(penumbra_common_interface INTERFACE)

  #==================#
  # Compiler options #
  #==================#

target_compile_options(penumbra_common_interface INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>: # Visual C++ (VS 2013)
    /GR
    /nologo
    /EHsc
    /W4
    /WX
    $<$<CONFIG:Release>:
      /GS-    # Disable buffer overrun checks for performance in release mode
      /O2
      /Ob2
      /DNDEBUG
    >
    $<$<CONFIG:Debug>:
      /Zi     #
      /Ob0    # Disable inlining
      /Od     # Turns off all optimizations in the program and speeds compilation
      /RTC1   # Runtime checks
    >
  >
  # GCC And Clang
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
    -pthread
    -pipe       # Faster compiler processing
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>: # Adds flag only to C++
      -pedantic   # Turn on warnings about constructs/situations that may be non-portable or outside of the standard
    >
    -Wall       # Turn on warnings
    -Wextra     # Turn on warnings
    -Werror     # Turn warnings into errors
    $<$<CONFIG:Release>:
      -fno-stack-protector  # Produces debugging information specifically for gdb
      -O3
    >
    $<$<CXX_COMPILER_ID:GNU>:
      $<$<CONFIG:Debug>:
        -ggdb
        -ffloat-store     # Improve debug run solution stability
        -fsignaling-nans  # Disable optimizations that may have concealed NaN behavior
      >
    >
    # -finline-limit=2000 # More aggressive inlining   This is causing unit test failures on Ubuntu 14.04
    $<$<BOOL:UNIX>:
      -fPIC
    >
  >
)

#======================#
# Compiler definitions #
#======================#

target_compile_definitions(penumbra_common_interface INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>: # Visual C++ (VS 2013)
    WIN32
    _WINDOWS
  >
  $<$<CONFIG:Release>:
    NDEBUG
  >
  # GCC
  $<$<CXX_COMPILER_ID:GNU>:
    $<$<CONFIG:Debug>:
      _GLIBCXX_DEBUG  # Standard container debug mode (bounds checking, ...)
    >
  >
)

#==================#
#  Linker options  #
#==================#

target_link_options(penumbra_common_interface INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:
    -pthread
  >

)
