# Set CMAKE flags for executable
SET_CXX_FLAGS()

if (WIN32)
    set(ICON glfw.rc)
elseif (APPLE)
    set(ICON glfw.icns)
    set_source_files_properties(glfw.icns PROPERTIES
        MAXOSX_PACKAGE_LOCATION "Resources")
endif()

# Sets the MT flag for Release or Debug config
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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
