cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

add_executable(client Tutorial-1/Client.c)
add_executable(server Tutorial-1/Server.c)