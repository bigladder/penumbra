# Empty default flags
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")

add_library(penumbra_common_interface INTERFACE)
set(use_msvc "$<CXX_COMPILER_ID:MSVC>")
set(use_gcc "$<CXX_COMPILER_ID:GNU>")
set(use_clang "$<CXX_COMPILER_ID:CLANG>")
set(use_win32_intel "$<AND:$<BOOL:${WIN32}>,$<CXX_COMPILER_ID:Intel>>")
set(use_unix_intel "$<AND:$<BOOL:${UNIX}>,$<CXX_COMPILER_ID:Intel>>")
set(release_config "$<CONFIG:Release>")
set(debug_config "$<CONFIG:Debug>")
set(use_msvc_release "$<AND:${use_msvc},${release_config}>")
set(use_msvc_debug "$<AND:${use_msvc},${debug_config}>")
set(use_gcc_release "$<AND:${use_gcc},${release_config}>")
set(use_gcc_debug "$<AND:${use_gcc},${debug_config}>")
set(use_clang_release "$<AND:${use_clang},${release_config}>")
set(use_clang_debug "$<AND:${use_clang},${debug_config}>")
set(use_win32_intel_release "$<AND:${use_win32_intel},${release_config}>")
set(use_win32_intel_debug "$<AND:${use_win32_intel},${debug_config}>")
set(use_unix_intel_release "$<AND:${use_unix_intel},${release_config}>")
set(use_unix_intel_debug "$<AND:${use_unix_intel},${debug_config}>")

  #================#
  #   MSVC flags   #
  #================#

set(msvc_common
  /nologo
  /EHsc
  /W4
  /WX
)

set(msvc_release
  "${msvc_common}"
  /GS-    # Disable buffer overrun checks for performance in release mode
  /MT     # Causes the application to use the multithread, static version of the run-time library, using LIBCMT.lib.
)

set(msvc_debug
  "${msvc_common}"
  /Zi     #
  /Ob0    # Disable inlining
  /Od     # Turns off all optimizations in the program and speeds compilation
  /RTC1   # Runtime checks
  /MTd    # Debug multithreaded executable file using LIBCMTD.lib.
)

  #================#
  #    GCC flags   #
  #================#

set(gcc_common
  -pthread
  -pipe       # Faster compiler processing
  # Adds flag only to C++
  "$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-pedantic>" # Turn on warnings about constructs/situations that may be non-portable or outside of the standard
  -Wall       # Turn on warnings
  -Wextra     # Turn on warnings
  -Werror     # Turn warnings into errors
  "$<$<BOOL:UNIX>:-fPIC>"
)
set(gcc_release
  "${gcc_common}"
  -fno-stack-protector  # Produces debugging information specifically for gdb
)
set(gcc_debug
  "${gcc_common}"
  -ggdb
  -ffloat-store     # Improve debug run solution stability
  -fsignaling-nans  # Disable optimizations that may have concealed NaN behavior
  -D_GLIBCXX_DEBUG  # Standard container debug mode (bounds checking, ...)
)

  #================#
  #  Clang flags   #
  #================#

set(clang_common
  -pipe       # Faster compiler processing
  # Adds flag only to C++
  "$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-pedantic>" # Turn on warnings about constructs/situations that may be non-portable or outside of the standard
  -Wall       # Turn on warnings
  -Wextra     # Turn on warnings
  -Werror     # Turn warnings into errors
)
set(clang_release
  "${clang_common}"
  -fno-stack-protector  # Produces debugging information specifically for gdb
)
set(clang_debug
  "${clang_common}"
  -ggdb
  -ffloat-store     # Improve debug run solution stability
  -fsignaling-nans  # Disable optimizations that may have concealed NaN behavior
  -D_GLIBCXX_DEBUG  # Standard container debug mode (bounds checking, ...)
)

  #===================#
  # Win32 Intel flags #
  #===================#

set(win32_intel_common
  /nologo         # Skip banner text
  /Qcxx-features  # Enables standard C++ features without disabling Microsoft extensions
  /Wall           # Enable "all" warnings
  /DNOMINMAX      # Avoid build errors due to STL/Windows min-max conflicts
  /DWIN32_LEAN_AND_MEAN # Excludes rarely used services and headers from compilation
)
set(win32_intel_release
  "${win32_intel_common}"
  /O3           # Agressive optimization
  /Qprec-div-   # Faster division
  /Qansi-alias  # Better optimization via strict aliasing rules
  /Qip          # Inter-procedural optimnization within a single file
  /Qinline-factor:225 # Aggressive inlining
  # /fp:fast=2  # Aggressive optimizations on floating-point data
)
set(win32_intel_debug
  "${win32_intel_common}"
  /fp:source      # Use source-specified floating point precision
  /Qtrapuv        # Initialize local variables to unusual values to help detect use uninitialized
  /check:stack,uninit # Enables runtime checking of the stack (buffer over and underruns; pointer verification) and uninitialized variables
  /Gs0            # Enable stack checking for all functions
  /GS             # Buffer overrun detection
  /Qfp-stack-check  # Tells the compiler to generate extra code after every function call to ensure fp stack is as expected
  /traceback      # Enables traceback on error
)

  #==================#
  # Unix Intel flags #
  #==================#

set(unix_intel_common
  # COMPILER FLAGS
  -Wall       # Enable "all" warnings
  # Not apple
  "$<$<NOT:${APPLE}>:-pthreat"
>
)
set(unix_intel_release
  "${unix_intel_common}"
  -O3           # Agressive optimization
  # -Ofast # More aggressive optimizations (instead of -O3) (enables -no-prec-div and -fp-model fast=2)
  -no-prec-div  # Faster division (enabled by -Ofast)
  -ansi-alias   # Enables more aggressive optimizations on floating-point data
  -ip           # Enables inter-procedural optimnization within a single file
  -inline-factor=225 # Enables more aggressive inlining
)
set(unix_intel_debug
  "${unix_intel_common}"
  -strict-ansi      # Strict language conformance: Performance impact so limit to debug build
  -fp-model source  # Use source-specified floating point precision
  -ftrapuv          # Initialize local variables to unusual values to help detect use uninitialized
  -check=stack,uninit # Enables runtime checking of the stack (buffer over and underruns; pointer verification) and uninitialized variables
  -fstack-security-check # Buffer overrun detection
  -fp-stack-check   # Check the floating point stack after every function call
  -traceback        # Enables traceback on error
)

  #================#
  # Compiler flags #
  #================#

  target_compile_options(penumbra_common_interface INTERFACE
    "$<${use_msvc_release}:${msvc_release}>"
    "$<${use_msvc_debug}:${msvc_debug}>"
    "$<${use_gcc_release}:${gcc_release}>"
    "$<${use_gcc_debug}:${gcc_debug}>"
    "$<${use_gcc_release}:${gcc_release}>"
    "$<${use_gcc_debug}:${gcc_debug}>"
    "$<${use_clang_release}:${clang_release}>"
    "$<${use_clang_debug}:${clang_debug}>"
    "$<${use_win32_intel_release}:${win32_intel_release}>"
    "$<${use_win32_intel_debug}:${win32_intel_debug}>"
    "$<${use_unix_intel_release}:${unix_intel_release}>"
    "$<${use_unix_intel_debug}:${unix_intel_debug}>"
  )

  #================#
  #  Linker flags  #
  #================#

target_link_options(penumbra_common_interface INTERFACE 
  "$<$<CXX_COMPILER_ID:GNU>:-pthread>"
)
#file(GENERATE OUTPUT hola.txt CONTENT "$<${CXX_COMPILER_ID}:MSVC>")