cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

add_library(
        a2_sut
        Assignment-2/sut.c
        Assignment-2/sut.h
)

add_executable(test1 Assignment-2/test1.c)
target_link_libraries(test1 PRIVATE a2_sut)
target_include_directories(test1 PRIVATE Assignment-2/queue.h)

add_executable(scratch Assignment-2/scratch.c)