if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32)
    set(ICON glfw.rc)
elseif (APPLE)
    set(ICON glfw.icns)
    set_source_files_properties(glfw.icns PROPERTIES
        MAXOSX_PACKAGE_LOCATION "Resources")
endif()

link_libraries(penumbra)

add_executable(awning WIN32 awning.cpp ${ICON})

set(EXE_BINARIES awning)

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${EXE_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()
