cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

add_library(
        a1_lib
        Assignment-1/a1_lib.h
        Assignment-1/a1_lib.c
)


add_executable(frontend Assignment-1/Client.c)
add_executable(backend Assignment-1/Server.c)
target_link_libraries(frontend PRIVATE a1_lib)
target_link_libraries(backend PRIVATE a1_lib)