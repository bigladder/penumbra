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

# Convert lists to space-separated strings
list(JOIN CMAKE_CXX_FLAGS " " CMAKE_CXX_FLAGS)
list(JOIN CMAKE_CXX_FLAGS_RELEASE " " CMAKE_CXX_FLAGS_RELEASE)
list(JOIN CMAKE_CXX_FLAGS_DEBUG " " CMAKE_CXX_FLAGS_DEBUG)

if (WIN32)
    set(ICON glfw.rc)
elseif (APPLE)
    set(ICON glfw.icns)
    set_source_files_properties(glfw.icns PROPERTIES
        MAXOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(awning WIN32 MACOSX_BUNDLE awning.cpp ${ICON})
target_link_libraries(awning PRIVATE ${PROJECT_NAME} penumbra_common_interface)
target_compile_features(awning PRIVATE cxx_std_17)
target_compile_definitions(awning PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)

set(EXE_BINARIES awning)

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${EXE_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
    set_target_properties(${EXE_BINARIES} PROPERTIES
                          LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    set_target_properties(${EXE_BINARIES} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")
endif()

if (APPLE)
    set_target_properties(awning PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Awning")

    set_target_properties(${EXE_BINARIES} PROPERTIES
                          RESOURCE glfw.icns
                          MACOSX_BUNDLE_SHORT_VERSION_STRING "0"
                          MACOSX_BUNDLE_LONG_VERSION_STRING "0"
                          MACOSX_BUNDLE_ICON_FILE glfw.icns )
endif()
