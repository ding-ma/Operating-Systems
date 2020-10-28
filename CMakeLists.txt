cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

add_library(
        a2_sut SHARED
        Assignment-2/sut.c
        Assignment-2/sut.h
)

add_library(
        socket_lib SHARED
        Assignment-2/socket_lib.h
        Assignment-2/socket_lib.c
)

add_executable(server Assignment-1/small_server.c)
target_link_libraries(server PRIVATE socket_lib)

foreach (test test1 test2 test3 test4 test5 test6)
    add_executable(${test} Assignment-2/${test}.c)
    target_link_libraries(${test} a2_sut socket_lib -pthread)
endforeach ()