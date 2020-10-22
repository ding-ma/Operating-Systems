cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

# todo generate object file only at the end
set(a2_lib PRIVATE
        a2_sut
        socket_lib
        pthread
        )

set(a2_queue PRIVATE Assignment-2/queue.h)

add_library(
        a2_sut
        Assignment-2/sut.c
        Assignment-2/sut.h
)

add_library(
        socket_lib
        Assignment-2/socket_lib.h
        Assignment-2/socket_lib.c
)

foreach (test test1 test2 test3 test4 test5)
    add_executable(${test} Assignment-2/${test}.c)
    target_link_libraries(${test} ${a2_lib})
    target_include_directories(${test} ${a2_queue})
endforeach ()