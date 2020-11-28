cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

set(sut sut)
set(socket socket)

add_library(
        ${sut} STATIC
        sut.c
        sut.h
)

add_library(
        ${socket} STATIC
        socket_lib.h
        socket_lib.c
)
set_target_properties(${sut} PROPERTIES PREFIX "")
set_target_properties(${socket} PROPERTIES PREFIX "")

add_executable(server small_server.c)
target_link_libraries(server PRIVATE ${socket})

foreach (test test1 test2 test3 test4 test5 test6)
    add_executable(${test} ${test}.c)
    target_link_libraries(${test} PRIVATE ${sut} ${socket} -pthread)
endforeach ()