project(GLAD LANGUAGES C)

add_library(glad src/glad.c)

target_include_directories(glad PUBLIC include)

if (UNIX)
    target_compile_options(glad PUBLIC "-fPIC")
endif()

