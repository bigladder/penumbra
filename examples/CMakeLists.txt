include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/examples")
include_directories("${PROJECT_SOURCE_DIR}/build/examples")
include_directories("${PROJECT_SOURCE_DIR}/build/src")

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

message(STATUS "Dynamic library")
add_library(fakestate SHARED fakestate.h fakestate.cpp fakestate_export.h)

target_link_libraries(fakestate ${PROJECT_NAME})
target_compile_features(fakestate PRIVATE cxx_std_17)
include(GenerateExportHeader)
generate_export_header(fakestate)

add_executable(awning WIN32 MACOSX_BUNDLE awning.cpp ${ICON})

target_link_libraries(awning fakestate)

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
