cmake_minimum_required(VERSION 3.10)

project(lib_test C)

option(BUILD_STATIC "Build static library instead of shared" ON)

set(LIBRARY_SRC lib.c)
include_directories(".")

if(BUILD_STATIC)
    message(STATUS "Building STATIC library")
    add_library(funcs STATIC ${LIBRARY_SRC})
else()
    message(STATUS "Building SHARED library")
    add_library(funcs SHARED ${LIBRARY_SRC})
endif()

add_executable(main main.c)

target_link_libraries(main funcs)

