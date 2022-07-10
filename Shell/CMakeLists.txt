cmake_minimum_required(VERSION 3.16)
project(shell)


set(SOURCE 
    src/main.c
)

add_library(sheller
    SHARED
        src/shell.c
)

add_compile_options(-O3 -w -Wimplicit-function-declaration)

target_include_directories(sheller
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
)

add_executable(shell ${SOURCE})

target_link_libraries(shell
    PRIVATE
        sheller
)
