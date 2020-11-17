cmake_minimum_required(VERSION 3.10)
project(A3_ptrs C)

set(CMAKE_C_STANDARD 99)

set(sma sma)

add_library(
        ${sma} STATIC
        sma.h sma.c
)

add_executable(a3_test a3_test.c)
target_link_libraries(a3_test PRIVATE ${sma})
