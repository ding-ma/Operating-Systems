cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

set(a1_lib a1_lib)
add_library(
        ${a1_lib} STATIC
        a1_lib.h
        a1_lib.c
)


add_executable(frontend Client.c)
add_executable(backend Server.c)
target_link_libraries(frontend PRIVATE ${a1_lib})
target_link_libraries(backend PRIVATE ${a1_lib})