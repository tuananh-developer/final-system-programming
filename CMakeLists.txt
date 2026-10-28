cmake_minimum_required(VERSION 3.10.0)

project(final_system_programming
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(final_system_programming
    src/main.c
    src/file_copy.c
    src/directory_copy.c
)

target_include_directories(final_system_programming
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(final_system_programming
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)