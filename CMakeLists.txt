cmake_minimum_required(VERSION 3.25)

project(ADAN VERSION 1.0.0 LANGUAGES CXX)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "This project requires GCC. "
        "Detected: ${CMAKE_CXX_COMPILER_ID}. "
        "Re-run CMake with -DCMAKE_CXX_COMPILER=g++")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)          # -std=c++23, not -std=gnu++23

file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh"
)

file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

set(PROJECT_INCLUDE_DIRS "")

foreach(_header IN LISTS PROJECT_HEADERS)
    get_filename_component(_dir "${_header}" DIRECTORY)
    list(APPEND PROJECT_INCLUDE_DIRS "${_dir}")
endforeach()

list(REMOVE_DUPLICATES PROJECT_INCLUDE_DIRS)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    $<$<CONFIG:Debug>:   -g -O0 -fsanitize=address,undefined>
    $<$<CONFIG:Release>: -O3 -DNDEBUG -march=native>
)

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>: -fsanitize=address,undefined>
)