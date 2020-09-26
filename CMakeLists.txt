cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

add_library(
        rpc
        Assignment-1/a1_lib.h
        Assignment-1/a1_lib.c
)

#add_executable(client Sample/example_client.c)
#add_executable(server Sample/example_server.c)
add_executable(client Assignment-1/Client.c)
add_executable(server Assignment-1/Server.c)
target_link_libraries(client PRIVATE rpc)
target_link_libraries(server PRIVATE rpc)