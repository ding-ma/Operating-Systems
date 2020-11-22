cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

#set(sma sma)
#
#add_library(
#        ${sma} STATIC
#        Assignment-3/sma.h
#        Assignment-3/sma.c
#)
#
#add_executable(a3_test Assignment-3/a3_test.c)
#target_link_libraries(a3_test PRIVATE ${sma})


add_library(
        gh_sma STATIC
        Scratch/malloc_api.c
)

add_executable(gh Scratch/malloc_test.c)
target_link_libraries(gh PRIVATE gh_sma)