# Empty default flags
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")

add_library(penumbra_common_interface INTERFACE)

  #================#
  # Compiler flags #
  #================#

target_compile_options(penumbra_common_interface INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>: # Visual C++ (VS 2013)
    /nologo
    /EHsc
    $<$<CONFIG:Release>:
      /GS-    # Disable buffer overrun checks for performance in release mode
    >
    $<$<CONFIG:Debug>:
      /Ob0    # Disable inlining
      /RTCsu  # Runtime checks
      /Zi
    >
  >
  # GCC
  $<$<CXX_COMPILER_ID:GNU>:
    -pthread
    -pipe       # Faster compiler processing
    -std=c++11  # Enable C++11 features in g++
    -pedantic   # Turn on warnings about constructs/situations that may be non-portable or outside of the standard
    -Wall       # Turn on warnings
    -Wextra     # Turn on warnings
    -Wno-unknown-pragmas
    -Wno-unused-but-set-parameter   # Suppress unused-but-set warnings until more serious ones are addressed
    -Wno-unused-but-set-variable    # Suppress unused-but-set warnings until more serious ones are addressed
    -Wno-maybe-uninitialized
    $<$<CONFIG:Release>:
      -fno-stack-protector  # Produces debugging information specifically for gdb
    >
    $<$<CONFIG:Debug>:
      -ggdb
      -ffloat-store     # Improve debug run solution stability
      -fsignaling-nans  # Disable optimizations that may have concealed NaN behavior
      -D_GLIBCXX_DEBUG  # Standard container debug mode (bounds checking, ...)
    >
    # -finline-limit=2000 # More aggressive inlining   This is causing unit test failures on Ubuntu 14.04
    $<$<BOOL:UNIX>:
      -fPIC
    >
  >
  $<$<CXX_COMPILER_ID:Clang>:
    -pipe       # Faster compiler processing
    -std=c++11  # Enable C++11 features in g++
    -pedantic   # Turn on warnings about constructs/situations that may be non-portable or outside of the standard
    -Wall       # Turn on warnings
    -Wextra     # Turn on warnings
    -Wno-unknown-pragmas
    -Wno-invalid-source-encoding
    $<$<CONFIG:Release>:
      -fno-stack-protector  # Produces debugging information specifically for gdb
    >
    $<$<CONFIG:Debug>:
      -ggdb
      -ffloat-store     # Improve debug run solution stability
      -fsignaling-nans  # Disable optimizations that may have concealed NaN behavior
      -D_GLIBCXX_DEBUG  # Standard container debug mode (bounds checking, ...)
    >
  >

  $<$<BOOL:${WIN32}>: $<$<CXX_COMPILER_ID:Intel>:
      # Disabled Warnings: Enable some of these as more serious warnings are addressed
      #   161 Unrecognized pragma
      #   177 Variable declared but never referenced
      #   488 Template parameter not used ...
      #   809 Exception specification consistency warnings that fire in gtest code
      #   869 Parameter never referenced
      #  1786 Use of deprecated items
      #  2259 Non-pointer conversions may lose significant bits
      #  3280 Declaration hides variable
      # 10382 xHOST remark
      # 11074 Inlining inhibited
      # 11075 Inlining inhibited
      /nologo         # Skip banner text
      /Qstd=c++11     # Specify C++11 language
      /Qcxx-features  # Enables standard C++ features without disabling Microsoft extensions
      /Wall           # Enable "all" warnings
      /Qdiag-disable:161,177,488,809,869,1786,2259,3280,10382,11074,11075 # Disable warnings listed above
      /DNOMINMAX      # Avoid build errors due to STL/Windows min-max conflicts
      /DWIN32_LEAN_AND_MEAN # Excludes rarely used services and headers from compilation
      $<$<CONFIG:Release>: # ADDITIONAL RELEASE-MODE-SPECIFIC FLAGS
        /O3           # Agressive optimization
        /Qprec-div-   # Faster division
        /Qansi-alias  # Better optimization via strict aliasing rules
        /Qip          # Inter-procedural optimnization within a single file
        /Qinline-factor:225 # Aggressive inlining
        # /fp:fast=2  # Aggressive optimizations on floating-point data
      >
      $<$<CONFIG:Debug>:  # ADDITIONAL DEBUG-MODE-SPECIFIC FLAGS
        /fp:source      # Use source-specified floating point precision
        /Qtrapuv        # Initialize local variables to unusual values to help detect use uninitialized
        /check:stack,uninit # Enables runtime checking of the stack (buffer over and underruns; pointer verification) and uninitialized variables
        /Gs0            # Enable stack checking for all functions
        /GS             # Buffer overrun detection
        /Qfp-stack-check  # Tells the compiler to generate extra code after every function call to ensure fp stack is as expected
        /traceback      # Enables traceback on error
      >
    >
  >
  $<$<BOOL: UNIX>: $<$<CXX_COMPILER_ID:Intel>:
    # Disabled Warnings: Enable some of these as more serious warnings are addressed
    #   161 Unrecognized pragma
    #   177 Variable declared but never referenced
    #   488 Template parameter not used ...
    #   809 Exception specification consistency warnings that fire in gtest code
    #   869 Parameter never referenced
    #  1786 Use of deprecated items
    #  2259 Non-pointer conversions may lose significant bits
    #  3280 Declaration hides variable
    # 10382 xHOST remark
    # 11074 Inlining inhibited
    # 11075 Inlining inhibited

    # COMPILER FLAGS
    -std=c++11  # Specify C++11 language
    -Wall       # Enable "all" warnings
    -diag-disable:161,177,488,809,869,1786,2259,3280,10382,11074,11075 # Disable warnings listed above
    # Not apple
    
    $<$<CONFIG:Release>:    # ADDITIONAL RELEASE-MODE-SPECIFIC FLAGS
      -O3           # Agressive optimization
      # -Ofast # More aggressive optimizations (instead of -O3) (enables -no-prec-div and -fp-model fast=2)
      -no-prec-div  # Faster division (enabled by -Ofast)
      -ansi-alias   # Enables more aggressive optimizations on floating-point data
      -ip           # Enables inter-procedural optimnization within a single file
      -inline-factor=225 # Enables more aggressive inlining
    >
    $<$<CONFIG:Debug>:    # ADDITIONAL DEBUG-MODE-SPECIFIC FLAGS
      -strict-ansi      # Strict language conformance: Performance impact so limit to debug build
      -fp-model source  # Use source-specified floating point precision
      -ftrapuv          # Initialize local variables to unusual values to help detect use uninitialized
      -check=stack,uninit # Enables runtime checking of the stack (buffer over and underruns; pointer verification) and uninitialized variables
      -fstack-security-check # Buffer overrun detection
      -fp-stack-check   # Check the floating point stack after every function call
      -traceback        # Enables traceback on error

    >
  >
  >
)

target_link_options(penumbra_common_interface INTERFACE 
  $<$<CXX_COMPILER_ID:GNU>:
    -pthread
  >

)
